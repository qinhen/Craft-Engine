#pragma once

#include "FtpCommon.h"

namespace CraftEngine
{
	namespace net
	{


		namespace ftp
		{



			class FtpClient
			{
			private:
				detail::Socket m_pi_socket;
				detail::Socket m_dtp_listen_socket;
				detail::Socket m_dtp_socket;
				detail::Thread m_thread;
				detail::Thread m_dtp_thread;
				bool m_shouldStop = false;
				bool m_listening = false;
				std::mutex m_pi_wait_mutex;
				std::atomic_bool m_pi_is_waiting = false;
				enum MsgType
				{
					MsgType_Nothing,
					MsgType_Data_Channal_Connect,
					MsgType_File_List,
					MsgType_Current_Path,
					MsgType_Abort,
					MsgType_File,
				};
				MsgType m_dtp_waiting_type = MsgType_Nothing;

				std::condition_variable m_wait_condition;

				void _Process_Dtp_Data_Receive(const void* data, int size)
				{
					switch (m_dtp_waiting_type)
					{
					case MsgType_File_List:
					{
						std::stringstream code((char*)data);
						std::list<std::string> file_list;
						std::string temp;
						std::cout << "[" << std::this_thread::get_id() << ":Message]: Dtp Server response flie list:\n";
						while (code >> temp)
						{
							file_list.push_back(temp);
						}
						int i = 1;
						for (auto it : file_list)
						{
							std::cout << " [" << i << "] " << it << "\n";
							i++;
						}
						break;
					}
					case MsgType_Current_Path:
					{
						std::string code((char*)data);
						std::cout << "[" << std::this_thread::get_id() << ":Message]: Dtp Server current path:\n " << code << std::endl;
						break;
					}

					}

				}

				void _Process_Pi_Data_Receive(const void* data, int size)
				{
					std::string code((char*)data);
					std::cout << "[" << std::this_thread::get_id() << ":Message]: Pi Server response code:" << code << "." << std::endl;
					m_pi_is_waiting = false;
					m_wait_condition.notify_one();
				}


				void _Dtp_Process_Loop()
				{

					m_listening = true;
					while (!m_shouldStop)
					{
						if (!m_dtp_socket.isValid())
						{
							m_dtp_socket = m_dtp_listen_socket.accept();
							std::cout << "[" << std::this_thread::get_id() << ":Message]: Dtp Server connect successfully." << std::endl;
						}
						else
						{
							char buf[CRAFT_ENGINE_NET_FTP_MAX_RECEIVE_BUFFER_SIZE + 1];
							auto count = recv(m_dtp_socket.m_socket, buf, CRAFT_ENGINE_NET_FTP_MAX_RECEIVE_BUFFER_SIZE, 0);
							if (count == 0)
							{

							}
							else if (count == SOCKET_ERROR)
							{
								std::cout << "[" << std::this_thread::get_id() << ":Message]: Dtp Server connection lost." << std::endl;
								m_dtp_socket.clear();
							}
							else
							{
								buf[count] = '\0';
								_Process_Dtp_Data_Receive(buf, count);
							}	
						}		
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}
					m_listening = false;
				}

				void _Pi_Accept_Loop()
				{
					char buf[CRAFT_ENGINE_NET_FTP_MAX_RECEIVE_BUFFER_SIZE + 1];
					while (m_pi_socket.isValid())
					{
						auto count = recv(m_pi_socket.m_socket, buf, CRAFT_ENGINE_NET_FTP_MAX_RECEIVE_BUFFER_SIZE, 0);
						if (count == 0)
						{

						}
						else if (count == SOCKET_ERROR)
						{
							std::cout << "[" << std::this_thread::get_id() << ":Message]: Pi Server connect lost." << std::endl;
							break;
						}
						else
						{
							buf[count] = '\0';
							_Process_Pi_Data_Receive(buf, count);
						}
					}
					clear();
				}

			public:

				void clear()
				{
					std::cout << "[" << std::this_thread::get_id() << ":Message]: Clear called." << std::endl;
					m_pi_socket.clear();
					m_dtp_listen_socket.clear();
					m_dtp_socket.clear();
					m_shouldStop = true;
				}

				bool connect(const char* address)
				{
					WSADATA wsa_data;
					int code = WSAStartup(MAKEWORD(2, 2), &wsa_data);
					if (code != NO_ERROR)
					{
						return false;
					}
					if (m_pi_socket.connect(address, 21))
					{
						std::cout << "[" << std::this_thread::get_id() << ":Message]: Pi Server connect successfully." << std::endl;
						m_thread.push([=] { _Pi_Accept_Loop(); });
						if (m_dtp_listen_socket.bind(20))
						{
							if (m_dtp_listen_socket.listen())
							{
								m_dtp_thread.push([=] { _Dtp_Process_Loop(); });
							}
						}
						return true;
					}
					clear();
					return false;
				}


				bool sendCommand(const void* data, int size)
				{
					if (m_pi_is_waiting)
						return false;

					std::stringstream cmd((char*)data);
					std::string cmd_type;
					cmd >> cmd_type;
					if (cmd_type == "LIST")
						m_dtp_waiting_type = MsgType_File_List;
					else if (cmd_type == "RETR")
						m_dtp_waiting_type = MsgType_File;
					else if (cmd_type == "PWD")
						m_dtp_waiting_type = MsgType_Current_Path;
					else if (cmd_type == "PASV")
						m_dtp_waiting_type = MsgType_Data_Channal_Connect;
					else if (cmd_type == "ABOR")
						m_dtp_waiting_type = MsgType_Abort;
					else
						m_dtp_waiting_type = MsgType_Nothing;

					if (!m_dtp_waiting_type == MsgType_Nothing)
					{
						int size_remain = size;
						int count, currentPosition = 0;
						while (size_remain > 0 && (count = ::send(m_pi_socket.m_socket, (char*)data + currentPosition, size_remain, 0)) != SOCKET_ERROR)
						{
							size_remain -= count;
							currentPosition += count;
						}

						m_pi_is_waiting = true;
						std::unique_lock<std::mutex> lock(m_pi_wait_mutex);
						m_wait_condition.wait(lock, [this]() { return !m_pi_is_waiting || !m_pi_socket.isValid(); });

						std::cout << "[" << std::this_thread::get_id() << ":Message]: Command Finished.\n" << std::endl;

						if (count != SOCKET_ERROR)
							return true;
						else
							return false;
					}
					return false;
				}

				void wait()
				{
					m_thread.wait();
				}
			protected:

			};





		}


	}
}

