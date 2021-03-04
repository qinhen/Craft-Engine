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
					wc.style = 0; //�ֶ�style��ʾ��WNDCLASS�����Ĵ��ھ��е���ʽ��Ʒ�������ʼ��Ϊ0��
					wc.lpfnWndProc = (WNDPROC)TempWndProc; //�ֶ�lpfnWndProcΪ���ڹ��̵�ַ�������ʼ��ΪWndProc��
					//����������ֶα�����Ԥ�����ֽ�����������Ը�����Ҫ��ʹ��Ԥ���ռ䣨��������û��ʹ�����ǣ����Գ�ʼ��Ϊ0����
					wc.cbClsExtra = 0;
					wc.cbWndExtra = 0;
					wc.hInstance = GetModuleHandle(NULL); //�ֶ�hInstanceΪӦ�ó����ʵ������������ʼ��ΪWinMain()�ĵ�һ������hInstance��
					//�����3���ֶηֱ�Ϊ����ͼ������������뱳����ˢ��������ﶼ��ʼ��ΪϵͳԤ�����ֵ����
					wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO); //ͨ��LoadIcon()���ء�
					wc.hCursor = LoadCursor(nullptr, IDC_ARROW); //ͨ��LoadCursor()���ء�
					wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
					wc.lpszMenuName = nullptr; //�ֶ�lpszMenuNameΪ�˵�����ָ�루nullptr��ʾ����û�в˵���
					wc.lpszClassName = "FoolWinClass"; //�ֶ�lpszClassName��ʾ��WNDCLASS�Լ�������ָ�룬���ﱻ��ʼ��Ϊ"MyWindClass"
					RegisterClassA(&wc); //������ע��WNDCLASS					
					//2����CreateWindow()�������ڣ��纯��ִ�гɹ����ش����Ĵ��ھ�������򷵻�nullptr��
					HWND hwnd = CreateWindowA(
						"FoolWinClass", //��һ������Ϊ��ʹ�õ�WNDCLASS���ƣ����ڽ��ɴ�WNDCLASSģ�����ɡ�
						nullptr, //�ڶ�������Ϊ���ڱ���������ʾ���ı�
						WS_OVERLAPPEDWINDOW, //�൱��WS_OVERLAPPED|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX����ϡ�
						//���µ�4��������ϵͳ��Ĭ��ֵָ���˴��ڵĳ�ʼλ�úʹ�С�����Ͻ�����x, y������nWidth��߶�nHeight����
						CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
						HWND_DESKTOP, //�������Ϊ�����ھ����HWND_DESKTOP��ʾ�������û�и����ڣ���Ϊ���㴰�ڣ���
						nullptr, //�������Ϊ�봰����ϵ�Ĳ˵������nullptr��ʾ�������û�в˵�����
						GetModuleHandle(NULL), //�������ΪӦ�ó���ʵ������������ó���Ա�����ǳ���������DLLģ���ֵ����
						nullptr //�������Ϊָ��һ��ֵ��������ָ�룬��ֵ�ᴫ�ݸ����ڵ�WM_CREATE��Ϣ�������
					);

					PIXELFORMATDESCRIPTOR pfd;			
					HDC tempHdc = GetDC(hwnd); // Attempt to set the pixel format 
					if (!SetPixelFormat(tempHdc, 1, &pfd))//ÿ������ֻ������һ�� 
						std::runtime_error("");
					HGLRC temphRC = wglCreateContext(tempHdc); //����һ����ʱ�Ļ���Ϊ�˳�ʼ��glew,��ʼ������ܹ�ʹ��wglChoosePixelFormatARB��wglCreateContextAttribsARB���� 
					wglMakeCurrent(tempHdc, temphRC); //ֻ�����õ�ǰopengl��������ܹ���ʼ��glew�� 

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
				if (!(m_hDC = GetDC(getHWnd()))) //�Ƿ�ȡ���豸������
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
						WGL_STENCIL_BITS_ARB, 8,//����ģ�建����,ģ�建����λ��=8
						WGL_SAMPLE_BUFFERS_ARB, GL_TRUE, // MSAA on,�������ز���
						WGL_SAMPLES_ARB, CRAFT_ENGINE_MSAA_SAMPLE_COUNT, // 4x MSAA ,���ز�����������Ϊ4
						0 }; // NULL termination 
					HDC dc_handle = m_hDC;
					int nPixelFormat = -1;
					int nPixCount = 0;
					wglChoosePixelFormatARB(dc_handle, iPixAttribs, fPixAttribs, 1, &nPixelFormat, (UINT*)&nPixCount);//�µĲ�ѯ���ظ�ʽ�ĺ��� 
					if (nPixelFormat == -1)    //���ز���ʱ,���Ӳ����֧�־�ʹ������Ĵ���رն��ز��� 
					{ // Try again without MSAA 
						iPixAttribs[19] = 1;
						wglChoosePixelFormatARB(dc_handle, iPixAttribs, fPixAttribs, 1, &nPixelFormat, (UINT*)&nPixCount);
					} // Got a format, now set it as the current one
					PIXELFORMATDESCRIPTOR pfd;
					if (!SetPixelFormat(dc_handle, nPixelFormat, &pfd))
						throw std::runtime_error("");

					GLint attribs[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 4,//���汾4
						WGL_CONTEXT_MINOR_VERSION_ARB, 5,                //�ΰ汾��5
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
					int msPixelformat = 14;//ȫ�����ز�����Ҫ��pixelformat
					unsigned normalPixelformat;//�������ƥ��Ľ��
					PIXELFORMATDESCRIPTOR normalpfd = //pdf���ߴ���ϣ���Ķ�����������ʹ�õ����ظ�ʽ
					{
					sizeof(PIXELFORMATDESCRIPTOR), //������ʽ�������Ĵ�С
					1,       //�汾��
					PFD_DRAW_TO_WINDOW |  //��ʽ֧�ִ���
					PFD_SUPPORT_OPENGL |  //��ʽ֧��OpenGL
					PFD_DOUBLEBUFFER,  //֧��˫����
					PFD_TYPE_RGBA,     //����RGBA��ʽ
					32,                //ѡ��ɫ�����
					0,0,0,0,0,0,       //���Ե�ɫ��λ
					0,                 //��Alpha����
					0,                 //����Shift Bit
					0,                 //���ۼӻ���
					0,0,0,0,           //���Ծۼ�λ
					24,                //16λZ-����(��Ȼ���)
					0,                 //���ɰ建��
					0,                 //�޸�������
					PFD_MAIN_PLANE,    //����ͼ��
					0,                 //Reserved
					0,0,0              //���Բ�����
					};
					if (!SetPixelFormat(m_hDC, msPixelformat, &normalpfd))
					{
						if (!(normalPixelformat = ChoosePixelFormat(m_hDC, &normalpfd))) //Windows�Ƿ��ҵ���Ӧ�����ظ�ʽ
							throw std::runtime_error("");
						if (!SetPixelFormat(m_hDC, normalPixelformat, &normalpfd)) //�Ƿ��ܹ��������ظ�ʽ
							throw std::runtime_error("");
					}
					if (!(m_hGLRC = wglCreateContext(m_hDC))) //�ܷ�ȡ����ɫ������
						throw std::runtime_error("");
				}

				if (!wglMakeCurrent(m_hDC, m_hGLRC)) //���Լ�����ɫ������
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
				if (m_hGLRC)//�Ƿ�ӵ��OpenGL��Ⱦ������
				{
					opengl::OpenGLSystem::lockContext();
					if (wglGetCurrentContext() != NULL)
					{
						if (!wglMakeCurrent(NULL, NULL)) 
							throw std::runtime_error("");
					}
					if (!wglDeleteContext(m_hGLRC)) //�ܷ�ɾ��RC
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


				// ���¼�ʱ��
				//std::this_thread::sleep_for(std::chrono::nanoseconds(5000000));
				getSyncTimerSystem()->step();
				Application::getThreadPool().push([=](){ getTaskManager()->solvingAsync(); });
				getTaskManager()->solvingSync();



#if (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_VULKAN)
				m_vkLogicalDevece->waitFence(m_renderFence);
				m_vkLogicalDevece->resetFence(m_renderFence);
#elif (CRAFT_ENGINE_RENDER_SYSTEM == CRAFT_ENGINE_GUI_RENDER_API_USING_OPENGL)
				SwapBuffers(m_hDC);//��������(˫����)
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
