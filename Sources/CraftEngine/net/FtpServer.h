#pragma once
#include "FtpCommon.h"



namespace CraftEngine
{
	namespace net
	{


		namespace ftp
		{

			


			class FtpServer
			{
			private:
				detail::Thread m_listeningThread;
				detail::Socket m_listenSocket;

				struct Instance
				{
					int m_id;
					detail::Socket m_dtp_socket;
					int m_dtp_port;
					int m_created_time;
					int m_last_activated_time;
					int m_life_duration_time;
					int m_wait_timeout;
					std::atomic_bool m_dtp_is_using = false;
					std::string m_address;
					detail::Socket m_pi_socket;
					detail::Thread m_pi_thread;
					detail::Thread m_dtp_thread;
				};
				std::list<Instance*> m_instances;
				std::mutex m_mutex;

				std::list<Instance*> m_instancesTemp;

				bool m_listening = false;
				bool m_shouldStop = false;

				void clear()
				{
					m_shouldStop = true;
					std::lock_guard<std::mutex> lock(m_mutex);
					for (auto it : m_instances)
					{
						it->m_pi_socket.clear();
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

				void _Process_Dtp_Loop(Instance* ins)
				{

					char buf[CRAFT_ENGINE_NET_FTP_MAX_RECEIVE_BUFFER_SIZE + 1];
					while (ins->m_dtp_socket.isValid())
					{
						auto count = recv(ins->m_dtp_socket.m_socket, buf, CRAFT_ENGINE_NET_FTP_MAX_RECEIVE_BUFFER_SIZE, 0);
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
							_Process_Dtp_Data_Receive(ins, buf, count);
						}
					}
					ins->m_dtp_socket.clear();
				}


				// 接收到数据
				void _Process_Dtp_Data_Receive(Instance* ins, const void* data, int size)
				{



				}

				void _Process_Pi_Data_Receive(Instance* ins, const void* data, uint32_t size)
				{
					std::stringstream ss((char*)data);
					std::string cmd;
					ss >> cmd;
					if (cmd == "PASV")
					{
						if (!ins->m_dtp_socket.isValid())
						{
							std::cout << "Dtp Server: PASV." << std::endl;
							if (ins->m_dtp_socket.connect(ins->m_address.c_str(), 20))
							{
								std::cout << "Dtp Server: Connect." << std::endl;
								ins->m_dtp_thread.push([=] {_Process_Dtp_Loop(ins); });
								std::string response = "200";
								_Process_Pi_Data_Send(ins, response.data(), response.length());
								return;
							}
						}
					}
					else if (cmd == "LIST")
					{
						if (ins->m_dtp_socket.isValid() && !ins->m_dtp_is_using)
						{
							ins->m_dtp_is_using = true;
							char cur_path[256];
							GetCurrentDirectoryA(256, cur_path);
							auto&& floder_list = detail::getDirectoryFolderList(cur_path);
							std::string msg;
							for (auto it : floder_list)
							{
								msg += it.name;
								msg += "\n";
							}
							std::cout << "file list = /n" << msg << std::endl;
							if (_Process_Dtp_Data_Send(ins, msg.data(), msg.length()))
								std::cout << "msg send successfully" << std::endl;
							else
								std::cout << "msg send faild" << std::endl;
							ins->m_dtp_is_using = false;
							std::string response = "200";
							_Process_Pi_Data_Send(ins, response.data(), response.length());
							return;
						}
					}
					else if (cmd == "PWD")
					{
						if (ins->m_dtp_socket.isValid() && !ins->m_dtp_is_using)
						{
							ins->m_dtp_is_using = true;
							char cur_path[256];
							GetCurrentDirectoryA(256, cur_path);		
							std::string path(cur_path);
							std::cout << "cur path = /n" << path << std::endl;
							if (_Process_Dtp_Data_Send(ins, path.data(), path.length()))
								std::cout << "msg send successfully" << std::endl;
							else
								std::cout << "msg send faild" << std::endl;
							ins->m_dtp_is_using = false;
							std::string response = "200";
							_Process_Pi_Data_Send(ins, response.data(), response.length());
							return;
						}
					}
					else if (cmd == "ABOR")
					{
						if (ins->m_dtp_socket.isValid() && !ins->m_dtp_is_using)
						{
							ins->m_dtp_is_using = true;
							ins->m_dtp_socket.clear();
							ins->m_dtp_is_using = false;
							std::string response = "200";
							_Process_Pi_Data_Send(ins, response.data(), response.length());
							return;
						}
					}

					{
						std::string response = "202";
						_Process_Pi_Data_Send(ins, response.data(), response.length());
					}

				}


				// 接收到数据
				bool _Process_Dtp_Data_Send(Instance* ins, const void* data, int size)
				{
					int size_remain = size;
					int count, currentPosition = 0;
					while (size_remain > 0 && (count = ::send(ins->m_dtp_socket.m_socket, (char*)data + currentPosition, size_remain, 0)) != SOCKET_ERROR)
					{
						size_remain -= count;
						currentPosition += count;
					}
					if (count != SOCKET_ERROR)
						return true;
					else
						return false;
				}
				

				bool _Process_Pi_Data_Send(Instance* ins, const void* data, uint32_t size)
				{
					int size_remain = size;
					int count, currentPosition = 0;
					while (size_remain > 0 && (count = ::send(ins->m_pi_socket.m_socket, (char*)data + currentPosition, size_remain, 0)) != SOCKET_ERROR)
					{
						size_remain -= count;
						currentPosition += count;
					}
					if (count != SOCKET_ERROR)
						return true;
					else
						return false;
				}

				void _Process_Pi_Loop(Instance* ins)
				{
					char buf[CRAFT_ENGINE_NET_FTP_MAX_RECEIVE_BUFFER_SIZE + 1];
					while (ins->m_pi_socket.isValid()) {

						int count = recv(ins->m_pi_socket.m_socket, buf, CRAFT_ENGINE_NET_FTP_MAX_RECEIVE_BUFFER_SIZE, 0);
						if (count == 0)
						{

						}
						else if (count == SOCKET_ERROR)
						{
							std::cout << "Client[" << ins->m_pi_socket.getId() << "]: Lost connection." << std::endl;
							break;
						}
						else if (count > 0)
						{
							buf[count] = 0;
							std::cout << "Client[" << ins->m_pi_socket.m_address << ":" << ins->m_pi_socket.getId() << "]: Message:\n" << buf << "." << std::endl;
							_Process_Pi_Data_Receive(ins, buf, count);
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}
					onClientDisconnect(ins->m_pi_socket.getId());
					ins->m_pi_socket.clear();
					if (ins->m_dtp_socket.isValid())
						ins->m_dtp_socket.clear();
					{
						std::lock_guard<std::mutex> lock(m_mutex);
						for (auto it = m_instances.begin(); it != m_instances.end(); it++)
						{
							if (*it == ins)
							{
								m_instances.erase(it);
								break;
							}
						}
						m_instancesTemp.push_back(ins);
					}

				}







				void _Listen_Loop()
				{
					m_listening = true;
					while (!m_shouldStop)
					{
						detail::Socket socket = m_listenSocket.accept();
						if (socket.isValid())
						{
							std::cout << "Client[" << socket.m_address << ":" << socket.getId() << "]: Connected." << std::endl;
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
							}
							ins->m_pi_socket = socket;
							ins->m_id = socket.getId();
							ins->m_created_time = time(0);
							ins->m_wait_timeout = 1000;
							ins->m_last_activated_time = ins->m_created_time;
							ins->m_life_duration_time = 60000;
							ins->m_dtp_socket = detail::Socket();
							ins->m_dtp_port = -1;
							ins->m_address = socket.m_address;
							ins->m_pi_thread.push([=] {_Process_Pi_Loop(ins); });
							{
								std::lock_guard<std::mutex> lock(m_mutex);
								m_instances.push_back(ins);
							}

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
					std::cout << "Server[" << m_listenSocket.m_address << ":" << m_listenSocket.getId() << "]: Start listening port:" << port << "." << std::endl;
					m_listeningThread.push([&] {_Listen_Loop(); });
				}

				void wait()
				{
					m_listeningThread.wait();
				}

				//bool send(int id, const void* data, int size)
				//{
				//	Instance* ins = nullptr;
				//	{
				//		std::lock_guard<std::mutex> lock(m_mutex);
				//		for (auto it = m_instances.begin(); it != m_instances.end(); it++)
				//		{
				//			if ((*it)->m_pi_socket.m_socket == id)
				//			{
				//				ins = *it;
				//			}
				//		}
				//	}
				//	if (ins != nullptr)
				//	{
				//		int size_remain = size;
				//		int count, currentPosition = 0;
				//		while (size_remain > 0 && (count = ::send(ins->m_pi_socket.m_socket, (char*)data + currentPosition, size_remain, 0)) != SOCKET_ERROR)
				//		{
				//			size_remain -= count;
				//			currentPosition += count;
				//		}
				//		if (count != SOCKET_ERROR)
				//			return true;
				//		else
				//			return false;
				//	}
				//	return false;
				//}

				//void close(int id)
				//{
				//	Instance* ins = nullptr;
				//	{
				//		std::lock_guard<std::mutex> lock(m_mutex);
				//		for (auto it = m_instances.begin(); it != m_instances.end(); it++)
				//		{
				//			if ((*it)->m_pi_socket.m_socket == id)
				//			{
				//				ins = *it;
				//			}
				//		}
				//	}
				//	if (ins != nullptr)
				//	{
				//		ins->m_pi_socket.clear();
				//	}
				//}

			protected:
				virtual void onClientConnect(int id)
				{

				}

				virtual void onClientDisconnect(int id)
				{

				}



			};






		}



	}
}

