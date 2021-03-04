#pragma once

#include "../advance/win32/Win32Api.h"

#define CRAFT_ENGINE_SERVER_BUFFER_SIZE 1024 * 20

namespace CraftEngine
{
	namespace net
	{

		// 线程
		class Thread
		{
		private:
			void runningLoop()
			{
				std::function<void(void)> default_task;
				std::function<void(void)>& task = default_task;
				while (true)
				{
					{
						std::unique_lock<std::mutex> lock(m_queueMutex);
						m_condition.wait(lock, [this]() { return !m_tasksQueue.empty() || m_destroying; });
						if (m_destroying)
							break;
						task = m_tasksQueue.front();
					}
					task();
					{
						std::lock_guard<std::mutex> lock(m_queueMutex);
						m_tasksQueue.pop();
						m_condition.notify_one();
					}
				}
			}

		public:
			Thread()
			{
				m_thread = std::thread(&Thread::runningLoop, this);
			}

			~Thread()
			{
				if (m_thread.joinable())
				{
					wait();
					m_queueMutex.lock();
					m_destroying = true;
					m_condition.notify_one();
					m_queueMutex.unlock();
					m_thread.join();
				}
			}

			void push(std::function<void()> task)
			{
				std::lock_guard<std::mutex> lock(m_queueMutex);
				m_tasksQueue.push(std::move(task));
				m_condition.notify_one();
			}

			void clear()
			{
				std::lock_guard<std::mutex> lock(m_queueMutex);
				while (m_tasksQueue.size() > 0)
					m_tasksQueue.pop();
			}

			void wait()
			{
				std::unique_lock<std::mutex> lock(m_queueMutex);
				m_condition.wait(lock, [this]() { return m_tasksQueue.empty(); });
			}

			uint32_t count()
			{
				std::unique_lock<std::mutex> lock(m_queueMutex);
				return m_tasksQueue.size();
			}

		private:
			bool m_destroying = false;
			std::thread m_thread;
			std::queue<std::function<void(void)>> m_tasksQueue;
			std::mutex m_queueMutex;
			std::condition_variable m_condition;
		};




		class Socket
		{
			friend class Server;
			friend class Client;
			SOCKET m_socket = INVALID_SOCKET;

		public:

			// 关闭
			void clear()
			{
				if (m_socket != INVALID_SOCKET)
					closesocket(m_socket);
				m_socket = INVALID_SOCKET;
			}

			// 可用的
			bool isValid()
			{
				return m_socket != INVALID_SOCKET;
			}

			// 绑定端口（作为服务器时使用）
			void bind(int port)
			{
				clear();
				m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (m_socket == INVALID_SOCKET) {
					wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
					throw std::runtime_error("");
				}

				sockaddr_in addrServer;
				addrServer.sin_family = AF_INET;
				addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
				addrServer.sin_port = htons(port);

				if (::bind(m_socket, (SOCKADDR*)&addrServer, sizeof(addrServer)) == SOCKET_ERROR) {
					wprintf(L"bind failed with error: %ld\n", WSAGetLastError());
					clear();
					throw std::runtime_error("");
				}
			}

			// 监听（作为服务器时使用）
			bool listen()
			{
				if (::listen(m_socket, 5) == SOCKET_ERROR)
				{
					wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
					return false;
				}
				return true;
			}

			// 接收到socket连接（作为服务器时使用）
			Socket accept()
			{
				Socket socket;
				SOCKADDR_IN addrClient;
				int len = sizeof(SOCKADDR);
				SOCKET accept_socket = ::accept(m_socket, (SOCKADDR*)&addrClient, &len);
				if (accept_socket == INVALID_SOCKET)
				{

				}
				else
				{
					socket.m_socket = accept_socket;
				}
				return socket;
			}

			// 连接一个主机（作为客户端时使用）
			bool connect(const char* address, uint16_t port)
			{
				clear();

				SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (ConnectSocket == INVALID_SOCKET) {
					throw std::runtime_error(std::string() + "unable to create socket, error code:" + std::to_string(WSAGetLastError()));
				}
				struct in_addr s;
				inet_pton(AF_INET, address, &s);
				sockaddr_in addrServer;
				addrServer.sin_family = AF_INET;
				addrServer.sin_addr.s_addr = s.s_addr;
				addrServer.sin_port = htons(port);

				auto iResult = ::connect(ConnectSocket, (SOCKADDR*)&addrServer, sizeof(addrServer));
				if (iResult == SOCKET_ERROR) {
					clear();
					throw std::runtime_error(std::string() + "unable to connect to server, error code:" + std::to_string(WSAGetLastError()));
				}
				m_socket = ConnectSocket;
				return true;
			}

			// 获取id，该id为系统分配
			int getId()
			{
				return (int)m_socket;
			}


		};



		class Server
		{
		private:
			util::Thread m_listeningThread;
			Socket m_listenSocket;

			struct Instance
			{
				Socket m_socket;
				Thread m_thread;
			};
			std::list<Instance*> m_instances;
			std::mutex m_mutex;
			std::list<Instance*> m_instancesTemp;

			bool m_listening = false;
			bool m_shouldStop = false;

			void clear()
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				for (auto it : m_instances)
				{
					(*it).m_socket.clear();
				}
				for (auto it : m_instancesTemp)
				{
					delete it;
				}
				m_instancesTemp.clear();
				for (auto it : m_instances)
				{
					delete it;
				}
				m_instances.clear();
			}

			void _Process_Loop(Instance* ins)
			{
				char buf[CRAFT_ENGINE_SERVER_BUFFER_SIZE + 1];
				while (ins->m_socket.isValid()) {

					int count = recv(ins->m_socket.m_socket, buf, CRAFT_ENGINE_SERVER_BUFFER_SIZE, 0);
					if (count == 0)
					{

					}
					else if (count == SOCKET_ERROR)
					{
						std::cout << "Client[" << ins->m_socket.getId() << "]: Lost connection." << std::endl;
						break;
					}
					else if (count > 0)
					{
						buf[count] = 0;
						std::cout << "Client[" << ins->m_socket.getId() << "]: Message:\n" << buf << "." << std::endl;
						onReceiveData(ins->m_socket.getId(), buf, count);
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
				onClientDisconnect(ins->m_socket.getId());
				ins->m_socket.clear();
				{
					std::lock_guard<std::mutex> lock(m_mutex);
					for (auto it = m_instances.begin(); it != m_instances.end(); it++)
					{
						if (*it == ins)
						{
							m_instances.erase(it);
						}
						break;
					}
				}
				m_instancesTemp.push_back(ins);
			}


			void _Listen_Loop()
			{
				m_listening = true;
				while (!m_shouldStop)
				{
					Socket socket = m_listenSocket.accept();
					if (socket.isValid())
					{
						std::cout << "Client[" << socket.getId() << "]: Connected." << std::endl;
						onClientConnect(socket.getId());
						Instance* ins = nullptr;
						{
							std::lock_guard<std::mutex> lock(m_mutex);
							if (m_instancesTemp.size() > 0)
							{
								ins = m_instancesTemp.front();
								m_instancesTemp.pop_front();
							}
							else
							{
								ins = new Instance;
							}
							m_instances.push_back(ins);
						}
						ins->m_socket = socket;
						ins->m_thread.push([=] {_Process_Loop(ins); });
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
				m_listening = false;
			}
		public:

			void listen(int port)
			{
				WSADATA wsa_data;
				int code = WSAStartup(MAKEWORD(2, 2), &wsa_data);
				if (code != NO_ERROR)
				{
					throw std::runtime_error("WSAStartup failed, error code:" + std::to_string(code));
				}
				if (code != NO_ERROR)
				{
					throw std::runtime_error("WSAStartup failed, error code:" + std::to_string(code));
				}
				m_listenSocket.bind(port);
				m_listenSocket.listen();
				std::cout << "Server[" << m_listenSocket.getId() << "]: Start listening port:" << port << "." << std::endl;
				m_listeningThread.push([&] {_Listen_Loop(); });
			}

			void wait()
			{
				m_listeningThread.wait();
			}

			bool send(int id, const void* data, int size)
			{
				Instance* ins = nullptr;
				{
					std::lock_guard<std::mutex> lock(m_mutex);
					for (auto it = m_instances.begin(); it != m_instances.end(); it++)
					{
						if ((*it)->m_socket.m_socket == id)
						{
							ins = *it;
						}
					}
				}
				if (ins != nullptr)
				{
					int size_remain = size;
					int count, currentPosition = 0;
					while (size_remain > 0 && (count = ::send(ins->m_socket.m_socket, (char*)data + currentPosition, size_remain, 0)) != SOCKET_ERROR)
					{
						size_remain -= count;
						currentPosition += count;
					}
					if (count != SOCKET_ERROR)
						return true;
					else
						return false;
				}
				return false;
			}

			void close(int id)
			{
				Instance* ins = nullptr;
				{
					std::lock_guard<std::mutex> lock(m_mutex);
					for (auto it = m_instances.begin(); it != m_instances.end(); it++)
					{
						if ((*it)->m_socket.m_socket == id)
						{
							ins = *it;
						}
					}
				}
				if (ins != nullptr)
				{
					ins->m_socket.clear();
				}
			}

		protected:
			virtual void onClientConnect(int id)
			{

			}

			virtual void onClientDisconnect(int id)
			{

			}

			virtual void onReceiveData(int id, const void* data, uint32_t size)
			{

			}

		};


		class Client
		{
		private:
			Socket m_socket;
			Thread m_thread;


			void _Accept_Loop()
			{
				char buf[128 + 1];
				while (m_socket.isValid())
				{
					auto count = recv(m_socket.m_socket, buf, 128, 0);
					if (count == 0)
					{

					}
					else if (count == SOCKET_ERROR)
					{
						std::cout << "Client: Connect lost." << std::endl;
						break;
					}
					else
					{
						buf[count] = '\0';
						std::cout << "Server: Message:" << buf << std::endl;
						onReceiveData(buf, count);
					}
				}
				clear();
			}

		public:

			void clear()
			{
				m_socket.clear();
			}

			bool connect(const char* address, uint16_t port)
			{
				WSADATA wsa_data;
				int code = WSAStartup(MAKEWORD(2, 2), &wsa_data);
				if (code != NO_ERROR)
				{
					return false;
				}
				if (m_socket.connect(address, port))
				{
					std::cout << "Client: Connect successfully." << std::endl;
					onConnectServer();
					m_thread.push([=] { _Accept_Loop(); });
				}
			}

			bool send(const void* data, int size)
			{
				int size_remain = size;
				int count, currentPosition = 0;
				while (size_remain > 0 && (count = ::send(m_socket.m_socket, (char*)data + currentPosition, size_remain, 0)) != SOCKET_ERROR)
				{
					size_remain -= count;
					currentPosition += count;
				}
				if (count != SOCKET_ERROR)
					return true;
				else
					return false;
			}

			void wait()
			{
				m_thread.wait();
			}
		protected:

			virtual void onConnectServer()
			{

			}

			virtual void onReceiveData(const void* data, uint32_t size)
			{

			}
		};


	}
}

