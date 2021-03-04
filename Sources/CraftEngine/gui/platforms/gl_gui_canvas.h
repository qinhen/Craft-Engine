#pragma once

#include "./gl_gui_renderer.h";

namespace CraftEngine
{
	namespace gui
	{

		namespace _OpenGL_Render_System_Detail
		{



			class VulkanGuiCanvas : public AbstractGuiCanvas
			{
			private:
				typedef opengl::ImageTexture Image;
				GLuint m_FBO;
				Image m_colorAttachment;
				OpenGLGuiCanvasRenderer m_renderer;
			public:

				void init(uint32_t width, uint32_t height)
				{
					m_colorAttachment = {};

					m_colorAttachment.mWidth = width;
					m_colorAttachment.mHeight = height;
					m_colorAttachment.mFormat = GL_RGBA;
					//初始化帧缓冲framebuffer
					glGenFramebuffers(1, &m_FBO);///创建缓存
					glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);//以下代码对framebuffer负责,包括纹理附件设置和rbo附件设置
					glGenTextures(1, &this->m_colorAttachment.mImage);///创建缓存
					{
						glBindTexture(GL_TEXTURE_2D, this->m_colorAttachment.mImage);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
							width, height,
							0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);//设置宽高但不赋予纹理文件的数据
						// Set the filtering mode
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						glBindTexture(GL_TEXTURE_2D, 0);
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->m_colorAttachment.mImage, 0);//完成FBO的纹理附件挂载
						GLenum attachment = GL_COLOR_ATTACHMENT0;
						glDrawBuffers(1, &attachment);
					}
					GLuint status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
					if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
						throw std::runtime_error("");
					glBindFramebuffer(GL_FRAMEBUFFER, 0);//完成纹理附件和rbo附件的设置，解绑fbo

					m_renderer.init(width, height, m_FBO, 1); //
				}
				void clear()
				{
					m_renderer.clear();
					glDeleteFramebuffers(1, &m_FBO);
					glDeleteTextures(1, &m_colorAttachment.mImage);
					m_colorAttachment = {};
				}

				virtual Size getCanvasSize() override
				{
					return Size(m_colorAttachment.mWidth, m_colorAttachment.mHeight);
				}
				virtual void setCanvasSize(const Size& size) override
				{
					glDeleteFramebuffers(1, &m_FBO);
					glDeleteTextures(1, &m_colorAttachment.mImage);

					m_colorAttachment = {};

					m_colorAttachment.mWidth = size.x;
					m_colorAttachment.mHeight = size.y;
					m_colorAttachment.mFormat = GL_RGBA;
					{
						glGenFramebuffers(1, &m_FBO);///创建缓存
						glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);//以下代码对framebuffer负责,包括纹理附件设置和rbo附件设置
						glGenTextures(1, &this->m_colorAttachment.mImage);///创建缓存
						{
							glBindTexture(GL_TEXTURE_2D, this->m_colorAttachment.mImage);
							glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
								m_colorAttachment.mWidth, m_colorAttachment.mHeight,
								0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);//设置宽高但不赋予纹理文件的数据
							// Set the filtering mode
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
							glBindTexture(GL_TEXTURE_2D, 0);
							glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->m_colorAttachment.mImage, 0);//完成FBO的纹理附件挂载
							GLenum attachment = GL_COLOR_ATTACHMENT0;
							glDrawBuffers(1, &attachment);
						}
						GLuint status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
						if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
							throw std::runtime_error("");
						glBindFramebuffer(GL_FRAMEBUFFER, 0);//完成纹理附件和rbo附件的设置，解绑fbo
					}
					m_renderer.resetRenderTarget(m_colorAttachment.mWidth, m_colorAttachment.mHeight, m_FBO);
				}
				virtual void setCurrentFramebufferIndex(int32_t idx) override
				{
					
				}
				virtual uint32_t getCurrentFramebufferIndex() const override
				{
					return 0;
				}
				virtual HandleImage getFramebufferImage(uint32_t attachmentIdx = 0, uint32_t framebufferIdx = 0)  override
				{
					return HandleImage(&m_colorAttachment);
				}
				virtual uint32_t getFramebufferCount() const  override
				{
					return 1;
				}
				virtual uint32_t getAttachmentCount() const override
				{
					return 1;
				}
				virtual AbstractGuiRenderer* getGuiRenderer() override
				{
					return &m_renderer;
				}
			};

		}

	};



}
