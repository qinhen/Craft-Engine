#pragma once
#ifndef CRAFT_ENGINE_GUI_MAIN_WINDOW_H_
#define CRAFT_ENGINE_GUI_MAIN_WINDOW_H_
#include "./platforms/win32_window.h"


#define CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL 0
#define CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN 1
#define CRAFT_ENGINE_GUI_RENDER_API_USING_SOFT3D 2

// RENDER API
#define CRAFT_ENGINE_RENDER_SYSTEM CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL
#define CRAFT_ENGINE_MSAA_SAMPLE_COUNT 4


#if (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
#include "./platforms/vk_gui_renderer.h"
#include "./platforms/vk_gui_render_system.h"
#include "../advance/vulkan/VulkanSwapChain.h"
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL)
#include "./platforms/gl_gui_renderer.h"
#include "./platforms/gl_gui_render_system.h"
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_SOFT3D)
#include "./platforms/s3d_gui_renderer.h"
#include "./platforms/s3d_gui_render_system.h"
#include "../soft3d/SwapChain.h"
#endif // CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN



namespace CraftEngine
{

	namespace gui
	{

		class MainWindow;

		class Application
		{
		private:
			friend class MainWindow;
			static bool m_isAppInitialized;
			static std::list<RootWidget*> m_windowList;
			static core::ThreadPool m_threadPool;
#if (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL)
			static HGLRC m_sharedHGLRC;
		public:
			static void sharedGlobalGLRC(HGLRC glrc)
			{
				if (m_sharedHGLRC != nullptr)
					wglShareLists(m_sharedHGLRC, glrc);
				else
					m_sharedHGLRC = glrc;
			}
			static void unsharedGlobalGLRC(HGLRC glrc)
			{
				if (m_sharedHGLRC != glrc)
				{
					
				}
				else
				{	
					// TODO
				}
			}
			static LRESULT CALLBACK TempWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_SOFT3D)
#endif // (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
		public:

			static core::ThreadPool& getThreadPool() { return m_threadPool; }

			Application()
			{
				if (!m_isAppInitialized)
				{
					m_threadPool.init(math::max(1, (int)std::thread::hardware_concurrency() - 2));

#if (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
					if (!vulkan::VulkanSystem::isInitialized())
						vulkan::VulkanSystem::initSystem();
					if (!gui::GuiRenderSystem::isInitialized())
						gui::GuiRenderSystem::initSystem(new _Vulkan_Render_System_Detail::VulkanGuiRenderSystem);
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL)
			
					WNDCLASSA wc;
					wc.style = 0; //字段style表示从WNDCLASS派生的窗口具有的样式或称风格，这里初始化为0。
					wc.lpfnWndProc = (WNDPROC)TempWndProc; //字段lpfnWndProc为窗口过程地址，这里初始化为WndProc。
					//下面的两个字段被当成预留的字节数，程序可以根据需要来使用预留空间（由于这里没有使用它们，所以初始化为0）。
					wc.cbClsExtra = 0;
					wc.cbWndExtra = 0;
					wc.hInstance = GetModuleHandle(NULL); //字段hInstance为应用程序的实例句柄，这里初始化为WinMain()的第一个参数hInstance。
					//下面的3个字段分别为窗口图标句柄、光标句柄与背景画刷句柄（这里都初始化为系统预定义的值）。
					wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO); //通过LoadIcon()返回。
					wc.hCursor = LoadCursor(nullptr, IDC_ARROW); //通过LoadCursor()返回。
					wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
					wc.lpszMenuName = nullptr; //字段lpszMenuName为菜单名称指针（nullptr表示窗口没有菜单）
					wc.lpszClassName = "FoolWinClass"; //字段lpszClassName表示本WNDCLASS自己的名称指针，这里被初始化为"MyWindClass"
					RegisterClassA(&wc); //在这里注册WNDCLASS					
					//2、用CreateWindow()创建窗口，如函数执行成功返回创建的窗口句柄，否则返回nullptr。
					HWND hwnd = CreateWindowA(
						"FoolWinClass", //第一个参数为所使用的WNDCLASS名称，窗口将由此WNDCLASS模板生成。
						nullptr, //第二个参数为窗口标题栏中显示的文本
						WS_OVERLAPPEDWINDOW, //相当于WS_OVERLAPPED|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX的组合。
						//以下的4个参数用系统的默认值指定了窗口的初始位置和大小（左上角坐标x, y及其宽度nWidth与高度nHeight）。
						CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
						HWND_DESKTOP, //这个参数为父窗口句柄（HWND_DESKTOP表示这个窗口没有父窗口，即为顶层窗口）。
						nullptr, //这个参数为与窗口联系的菜单句柄（nullptr表示这个窗口没有菜单）。
						GetModuleHandle(NULL), //这个参数为应用程序实例句柄（用来让程序员区分是程序自身还是DLL模块的值）。
						nullptr //这个参数为指向一个值的无类型指针，该值会传递给窗口的WM_CREATE消息处理程序。
					);

					PIXELFORMATDESCRIPTOR pfd;			
					HDC tempHdc = GetDC(hwnd); // Attempt to set the pixel format 
					if (!SetPixelFormat(tempHdc, 1, &pfd))//每个窗口只能设置一次 
						std::runtime_error("");
					HGLRC temphRC = wglCreateContext(tempHdc); //创建一个临时的环境为了初始化glew,初始化后才能够使用wglChoosePixelFormatARB，wglCreateContextAttribsARB函数 
					wglMakeCurrent(tempHdc, temphRC); //只有设置当前opengl环境后才能够初始化glew库 

					if (!opengl::OpenGLSystem::isInitialized())
						opengl::OpenGLSystem::initSystem();

					wglMakeCurrent(NULL, NULL);
					wglDeleteContext(temphRC);

					DestroyWindow(hwnd);
					UnregisterClassA("FoolWinClass", GetModuleHandle(NULL));
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_SOFT3D)

					if (!gui::GuiRenderSystem::isInitialized())
						gui::GuiRenderSystem::initSystem(new _Soft3D_Render_System_Detail::Soft3DGuiRenderSystem);
#endif // (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
					m_isAppInitialized = true;
				}
				else
				{
					throw std::runtime_error("CraftEngine GUI Application is already initialized.");
				}
			}

			~Application()
			{
				if (m_isAppInitialized)
				{

					std::vector<RootWidget*> window_list(m_windowList.size());
					int i = 0;
					for (auto it : m_windowList)
						window_list[i++] = it;

					for (auto& it : window_list)
						it->getTaskManager()->clearAllTask();

					m_threadPool.wait();

					for (auto& it : window_list)
						delete it;

					window_list.clear();

					m_windowList.clear();

					if (gui::GuiRenderSystem::isInitialized())
						gui::GuiRenderSystem::destroySystem();
#if (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
					if (vulkan::VulkanSystem::isInitialized())
						vulkan::VulkanSystem::destroySystem();
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL)
					m_sharedHGLRC = NULL;
					if (opengl::OpenGLSystem::isInitialized())
						opengl::OpenGLSystem::destroySystem();
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_SOFT3D)


#endif // (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)

					m_isAppInitialized = false;
					std::cout << "count = " << gui::widget_instances_count << std::endl;
				}
				else
				{
					throw std::runtime_error("CraftEngine GUI Application is not yet initialized.");
				}
			}
		};
		bool Application::m_isAppInitialized = false;
		std::list<RootWidget*> Application::m_windowList;
		core::ThreadPool Application::m_threadPool;
#if (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL)
		HGLRC Application::m_sharedHGLRC = NULL;
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_SOFT3D)

#endif // (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)



		class MainWindow : public Win32Window
		{
		private:

#if (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
			vulkan::Instance*       m_vkInstance;
			vulkan::PhysicalDevice* m_vkPhysicalDevice;
			vulkan::LogicalDevice*  m_vkLogicalDevece;
			vulkan::SwapChain       m_vkSwapChain;
			VkSemaphore imageAvailableSemaphore;
			VkSemaphore guiRenderFinishedSemaphore;
			VkFence m_renderFence;
			_Vulkan_Render_System_Detail::VulkanGuiRenderer* m_guiRenderer;
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL)

			_OpenGL_Render_System_Detail::OpenGLGuiRenderer* m_guiRenderer;
			HDC m_hDC;
			HGLRC m_hGLRC;
			void initOpenGLContext()
			{
				if (!(m_hDC = GetDC(getHWnd()))) //是否取得设备描述表
					throw std::runtime_error("");
				
				if (wglChoosePixelFormatARB && wglCreateContextAttribsARB && !(CRAFT_ENGINE_MSAA_SAMPLE_COUNT == 1))
				{
					float fPixAttribs[] = { 0, 0 };
					int iPixAttribs[] = { WGL_SUPPORT_OPENGL_ARB, GL_TRUE, // Must support OGL rendering
						WGL_DRAW_TO_WINDOW_ARB, GL_TRUE, // pf that can run a window 
						WGL_ACCELERATION_ARB,
						WGL_FULL_ACCELERATION_ARB, // must be HW accelerated
						WGL_COLOR_BITS_ARB, 32, // 8 bits of each R, G and B
						WGL_DEPTH_BITS_ARB, 24, // 24 bits of depth precision for window
						WGL_DOUBLE_BUFFER_ARB, GL_TRUE, // Double buffered context
						WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, // pf should be RGBA type
						WGL_STENCIL_BITS_ARB, 8,//开启模板缓冲区,模板缓冲区位数=8
						WGL_SAMPLE_BUFFERS_ARB, GL_TRUE, // MSAA on,开启多重采样
						WGL_SAMPLES_ARB, CRAFT_ENGINE_MSAA_SAMPLE_COUNT, // 4x MSAA ,多重采样样本数量为4
						0 }; // NULL termination 
					HDC dc_handle = m_hDC;
					int nPixelFormat = -1;
					int nPixCount = 0;
					wglChoosePixelFormatARB(dc_handle, iPixAttribs, fPixAttribs, 1, &nPixelFormat, (UINT*)&nPixCount);//新的查询像素格式的函数 
					if (nPixelFormat == -1)    //多重采样时,如果硬件不支持就使用下面的代码关闭多重采样 
					{ // Try again without MSAA 
						iPixAttribs[19] = 1;
						wglChoosePixelFormatARB(dc_handle, iPixAttribs, fPixAttribs, 1, &nPixelFormat, (UINT*)&nPixCount);
					} // Got a format, now set it as the current one
					PIXELFORMATDESCRIPTOR pfd;
					if (!SetPixelFormat(dc_handle, nPixelFormat, &pfd))
						throw std::runtime_error("");

					GLint attribs[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 4,//主版本4
						WGL_CONTEXT_MINOR_VERSION_ARB, 5,                //次版本号5
						WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
						0 };
					m_hGLRC = wglCreateContextAttribsARB(dc_handle, 0, attribs);

					if (m_hGLRC == NULL)
					{
						attribs[1] = 1;
						attribs[3] = 3;
						m_hGLRC = wglCreateContextAttribsARB(dc_handle, 0, attribs);
						if (m_hGLRC == NULL)
							throw std::runtime_error("Could not create an OpenGL 1.3 context.");
					}
				}			
				else
				{
					int msPixelformat = 14;//全屏多重采样需要的pixelformat
					unsigned normalPixelformat;//保存查找匹配的结果
					PIXELFORMATDESCRIPTOR normalpfd = //pdf告诉窗口希望的东西，即窗口使用的像素格式
					{
					sizeof(PIXELFORMATDESCRIPTOR), //上述格式描述符的大小
					1,       //版本号
					PFD_DRAW_TO_WINDOW |  //格式支持窗口
					PFD_SUPPORT_OPENGL |  //格式支持OpenGL
					PFD_DOUBLEBUFFER,  //支持双缓冲
					PFD_TYPE_RGBA,     //申请RGBA格式
					32,                //选定色彩深度
					0,0,0,0,0,0,       //忽略的色彩位
					0,                 //无Alpha缓存
					0,                 //忽略Shift Bit
					0,                 //无累加缓存
					0,0,0,0,           //忽略聚集位
					24,                //16位Z-缓存(深度缓存)
					0,                 //无蒙板缓存
					0,                 //无辅助缓存
					PFD_MAIN_PLANE,    //主绘图层
					0,                 //Reserved
					0,0,0              //忽略层遮罩
					};
					if (!SetPixelFormat(m_hDC, msPixelformat, &normalpfd))
					{
						if (!(normalPixelformat = ChoosePixelFormat(m_hDC, &normalpfd))) //Windows是否找到相应的像素格式
							throw std::runtime_error("");
						if (!SetPixelFormat(m_hDC, normalPixelformat, &normalpfd)) //是否能够设置像素格式
							throw std::runtime_error("");
					}
					if (!(m_hGLRC = wglCreateContext(m_hDC))) //能否取得着色描述表
						throw std::runtime_error("");
				}

				if (!wglMakeCurrent(m_hDC, m_hGLRC)) //尝试激活着色描述表
					throw std::runtime_error("");
				Application::sharedGlobalGLRC(m_hGLRC);
			}
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_SOFT3D)
			_Soft3D_Render_System_Detail::Soft3DGuiRenderer* m_guiRenderer;
			soft3d::SwapChain m_swapChain;
#endif 
		private:
			bool m_isInitialized = false;
			bool m_isVsnyc = true;
			bool m_updateEveryFrame = false;
		public:

			MainWindow(
				const MainWindow* parent
			) : MainWindow(L"Powered by Craft Engine", 1280, 720, parent) {}

			MainWindow(
				const std::wstring& title = L"Powered by Craft Engine",
				uint32_t    width = 1280,
				uint32_t    height = 720,
				const MainWindow* parent = nullptr
			) : Win32Window(title, width, height, parent)
			{
				if (!Application::m_isAppInitialized)
					throw std::runtime_error("CraftEngine GUI Application is not yet initialized.");

				uint32_t w = width, h = height;
#if (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
				m_vkInstance = vulkan::VulkanSystem::getInstance();
				m_vkPhysicalDevice = vulkan::VulkanSystem::getPhysicalDevice();
				m_vkLogicalDevece = vulkan::VulkanSystem::getLogicalDevice();
				m_vkSwapChain.connect(m_vkInstance->getInstance(), m_vkPhysicalDevice->getPhysicalDevice(), m_vkLogicalDevece->getLogicalDevice());
				m_vkSwapChain.initSurface(GetModuleHandle(0), getHWnd());
				m_vkSwapChain.create(&w, &h, m_isVsnyc);
				m_guiRenderer = new _Vulkan_Render_System_Detail::VulkanGuiRenderer();
				m_guiRenderer->init(w, h, m_vkSwapChain.images(), m_vkSwapChain.imageViews(), m_vkSwapChain.format(), m_vkLogicalDevece, CRAFT_ENGINE_MSAA_SAMPLE_COUNT); // MSAA
				imageAvailableSemaphore = m_vkLogicalDevece->createSemaphore();
				guiRenderFinishedSemaphore = m_vkLogicalDevece->createSemaphore();
				m_renderFence = m_vkLogicalDevece->createFence();
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL)

				initOpenGLContext();
				if (!gui::GuiRenderSystem::isInitialized())
					gui::GuiRenderSystem::initSystem(new _OpenGL_Render_System_Detail::OpenGLGuiRenderSystem);

				m_guiRenderer = new _OpenGL_Render_System_Detail::OpenGLGuiRenderer;
				m_guiRenderer->init(w, h, 0, CRAFT_ENGINE_MSAA_SAMPLE_COUNT); // MSAA
				
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_SOFT3D)

				m_swapChain = soft3d::createSwapChain();
				m_swapChain.create(getHWnd(), w, h, 1);
				m_guiRenderer = new _Soft3D_Render_System_Detail::Soft3DGuiRenderer();
				m_guiRenderer->init(w, h, m_swapChain.getImages(), CRAFT_ENGINE_MSAA_SAMPLE_COUNT); // MSAA

#endif // (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)


				this->setGuiRenderer(m_guiRenderer);
				this->setRect(Rect(0, 0, w, h));
				m_isInitialized = true;
				this->setSystemCursorHide(true);
				// init root widget after the setup of render system
				_Root_Widget_Init_Future();
				Application::m_windowList.push_back(this);
			}


			virtual ~MainWindow()
			{
				for (auto it = Application::m_windowList.begin(); it != Application::m_windowList.end(); it++)
				{
					if ((*it) == this)
					{
						Application::m_windowList.erase(it);
						break;
					}
				}
				m_guiRenderer->clear();
				delete[] m_guiRenderer;
#if (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
				m_vkSwapChain.cleanup();
				m_vkLogicalDevece->destroyFence(m_renderFence);
				m_vkLogicalDevece->destroySemaphore(imageAvailableSemaphore);
				m_vkLogicalDevece->destroySemaphore(guiRenderFinishedSemaphore);
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL)
				if (m_hGLRC)//是否拥有OpenGL渲染描述表
				{
					opengl::OpenGLSystem::lockContext();
					if (wglGetCurrentContext() != NULL)
					{
						if (!wglMakeCurrent(NULL, NULL)) 
							throw std::runtime_error("");
					}
					if (!wglDeleteContext(m_hGLRC)) //能否删除RC
						throw std::runtime_error("");
					opengl::OpenGLSystem::unlockContext();
					m_hGLRC = NULL;
				}
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_SOFT3D)

				soft3d::destroySwapChain(m_swapChain);

#endif 


			}

			void setUpdateEveryFrame(bool enable)
			{
				m_updateEveryFrame = enable;
			}

			void setVsnyc(bool enable)
			{
				if (m_isVsnyc == enable)
					return;
				m_isVsnyc = enable;
				if (m_isInitialized)
				{
#if (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
					m_vkLogicalDevece->waitDevice();
					uint32_t w = getWidth(), h = getHeight();
					if (w == 0 || h == 0)
						return;
					m_vkSwapChain.create(&w, &h, m_isVsnyc);
					if (w == 0 || h == 0)
						return;
					m_guiRenderer->resetRenderTarget(w, h, m_vkSwapChain.images(), m_vkSwapChain.imageViews());
					//std::cout << "resize callback called..." << std::endl;
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL)
					wglSwapIntervalEXT(m_isVsnyc ? 1 : 0);
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_SOFT3D)

#endif 
				}
			}

		protected:

			virtual void onWindowResize(uint32_t width, uint32_t height) override final
			{
				if (m_isInitialized)
				{
					std::cout << "resize callback called..." << std::endl;
					uint32_t w = width, h = height;
					if (w == 0 || h == 0)
						return;
#if (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
					//m_vkLogicalDevece->waitDevice();
					m_vkSwapChain.create(&w, &h, m_isVsnyc);
					if (w == 0 || h == 0)
						return;
					this->_Resize_No_Event({ (int)w,(int)h });
					m_guiRenderer->resetRenderTarget(w, h, m_vkSwapChain.images(), m_vkSwapChain.imageViews());
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL)
					this->_Resize_No_Event({ (int)w,(int)h });
					m_guiRenderer->resetRenderTarget(w, h, 0);
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_SOFT3D)
					//m_guiRenderer->stop();
					m_guiRenderer->wait();
					m_swapChain.create(getHWnd(), w, h, 1);
					this->_Resize_No_Event({ (int)w,(int)h });
					m_guiRenderer->resetRenderTarget(w, h, m_swapChain.getImages());
#endif 
					RootWidget::onResizeEvent({ Size(w, h) });//

				}
			}


			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override final
			{
				setWindowSize(resizeEvent.size);
				RootWidget::onResizeEvent(resizeEvent);//
			}

			virtual bool onDrawFrame(float duration) override final
			{

				if (!m_updateEveryFrame && m_shouldRepaint == 0)
				{
					Sleep(1);
					getSyncTimerSystem()->step();
					Application::getThreadPool().push([=]() { getTaskManager()->solvingAsync(); });
					getTaskManager()->solvingSync();
					return true;
				}
				//else if (m_shouldRepaint == 1)
				//	return true;
				m_shouldRepaint--;

#if (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
				uint32_t currentSwapChainImageIndex;
				auto result = m_vkSwapChain.acquireNextImage(imageAvailableSemaphore, &currentSwapChainImageIndex);
				if (result == VK_ERROR_OUT_OF_DATE_KHR) {
					//throw std::runtime_error("failed to acquire swap chain image!");
					uint32_t w = getWidth(), h = getHeight();
					m_vkSwapChain.create(&w, &h, m_isVsnyc);
					if (w == 0 || h == 0)
						return false;
					this->_Resize_No_Event({ (int)w,(int)h });
					m_guiRenderer->resetRenderTarget(w, h, m_vkSwapChain.images(), m_vkSwapChain.imageViews());
					return false;
				}
				else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
					throw std::runtime_error("failed to acquire swap chain image!");

				std::vector<VkCommandBuffer> commandBuffers;
				std::vector<VkPipelineStageFlags> waitStages;
				std::vector<VkSemaphore> waitSemaphore;
				std::vector<VkSemaphore> signalSemaphore;

				m_guiRenderer->setFrameBufferIndex(currentSwapChainImageIndex);


				onRenderStarted();
				m_guiRenderer->begin(0);
				RootWidget::onPaintEvent();
				m_guiRenderer->end(0);
				onRenderFinished();


				waitSemaphore = { imageAvailableSemaphore };
				waitStages = { VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };
				signalSemaphore = { guiRenderFinishedSemaphore };
				commandBuffers = { m_guiRenderer->getCmdBuf() };
				m_vkLogicalDevece->submitCommands(
					m_vkLogicalDevece->getGraphicsQueueIndex(),
					commandBuffers.size(), commandBuffers.data(),
					waitSemaphore.size(), waitSemaphore.data(), waitStages.data(),
					signalSemaphore.size(), signalSemaphore.data(),
					m_renderFence
				);
				waitSemaphore = { guiRenderFinishedSemaphore };
				int queue_idx = m_vkLogicalDevece->getPresentQueueIndex();
				m_vkLogicalDevece->lockQueue(queue_idx);
				result = m_vkSwapChain.queuePresent(m_vkLogicalDevece->getQueue(queue_idx), currentSwapChainImageIndex, waitSemaphore.size(), waitSemaphore.data());
				m_vkLogicalDevece->unlockQueue(queue_idx);
				if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
					onWindowResize(getWidth(), getHeight());
				else if (result != VK_SUCCESS)
					throw std::runtime_error("failed to present swap chain image!");

#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL)
				if (wglGetCurrentContext() != m_hGLRC)
					wglMakeCurrent(m_hDC, m_hGLRC);


				onRenderStarted();
				m_guiRenderer->begin(0);
				RootWidget::onPaintEvent();
				m_guiRenderer->end(0);
				onRenderFinished();
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_SOFT3D)

				m_swapChain.acquireNextImage();
				m_guiRenderer->setFrameBufferIndex(m_swapChain.currentImageIndex());

				onRenderStarted();
				m_guiRenderer->begin(0);
				RootWidget::onPaintEvent();
				m_guiRenderer->end(0);
				onRenderFinished();


#endif 

				m_guiRenderer->setFrameDuration(duration);
				static float time_accu = 0.0f;
				static int frame_count = 0;
				time_accu += duration;
				frame_count++;
				if (time_accu > 1000.0f)
				{
					float frame_per_sec = 1000.0f / (time_accu / frame_count);
					m_guiRenderer->setFrameRate(frame_per_sec);
					std::cout << "fps =" << frame_per_sec << std::endl;
					time_accu = 0.0f;
					frame_count = 0;
				}


				// 更新计时器
				//std::this_thread::sleep_for(std::chrono::nanoseconds(5000000));
				getSyncTimerSystem()->step();
				Application::getThreadPool().push([=](){ getTaskManager()->solvingAsync(); });
				getTaskManager()->solvingSync();



#if (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
				m_vkLogicalDevece->waitFence(m_renderFence);
				m_vkLogicalDevece->resetFence(m_renderFence);
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL)
				SwapBuffers(m_hDC);//交换缓存(双缓存)
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_SOFT3D)

				m_guiRenderer->wait();
				soft3d::imgTrans(m_guiRenderer->m_swapchainImages[m_swapChain.currentImageIndex()]);
				//m_guiRenderer->stop();
				m_swapChain.presentImage(m_swapChain.currentImageIndex());
#endif 
				return true;
			}

		};

	}

}


#endif // CRAFT_ENGINE_GUI_MAIN_WINDOW_H_
