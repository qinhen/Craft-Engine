#pragma once

#include <array>
#include "../Common.h"
#include "gl_gui_canvas.h"
#include "../../advance/opengl/OpenGLSystem.h"
namespace CraftEngine
{
	namespace gui
	{

		namespace _OpenGL_Render_System_Detail
		{

			constexpr const uint32_t MAX_IMAGE_COUNT = 4096;

			class OpenGLGuiRenderSystem :public AbstractGuiRenderSystem
			{
			private:

				typedef opengl::ImageTexture Image;
				core::ObjectPool<Image, MAX_IMAGE_COUNT> m_imagePool;

				HDC m_curHDC;
				HGLRC m_curHRC;
				HGLRC m_curHRC2;
				std::thread::id m_curID;
				std::mutex m_resourcePoolMutex;

				void bindRes()
				{
					if (std::this_thread::get_id() != m_curID)
					{
						m_resourcePoolMutex.lock();
						wglMakeCurrent(m_curHDC, m_curHRC2);
					}
				}

				void unbindRes()
				{
					if (std::this_thread::get_id() != m_curID)
					{
						wglMakeCurrent(NULL, NULL);
						m_resourcePoolMutex.unlock();
					}
				}

			public:
				OpenGLGuiRenderSystem()
				{
					m_curHDC = wglGetCurrentDC();
					m_curHRC = wglGetCurrentContext();
					m_curID = std::this_thread::get_id();
					m_curHRC2 = wglCreateContext(m_curHDC);
					wglShareLists(m_curHRC, m_curHRC2);
				}

				virtual ~OpenGLGuiRenderSystem()
				{
					std::lock_guard<std::mutex> lock(m_resourcePoolMutex);
					wglDeleteContext(m_curHRC2);
				}

				virtual HandleImage createImage(const void* data, uint32_t size, uint32_t width, uint32_t height) override
				{
					bindRes();
					Image image{};
					if (size == width * height * 4)
						image = opengl::createTexture2D(width, height, GL_RGBA, data, size);
					else if (size == width * height)
						image = opengl::createTexture2D(width, height, GL_RED, data, size, false, GL_CLAMP_TO_EDGE);
					else
						throw std::runtime_error("unrecognized format!");
					auto himage = m_imagePool.store(image);
					unbindRes();
					return HandleImage(himage);
				}
				virtual void deleteImage(HandleImage himage) override
				{
					bindRes();
					if (himage != nullptr)
					{
						Image* pimage = (Image*)himage.operator void* ();
						opengl::destroyTexture(*pimage);
						m_imagePool.free(pimage);
					}
					else
						throw std::runtime_error("himage == nullptr!");
					unbindRes();
				}
				virtual void updateImage(HandleImage image, const void* data, uint32_t size, uint32_t width, uint32_t height) override
				{
					bindRes();
					if (image != nullptr)
					{
						Image* pimage = (Image*)image.operator void* ();
						if (pimage->mWidth == width && pimage->mHeight == height)
						{
							opengl::updateTexture2D(*pimage, 0, 0, width, height, data, size);
						}
						else
						{
							opengl::destroyTexture(*pimage);
							*pimage = opengl::createTexture2D(width, height, GL_RGBA, data, size);
						}
					}
					else
						throw std::runtime_error("himage == nullptr!");
					unbindRes();
				}

				virtual ImageInfo getImageInfo(HandleImage image) override
				{
					ImageInfo info{};
					Image* pimage = (Image*)image.operator void* ();
					if (image == nullptr)
						return info;
					info.mWidth = pimage->mWidth;
					info.mHeight = pimage->mHeight;
					info.mChannels = pimage->mFormat == (int)GL_RGBA ? 4 : 1;
					info.mMipLevels = pimage->mMipLevel;
					return info;
				}

				virtual AbstractGuiCanvas* createGuiCanvas(const Size& size) override
				{
					bindRes();
					VulkanGuiCanvas* canvas = new VulkanGuiCanvas;
					canvas->init(size.x, size.y);
					unbindRes();
					return canvas;
				}

				virtual void deleteGuiCanvas(AbstractGuiCanvas* canvas) override
				{
					bindRes();
					((VulkanGuiCanvas*)canvas)->clear();
					delete canvas;
					unbindRes();
				}



			};


		}

	};

}
