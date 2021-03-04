#pragma once

//#define CRAFT_ENGINE_NET_HTTP_ZLIB_SUPPORT
//#define CRAFT_ENGINE_NET_HTTP_OPENSSL_SUPPORT


#ifdef _WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS

#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif //_CRT_NONSTDC_NO_DEPRECATE

#if defined(_MSC_VER)
#ifdef _WIN64
using ssize_t = __int64;
#else
using ssize_t = int;
#endif

#if _MSC_VER < 1900
#define snprintf _snprintf_s
#endif
#endif // _MSC_VER

#ifndef S_ISREG
#define S_ISREG(m) (((m)&S_IFREG) == S_IFREG)
#endif // S_ISREG

#ifndef S_ISDIR
#define S_ISDIR(m) (((m)&S_IFDIR) == S_IFDIR)
#endif // S_ISDIR

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#ifndef WSA_FLAG_NO_HANDLE_INHERIT
#define WSA_FLAG_NO_HANDLE_INHERIT 0x80
#endif

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

#ifndef strcasecmp
#define strcasecmp _stricmp
#endif // strcasecmp

using socket_t = SOCKET;
#ifdef CRAFT_ENGINE_NET_HTTP_USE_POLL
#define poll(fds, nfds, timeout) WSAPoll(fds, nfds, timeout)
#endif

#else // not _WIN32

#include <arpa/inet.h>
#include <cstring>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#ifdef CRAFT_ENGINE_NET_HTTP_USE_POLL
#include <poll.h>
#endif
#include <csignal>
#include <pthread.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

using socket_t = int;
#define INVALID_SOCKET (-1)
#endif //_WIN32

#include <array>
#include <atomic>
#include <cassert>
#include <climits>
#include <condition_variable>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <regex>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <unordered_map>
#include <iostream>
#include <queue>
#include <sstream>


#define CRAFT_ENGINE_NET_FTP_MAX_RECEIVE_BUFFER_SIZE 1024 * 20


namespace CraftEngine
{
    namespace net
    {


        namespace ftp
        {
			namespace detail
			{
				class Socket
				{
					friend class FtpServer;

				public:
					SOCKET m_socket = INVALID_SOCKET;
					std::string m_address;
					void clear()
					{
						if (m_socket != INVALID_SOCKET)
							closesocket(m_socket);
						m_socket = INVALID_SOCKET;
					}

					bool isValid()
					{
						return m_socket != INVALID_SOCKET;
					}

					bool bind(int port)
					{
						clear();
						m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						if (m_socket == INVALID_SOCKET) {
							wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
							return false;
						}

						sockaddr_in addrServer;
						addrServer.sin_family = AF_INET;
						addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
						addrServer.sin_port = htons(port);

						if (::bind(m_socket, (SOCKADDR*)&addrServer, sizeof(addrServer)) == SOCKET_ERROR) {
							wprintf(L"bind failed with error: %ld\n", WSAGetLastError());
							clear();
							return false;
						}
						return true;
					}

					bool listen()
					{
						if (::listen(m_socket, 5) == SOCKET_ERROR)
						{
							wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
							return false;
						}
						return true;
					}

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
							char str[INET_ADDRSTRLEN];
							const char* ptr = inet_ntop(AF_INET, &addrClient.sin_addr, str, sizeof(str));
							socket.m_address = str;
						}
						return socket;
					}

					bool connect(const char* address, uint16_t port)
					{
						clear();

						SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						if (ConnectSocket == INVALID_SOCKET) {
							return false;
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
							return false;
						}
						m_socket = ConnectSocket;
						return true;
					}

					int getId()
					{
						return (int)m_socket;
					}


				};



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






				struct Folder
				{
					std::string name;
					std::string full_path;
					uint64_t size;
					bool is_file;
				};



				std::list<Folder> getDirectoryFolderList(const std::string& path, const std::string& suffix = "")
				{
					std::list<Folder> folder_list;

					{
						WIN32_FIND_DATAA FindFileData;
						HANDLE hListFile;
						CHAR szFilePath[MAX_PATH];

						// 构造代表子目录和文件夹路径的字符串，使用通配符"*"
						strcpy_s(szFilePath, MAX_PATH, path.c_str());
						// 注释的代码可以用于查找所有以“.txt”结尾的文件
						// lstrcat(szFilePath, "\\*.txt");
						strcat_s(szFilePath, ("\\*" + suffix).c_str());

						// 查找第一个文件/目录，获得查找句柄
						hListFile = FindFirstFileA(szFilePath, &FindFileData);
						// 判断句柄
						if (hListFile == INVALID_HANDLE_VALUE)
						{
							printf("错误：%d\n", GetLastError());
						}
						else
						{
							do
							{
								// 如果不想显示代表本级目录和上级目录的“.”和“..”，
								// 可以使用注释部分的代码过滤
								if (lstrcmpA(FindFileData.cFileName, (".")) == 0 ||
									lstrcmpA(FindFileData.cFileName, ("..")) == 0)
								{
									continue;
								}

								Folder f;
								f.name = FindFileData.cFileName;
								f.full_path = path + "\\" + f.name;
								f.size = int64_t(int64_t(FindFileData.nFileSizeHigh) * (MAXDWORD + 1)) + FindFileData.nFileSizeLow;
								if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
									f.is_file = false;
								else
									f.is_file = true;

								folder_list.push_back(f);

							} while (FindNextFileA(hListFile, &FindFileData));
						}
						return folder_list;
					}



				}
			}
        }
    }
}