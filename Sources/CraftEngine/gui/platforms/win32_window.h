#pragma once
#ifdef CRAFT_ENGINE_GUI_APPLICATION
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif

#include "../widgets/Widget.h"
#include "../../advance/win32/Win32Api.h"

namespace CraftEngine
{
	namespace gui
	{

		class Win32Window: public RootWidget
		{
		private:
			HWND     m_hWnd;
			wchar_t  m_windowClassName[256];
			wchar_t  m_windowTitleName[256];
			uint32_t m_windowWidth;
			uint32_t m_windowHeight;
			bool     m_windowIsFullscreen = false;
			bool     m_windowIsActive = true;
			bool     m_windowIsDestroy = false;
			bool     m_isSystemCursorHidden = false;
			int32_t  m_windowMouseCoordX;
			int32_t  m_windowMouseCoordY;

			int32_t  m_tempWindowWidth;
			int32_t  m_tempWindowHeight;

			std::chrono::system_clock::time_point m_windowSystemTimeStamp;
			std::wstring m_windowSystemInputCompositionString;
			std::vector<std::wstring> m_windowSystemInputCandidateStringList;

			struct THUNK
			{
#if defined(_M_IX86)
#pragma pack(push, 1)
				DWORD m_mov; // = 0x042444C7
				DWORD m_this; // = this
				BYTE  m_jmp; // = 0xe9
				DWORD m_relproc; // = relative distance
#elif defined(_M_X64)
#pragma pack(push, 2)
				USHORT  RcxMov;         // mov rcx, pThis
				ULONG64 RcxImm;         //
				USHORT  RaxMov;         // mov rax, target
				ULONG64 RaxImm;         //
				USHORT  RaxJmp;         // jmp target
#else
#error "unsurport instruction set!"
#endif
			}*thunk;
#pragma pack(pop)

		public:
			Win32Window(
				const std::wstring& title = L"",
				uint32_t    width = 1280,
				uint32_t    height = 720,
				const Win32Window* parent = nullptr
			) : RootWidget(nullptr)
			{
				m_hWnd = (nullptr);
				thunk = (nullptr);



				static int windows_count = 0;

				//if (windows_count != 0 && parent == nullptr)
				//	throw std::exception("子窗口必须指定父窗口", -1);

				thunk = (THUNK*)VirtualAlloc(NULL, sizeof(THUNK), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#if defined(_M_IX86)
				thunk->m_mov = 0x042444c7;
				thunk->m_this = (DWORD)this;
				thunk->m_jmp = 0xe9;
				thunk->m_relproc = (DWORD)(&Win32Window::StaticWndProc) - ((DWORD)thunk + sizeof(THUNK));
#elif defined(_M_X64)
				thunk->RcxMov = 0xb948;          // mov rcx, pThis
				thunk->RcxImm = (ULONG64)this;   //
				thunk->RaxMov = 0xb848;          // mov rax, target
				thunk->RaxImm = (ULONG64)&Win32Window::StaticWndProc;
				thunk->RaxJmp = 0xe0ff;
#else
				// 不受支持的机器指令集
#error unsurport instruction set!
#endif
				FlushInstructionCache(GetCurrentProcess(), this, sizeof(THUNK));


				wcscpy_s(m_windowClassName, 256, L"CraftEngineWindow");
				wcscat_s(m_windowClassName, 256, std::to_wstring(windows_count++).c_str());
				wcscpy_s(m_windowTitleName, 256, title.c_str());
				m_windowIsFullscreen = false;

				WNDCLASSEXW wcex;

				std::wstring wsClassName = m_windowClassName;
				std::wstring wsTitleName = m_windowTitleName;

				const wchar_t* lpszClassName = wsClassName.c_str();
				const wchar_t* lpszTitleName = wsTitleName.c_str();

				wcex.cbSize = sizeof(WNDCLASSEX);
				wcex.style = CS_HREDRAW | CS_VREDRAW |
					CS_DBLCLKS // Double Click Support
					;
				wcex.lpfnWndProc = Win32Window::TempWndProc;
				wcex.cbClsExtra = 0;
				wcex.cbWndExtra = 0;
				wcex.hInstance = GetModuleHandle(NULL);
				wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
				wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
				wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
				wcex.lpszMenuName = NULL;
				wcex.lpszClassName = lpszClassName;
				wcex.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
				if (RegisterClassExW(&wcex) == FALSE)
					throw std::exception("注册窗口类失败", -1);

				// 预设窗口大小
				int screenWidth = GetSystemMetrics(SM_CXSCREEN);
				int screenHeight = GetSystemMetrics(SM_CYSCREEN);

				DWORD dwExStyle;
				DWORD dwStyle;
				dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
				dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				RECT windowRect = {};
				m_windowWidth = (long)width;
				m_windowHeight = (long)height;
				windowRect.left = ((long)screenWidth - m_windowWidth) / 2;
				windowRect.top = ((long)screenHeight - m_windowHeight) / 2;
				windowRect.right = windowRect.left + m_windowWidth;
				windowRect.bottom = windowRect.top + m_windowHeight;

				AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

				//CreateWindow的最后一个参数为this指针
				m_hWnd = CreateWindowExW(0L, lpszClassName, lpszTitleName, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
					windowRect.left, windowRect.top, m_windowWidth, m_windowHeight, parent != nullptr ? parent->m_hWnd : nullptr, NULL, GetModuleHandle(NULL), this);

				if (m_hWnd == NULL)
				{
					throw std::runtime_error("");
				}
				UpdateWindow(m_hWnd);

				RECT rect;
				GetClientRect(this->m_hWnd, &rect);
				m_windowWidth = rect.right - rect.left;
				m_windowHeight = rect.bottom - rect.top;

				DragAcceptFiles(m_hWnd, TRUE);
				ChangeWindowMessageFilterEx(m_hWnd, WM_DROPFILES, MSGFLT_ADD, 0);
				ChangeWindowMessageFilterEx(m_hWnd, WM_COPYDATA, MSGFLT_ADD, 0);       // 0x0049 == WM_COPYGLOBALDATA

				showWindow();
			}

			virtual ~Win32Window()
			{
				DestroyWindow(m_hWnd);
				VirtualFree(thunk, sizeof(THUNK), MEM_RELEASE);
			}

			HWND     getHWnd() const { return m_hWnd; }
			uint32_t getWindowWidth() const { return m_windowWidth; }
			uint32_t getWindowHeight() const { return m_windowHeight; }

			const std::wstring& getInputCompositionString()const { return m_windowSystemInputCompositionString; }
			const std::vector<std::wstring>& getInputCandidateStringList()const { return m_windowSystemInputCandidateStringList; }

			bool isFullScreen()const { return m_windowIsFullscreen; }


			virtual Size getWindowSize()override
			{
				Size size;
				RECT clientRect;
				GetClientRect(m_hWnd, &clientRect);
				size.x = clientRect.right - clientRect.left;
				size.y = clientRect.bottom - clientRect.top;
				return size;
			}

			virtual void setWindowSize(const Size& size) override
			{
				if (m_windowIsFullscreen)
					return;

				int real_width = size.x;
				int real_height = size.y;

				RECT clientRect;
				GetClientRect(m_hWnd, &clientRect);
				RECT rect;
				GetWindowRect(m_hWnd, &rect);

				int width_ex = (rect.right - rect.left) - (clientRect.right - clientRect.left);
				int height_ex = (rect.bottom - rect.top) - (clientRect.bottom - clientRect.top);

				real_width += width_ex;
				real_height += height_ex;


				MoveWindow(m_hWnd, (::GetSystemMetrics(SM_CXSCREEN) - real_width) / 2, (::GetSystemMetrics(SM_CYSCREEN) - real_height) / 2, real_width, real_height, false);

				m_windowWidth = rect.right - rect.left;
				m_windowHeight = rect.bottom - rect.top;

			}

			void setWindowFullScreen(bool fullscreen)
			{
				if (m_windowIsFullscreen == fullscreen)
					return;
				if (fullscreen) {
					RECT clientRect;
					GetWindowRect(m_hWnd, &clientRect);
					m_tempWindowWidth = clientRect.right - clientRect.left;
					m_tempWindowHeight = clientRect.bottom - clientRect.top;
					SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) & ~WS_CAPTION);
					ShowWindow(m_hWnd, SW_MAXIMIZE);
					m_windowIsFullscreen = fullscreen;
				}
				else {
					m_windowIsFullscreen = fullscreen;
					SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) | WS_CAPTION);
					SetWindowPos(m_hWnd, NULL, 0, 0, m_tempWindowWidth, m_tempWindowHeight, SWP_SHOWWINDOW | SWP_DRAWFRAME);
					ShowWindow(m_hWnd, SW_NORMAL);
				}
			}

			void moveWindowToCenter()
			{
				RECT rect;
				GetWindowRect(this->m_hWnd, &rect);
				uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
				uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2;
				SetWindowPos(m_hWnd, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}

			void showWindow()
			{
				ShowWindow(m_hWnd, SW_SHOW);
				SetForegroundWindow(m_hWnd);
				SetFocus(m_hWnd);
			}

			void hideWindow()
			{
				ShowWindow(m_hWnd, SW_HIDE);
			}

			void setWindowTitle(const wchar_t* title)
			{
				wcscpy_s(m_windowTitleName, 256, title);
				SetWindowTextW(m_hWnd, m_windowTitleName);
				//SetConsoleTitle(m_windowTitleName);
			}

			virtual bool onDrawFrame(float duration) = 0;

			void exec()
			{
				showWindow();

				MSG msg;//Windows消息结构
				while (1)
				{
					if (PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE)) //是否有消息在等待
					{
						if (m_shouldRepaint <= 0)
							m_shouldRepaint = 1;
						if (m_windowIsDestroy == true) //收到退出消息
						{
							if (m_hWnd && !DestroyWindow(m_hWnd)) //能否销毁窗口
								throw std::runtime_error("释放窗口句柄失败。");
							if (!UnregisterClassW(m_windowClassName, GetModuleHandle(0))) //能否注销窗口类
								throw std::runtime_error("不能注销窗口类。");
							break;
						}
						else //处理窗口消息
						{
							TranslateMessage(&msg);  //翻译消息
							DispatchMessage(&msg);   //发送消息
						}
					}
					else //如果没有消息
					{
						auto system_clock_now = std::chrono::system_clock::now();
						float duration_ms = fabsf(std::chrono::duration_cast<std::chrono::nanoseconds>(system_clock_now - m_windowSystemTimeStamp).count() / 1000000.0f);
						if (duration_ms > 1000)
							duration_ms = 1000;

						if (m_windowIsActive)
						{
							if (!onDrawFrame(duration_ms))
								Sleep(100);
						}
						else
						{
							Sleep(100);
						}
						m_windowSystemTimeStamp = system_clock_now;
					}
				}


			}

			void exit()
			{
				m_windowIsDestroy = true;
			}

			virtual bool isSystemCursorHide()override
			{
				return m_isSystemCursorHidden;
			}
			virtual void setSystemCursorHide(bool enable)override
			{
				if (m_isSystemCursorHidden != enable)
				{
					ShowCursor(!enable);
				}
				m_isSystemCursorHidden = enable;
			}

		private:

			// 辅助回调函数, Executed only when window created
			static LRESULT CALLBACK TempWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				if (message == WM_CREATE)//此处使用WM_NCCREATE消息也可以
				{
					//提取出对象指针
					Win32Window* w = (Win32Window*)((LPCREATESTRUCT)lParam)->lpCreateParams;
					WNDPROC pWndProc = (WNDPROC)w->thunk;

#if defined(_M_IX86)
					SetWindowLong(hWnd, GWLP_WNDPROC, (LONG)pWndProc);//
#elif defined(_M_X64)
					SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)pWndProc);//
#else
#error unsurport instruction set!
#endif
					return pWndProc(hWnd, message, wParam, lParam);
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}

			static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				return ((Win32Window*)hWnd)->WndProc(message, wParam, lParam);
			}

			bool m_track_mouse_leave = false;
			//bool m_track_mouse_hover = false;
			// 主回调函数
			virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam)
			{

				//std::cout << "msg:" << message << std::endl;
				HDC hdc = nullptr;
				PAINTSTRUCT ps = {};
				RECT rect = {};
				POINT pt = {};
				switch (message)
				{
				case WM_MOUSEMOVE:
				{
					if (!m_track_mouse_leave)
					{
						TRACKMOUSEEVENT tme;
						tme.cbSize = sizeof(TRACKMOUSEEVENT); //监控鼠标离开   
						tme.dwFlags = TME_LEAVE;
						tme.hwndTrack = this->m_hWnd;
						TrackMouseEvent(&tme);
						m_track_mouse_leave = true;
					}

					int32_t xDelta = LOWORD(lParam) - m_windowMouseCoordX;
					int32_t yDelta = HIWORD(lParam) - m_windowMouseCoordY;
					m_windowMouseCoordX = LOWORD(lParam);
					m_windowMouseCoordY = HIWORD(lParam);

					this->setSystemCursorHide(true);
					onMouseMove(m_windowMouseCoordX, m_windowMouseCoordY);

					break;
				}
				case WM_KEYDOWN: //有键按下
				{
					onKeyDown(wParam);
					break;
				}
				case WM_KEYUP: //有键放开
				{
					onKeyUp(wParam);
					break;
				}
				// 左键消息
				case WM_LBUTTONDOWN: {
					onLButtonDown();
					break;
				}
				case WM_LBUTTONUP: {
					onLButtonUp();
					break;
				}
				case WM_LBUTTONDBLCLK:
					onLButtonDoubleClicked();
					break;
					// 右键消息
				case WM_RBUTTONDOWN: {
					onRButtonDown();
					break;
				}case WM_RBUTTONUP: {
					onRButtonUp();
					break;
				}case WM_RBUTTONDBLCLK:
					onRButtonDoubleClicked();
					break;

				case WM_MOUSEWHEEL: {
					onMouseWheel((short)HIWORD(wParam));
					break;
				}
				case WM_CHAR: {
					wchar_t c = (wchar_t)wParam;
					onCharInput(c);
					break;
				}
				case WM_MOUSELEAVE:
				{
					setSystemCursorHide(false);
					POINT pt;
					GetCursorPos(&pt);
					ScreenToClient(m_hWnd, &pt);    // 将鼠标指针位置转换为窗口坐标
					pt.x = math::clamp((int32_t)pt.x, getRect().left(), getRect().right());
					pt.y = math::clamp((int32_t)pt.y, getRect().top(), getRect().bottom());
					m_windowMouseCoordX = pt.x;
					m_windowMouseCoordY = pt.y;
					onMouseMove(m_windowMouseCoordX, m_windowMouseCoordY);
					onMouseLeave();
					m_track_mouse_leave = false;

					//if (!m_track_mouse_hover)
					//{
					//	TRACKMOUSEEVENT tme;
					//	tme.cbSize = sizeof(TRACKMOUSEEVENT); //监控鼠标离开   
					//	tme.dwFlags = TME_HOVER;
					//	tme.hwndTrack = this->m_hWnd;
					//	TrackMouseEvent(&tme);
					//	m_track_mouse_hover = true;
					//}
					break;
				}
				//case WM_MOUSEHOVER:
				//{
				//	setSystemCursorHide(true);
				//	POINT pt;
				//	GetCursorPos(&pt);
				//	ScreenToClient(m_hWnd, &pt);    // 将鼠标指针位置转换为窗口坐标
				//	pt.x = math::clamp((int32_t)pt.x, getRect().left(), getRect().right());
				//	pt.y = math::clamp((int32_t)pt.y, getRect().top(), getRect().bottom());
				//	int32_t xDelta = pt.x - m_windowMouseCoordX;
				//	int32_t yDelta = pt.y - m_windowMouseCoordY;
				//	m_windowMouseCoordX = pt.x;
				//	m_windowMouseCoordY = pt.y;
				//	onMouseMove(m_windowMouseCoordX, m_windowMouseCoordY, xDelta, yDelta);
				//	m_track_mouse_hover = false;
				//}
				case WM_SYSKEYDOWN:
				{
					//std::cout << "WM_SYSKEYDOWN" << std::endl;
					auto nVirtKey = (int)wParam; // virtual-key code 
					auto lKeyData = lParam;       // key data 
					break;
				}


				case WM_IME_STARTCOMPOSITION:// 开始编码
				{
					break;
				}
				case WM_IME_COMPOSITION:
				{
					LONG lRet;
					HIMC hIMC;
					if (hIMC = ImmGetContext(m_hWnd))
					{
						wchar_t szCompStr[256];
						DWORD dwCursorPos;
						//获取显示字符串 
						if (lParam & GCS_COMPSTR)
						{
							lRet = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, szCompStr, ARRAYSIZE(szCompStr)) / sizeof(TCHAR);
							szCompStr[lRet] = 0;
						}
						//获取显示字符串的属性标记 
						if (lParam & GCS_COMPSTR)
						{
							lRet = ImmGetCompositionStringW(hIMC, GCS_COMPATTR, szCompStr, ARRAYSIZE(szCompStr)) / sizeof(TCHAR);
							szCompStr[lRet] = 0;
						}
						//获取显示字符串的光标位置 
						dwCursorPos = ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, NULL, 0);
						//获取上屏字符串 
						if (lParam & GCS_RESULTSTR)
						{
							lRet = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, szCompStr, ARRAYSIZE(szCompStr)) / sizeof(TCHAR);
							szCompStr[lRet] = 0;
						}
						ImmReleaseContext(m_hWnd, hIMC);
						m_windowSystemInputCompositionString = szCompStr;
					}
					break;
				}
				case WM_IME_NOTIFY:
				{
					switch (wParam)
					{
					case IMN_OPENCANDIDATE:          // 打开候选列表
					case IMN_CHANGECANDIDATE:        // 更新候选列表
					{
						HIMC hIMC;
						if (hIMC = ImmGetContext(m_hWnd))
						{
							LPCANDIDATELIST lpCandList = NULL;
							DWORD dwIndex = 0;
							DWORD dwBufLen = ImmGetCandidateList(hIMC, dwIndex, NULL, 0);
							if (dwBufLen)
							{
								lpCandList = (LPCANDIDATELIST)GlobalAlloc(GPTR, dwBufLen);
								dwBufLen = ImmGetCandidateList(hIMC, dwIndex, lpCandList, dwBufLen);
							}
							if (lpCandList)
							{
								DWORD dwSelection = lpCandList->dwSelection;    //处于选中状态的候选序号
								DWORD dwCount = lpCandList->dwCount;        //当前页候选数
								DWORD dwPageStart = lpCandList->dwPageStart;    //当前页起始序号
								DWORD dwPageSize = lpCandList->dwPageSize;     //当前页容量
								m_windowSystemInputCandidateStringList.resize(dwCount);
								for (int i = 0; i < dwCount; i++)
									m_windowSystemInputCandidateStringList[i] = (wchar_t*)((char*)lpCandList + lpCandList->dwOffset[i]);  //候选字符串
								GlobalFree(lpCandList);
							}
							ImmReleaseContext(m_hWnd, hIMC);
						}
					}
					}
					break;
				}
				case WM_IME_ENDCOMPOSITION:// 输入结束
				{
					m_windowSystemInputCandidateStringList.resize(0);
					m_windowSystemInputCompositionString.resize(0);
					break;
				}

				case WM_DROPFILES:
				{
					{ // update cursor hover widget
						POINT pt;
						GetCursorPos(&pt);
						ScreenToClient(m_hWnd, &pt);    // 将鼠标指针位置转换为窗口坐标
						pt.x = math::clamp((int32_t)pt.x, getRect().left(), getRect().right());
						pt.y = math::clamp((int32_t)pt.y, getRect().top(), getRect().bottom());
						m_windowMouseCoordX = pt.x;
						m_windowMouseCoordY = pt.y;
						onMouseMove(m_windowMouseCoordX, m_windowMouseCoordY);
					}

					HDROP hdrop = (HDROP)wParam;
					wchar_t sDropFilePath[MAX_PATH + 1];
					int iDropFileNums = 0;
					iDropFileNums = DragQueryFileW(hdrop, 0xFFFFFFFF, NULL, 0);//获取拖放文件个数
					StringList filenames;
					for (int i = 0; i < iDropFileNums; i++)//分别获取拖放文件名(针对多个文件操作)
					{
						DragQueryFileW(hdrop, i, sDropFilePath, sizeof(sDropFilePath) / sizeof(wchar_t));
						filenames.push_back(sDropFilePath);
					}
					onDropFiles(filenames);
					//DragQueryFile(hdrop, 0, sDropFilePath, MAX_PATH);//分别获取拖放文件名(针对单个文件操作)    
					DragFinish(hdrop);//释放文件名缓冲区    
					DragAcceptFiles(m_hWnd, TRUE);
					break;
				}

				case WM_KILLFOCUS:
					onWindowFocus(false);
					break;
				case WM_SETFOCUS: {
					::GetCursorPos(&pt);
					m_windowMouseCoordX = pt.x;
					m_windowMouseCoordY = pt.y;
					onWindowFocus(true);
					break;
				}
				case WM_SIZE:
					if (m_windowIsFullscreen || !m_windowIsActive)
						return 0;
					GetClientRect(m_hWnd, &rect);
					m_windowWidth = rect.right - rect.left;
					m_windowHeight = rect.bottom - rect.top;
					onWindowResize(m_windowWidth, m_windowHeight);
					break;

					//case WM_MOVE:
					//	GetWindowRect(m_hWnd, &rect);
					//	onWindowMove(rect.left, rect.top);
					//	break;
				//case WM_ACTIVATEAPP:
				//{
				//	if (wParam == TRUE) //检查最小化状态
				//		m_windowIsActive = TRUE; //程序处于激活状态
				//	else
				//		m_windowIsActive = FALSE;//程序不再激活
				//	//std::cout << "WM_ACTIVATEAPP:" << wParam << "\n";
				//	return 0;
				//}
				case WM_ACTIVATE:  //监视窗口激活消息
				{
					if (!HIWORD(wParam)) //检查最小化状态
						m_windowIsActive = TRUE; //程序处于激活状态
					else
						m_windowIsActive = FALSE;//程序不再激活
					//std::cout << "activate:" << m_windowIsActive << "\n";
					break;//返回消息循环
				}
				//case WM_TIMER:
				//	onTimer();
				//	break;
				//case WM_PAINT: {
				//	hdc = BeginPaint(m_hWnd, &ps);
				//	onWindowSystemPaint(hdc);
				//	EndPaint(m_hWnd, &ps);
				//	break;
				//}

				case WM_CLOSE: //收到Close消息	
				{
					m_windowIsDestroy = true;
					onWindowDestroy();
					PostQuitMessage(0);//发出退出消息
					return 0;//
				}
				//case WM_CREATE:
				//{
				//	onCreate();
				//	break;
				//}
				case WM_ERASEBKGND:
				{
					return 0;
				}
				case WM_SYSCOMMAND: //系统中断命令
				{
					switch (wParam) //检查系统调用
					{
					case  SC_SCREENSAVE: //屏保运行

					case SC_MONITORPOWER: //显示器进入节电模式
						return 0; //阻止发生
					}
					break; //退出
				}
				}
				return DefWindowProc(m_hWnd, message, wParam, lParam);;
			}


		};

	}
}
