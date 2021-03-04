#pragma once

#include "../Common.h"

#include "./gl_gui_pipeline.h"

namespace CraftEngine
{
	namespace gui
	{

		namespace _OpenGL_Render_System_Detail
		{

			class OpenGLGuiRenderer : public CraftEngine::gui::AbstractGuiRenderer
			{
			public:
				typedef opengl::ImageTexture Image;
				OpenGLGuiRendererPipeline m_pipeline;

				GLuint m_FBO;

				GLuint m_VAO;
				GLuint m_EBO;
				GLuint m_VBO;

				uint32_t m_renderWidth;
				uint32_t m_renderHeight;
				int m_sampleCount = 1;
			public:

				void init(
					uint32_t initWidth,
					uint32_t initHeight,
					GLuint fbo,
					int sampleCount
				)
				{
					m_FBO = fbo;
					m_renderWidth = initWidth;
					m_renderHeight = initHeight;
					m_sampleCount = sampleCount;

					m_pipeline.init();
					m_pipeline.create();

					glGenVertexArrays(1, &m_VAO);
					glGenBuffers(1, &m_VBO);
					glGenBuffers(1, &m_EBO);
					glBindVertexArray(m_VAO);
					glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, pos));
					glEnableVertexAttribArray(1);
					glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex));
					glEnableVertexAttribArray(2);
					glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(Vertex), (const void*)offsetof(Vertex, col));
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindVertexArray(0);

				}

				virtual void clear() override
				{
					glDeleteBuffers(1, &m_EBO);
					glDeleteBuffers(1, &m_VBO);
					glDeleteVertexArrays(1, &m_VAO);
					m_pipeline.clear();
				}

				void resetRenderTarget(
					uint32_t initWidth,
					uint32_t initHeight,
					GLuint fbo
				) {
					m_FBO = fbo;
					m_renderWidth = initWidth;
					m_renderHeight = initHeight;
				}

				virtual void begin(void* userData)
				{
					m_drawList.mDrawCommand.clear();
					m_drawList.mVerticesData.clear();
					m_drawList.mIndicesData.clear();
				}

				virtual void end(void* userData)
				{
					glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
					Color background_color = GuiColorStyle::getBackgroundColor();
					glClearColor(background_color.r / 255.0f, background_color.g / 255.0f, background_color.b / 255.0f, background_color.a / 255.0f);
					glViewport(0, 0, m_renderWidth, m_renderHeight);
					glClear(GL_COLOR_BUFFER_BIT);
					glDisable(GL_DEPTH_TEST);//禁用深度测试

					glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//告诉系统对透视进行修正
					if (m_sampleCount > 1)
						glEnable(GL_MULTISAMPLE);
					else
						glDisable(GL_MULTISAMPLE);

					glDisable(GL_CULL_FACE);
					glEnable(GL_BLEND);// 启用混合模式

					//glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
					glEnable(GL_SCISSOR_TEST);
					glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
					//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					//glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);
					//glBlendEquation(GL_FUNC_ADD);
					glFrontFace(GL_CW);

					glBindVertexArray(m_VAO);
					glBindBuffer(GL_ARRAY_BUFFER, m_VBO);	
					glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_drawList.mVerticesData.size(), m_drawList.mVerticesData.data(), GL_STATIC_DRAW);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

					const mat4 projectionMatrix = math::orthoWorldLH_DeviceLH<float>(0.0f, m_renderWidth, 0.0f, m_renderHeight, 0.0f, 2.0f);
					auto shader = m_pipeline.getShader(0);
					shader.use();
					HandleImage current_image = HandleImage(nullptr);
					int current_shader = 0;
					glActiveTexture(GL_TEXTURE0);

					for (auto& draw_buffer : m_drawList.mDrawCommand)
					{
						if (current_shader != draw_buffer.mPipeline)
						{
							current_shader = draw_buffer.mPipeline;
							switch (draw_buffer.mPipeline)
							{
							case 0:
								shader = m_pipeline.getShader(0);
								break;
							case 1:
								shader = m_pipeline.getShader(1);
								break;
							case 2:
								shader = m_pipeline.getShader(0);
								break;
							}
							shader.use();
						}
						shader.setUniform(0, projectionMatrix * draw_buffer.mTransform);
						
						if (current_image != draw_buffer.mImage)
						{
							Image* pImage = (Image*)draw_buffer.mImage.unused;							
							glBindTexture(GL_TEXTURE_2D, pImage->mImage);
							current_image = draw_buffer.mImage;
						}
						glScissor(draw_buffer.mScissor.mX, m_renderHeight - draw_buffer.mScissor.mHeight - draw_buffer.mScissor.mY, draw_buffer.mScissor.mWidth, draw_buffer.mScissor.mHeight);
						
						//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * draw_buffer.mVertexCount, m_drawList.mVerticesData.data() + draw_buffer.mVertexOffset, GL_STATIC_DRAW);
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * draw_buffer.mIndexCount, m_drawList.mIndicesData.data() + draw_buffer.mIndexOffset, GL_STATIC_DRAW);
						//glDrawElements(GL_TRIANGLES, draw_buffer.mIndexCount, GL_UNSIGNED_INT, nullptr);
						glDrawElementsBaseVertex(GL_TRIANGLES, draw_buffer.mIndexCount, GL_UNSIGNED_INT, nullptr, draw_buffer.mVertexOffset);
					}
					glBindVertexArray(0);
					// Flush command.
					glFlush();
				}

			};


			class OpenGLGuiCanvasRenderer : public CraftEngine::gui::AbstractGuiRenderer
			{
			public:
				typedef opengl::ImageTexture Image;
				OpenGLGuiRendererPipeline m_pipeline;

				GLuint m_FBO;

				GLuint m_VAO;
				GLuint m_EBO;
				GLuint m_VBO;

				uint32_t m_renderWidth;
				uint32_t m_renderHeight;
				int m_sampleCount = 1;
			public:

				void init(
					uint32_t initWidth,
					uint32_t initHeight,
					GLuint fbo,
					int sampleCount
				)
				{
					m_FBO = fbo;
					m_renderWidth = initWidth;
					m_renderHeight = initHeight;
					m_sampleCount = sampleCount;

					m_pipeline.init();
					m_pipeline.create();

					glGenVertexArrays(1, &m_VAO);
					glGenBuffers(1, &m_VBO);
					glGenBuffers(1, &m_EBO);
					glBindVertexArray(m_VAO);
					glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, pos));
					glEnableVertexAttribArray(1);
					glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex));
					glEnableVertexAttribArray(2);
					glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(Vertex), (const void*)offsetof(Vertex, col));
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindVertexArray(0);

				}

				virtual void clear() override
				{
					glDeleteBuffers(1, &m_EBO);
					glDeleteBuffers(1, &m_VBO);
					glDeleteVertexArrays(1, &m_VAO);
					m_pipeline.clear();
				}

				void resetRenderTarget(
					uint32_t initWidth,
					uint32_t initHeight,
					GLuint fbo
				) {
					m_FBO = fbo;
					m_renderWidth = initWidth;
					m_renderHeight = initHeight;
				}

				virtual void begin(void* userData)
				{
					m_drawList.mDrawCommand.clear();
					m_drawList.mVerticesData.clear();
					m_drawList.mIndicesData.clear();
				}

				virtual void end(void* userData)
				{
					glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
					Color background_color = GuiColorStyle::getBackgroundColor();
					glClearColor(background_color.r / 255.0f, background_color.g / 255.0f, background_color.b / 255.0f, 0.0f);
					glViewport(0, 0, m_renderWidth, m_renderHeight);
					glClear(GL_COLOR_BUFFER_BIT);
					glDisable(GL_DEPTH_TEST);//禁用深度测试

					glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//告诉系统对透视进行修正
					if (m_sampleCount > 1)
						glEnable(GL_MULTISAMPLE);
					else
						glDisable(GL_MULTISAMPLE);

					glDisable(GL_CULL_FACE);
					glEnable(GL_BLEND);// 启用混合模式

					//glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
					glEnable(GL_SCISSOR_TEST);
					glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
					//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					//glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);
					//glBlendEquation(GL_FUNC_ADD);
					glFrontFace(GL_CW);

					glBindVertexArray(m_VAO);
					glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
					glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_drawList.mVerticesData.size(), m_drawList.mVerticesData.data(), GL_STATIC_DRAW);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

					const mat4 projectionMatrix = math::orthoWorldLH_DeviceRH<float>(0.0f, m_renderWidth, 0.0f, m_renderHeight, 0.0f, 2.0f);
					auto shader = m_pipeline.getShader(0);
					shader.use();
					HandleImage current_image = HandleImage(nullptr);
					int current_shader = 0;
					glActiveTexture(GL_TEXTURE0);

					for (auto& draw_buffer : m_drawList.mDrawCommand)
					{
						if (current_shader != draw_buffer.mPipeline)
						{
							current_shader = draw_buffer.mPipeline;
							switch (draw_buffer.mPipeline)
							{
							case 0:
								shader = m_pipeline.getShader(0);
								break;
							case 1:
								shader = m_pipeline.getShader(1);
								break;
							case 2:
								shader = m_pipeline.getShader(0);
								break;
							}
							shader.use();
						}
						shader.setUniform(0, projectionMatrix * draw_buffer.mTransform);

						if (current_image != draw_buffer.mImage)
						{
							Image* pImage = (Image*)draw_buffer.mImage.unused;
							glBindTexture(GL_TEXTURE_2D, pImage->mImage);
							current_image = draw_buffer.mImage;
						}
						glScissor(draw_buffer.mScissor.mX, draw_buffer.mScissor.mY, draw_buffer.mScissor.mWidth, draw_buffer.mScissor.mHeight);

						//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * draw_buffer.mVertexCount, m_drawList.mVerticesData.data() + draw_buffer.mVertexOffset, GL_STATIC_DRAW);
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * draw_buffer.mIndexCount, m_drawList.mIndicesData.data() + draw_buffer.mIndexOffset, GL_STATIC_DRAW);
						//glDrawElements(GL_TRIANGLES, draw_buffer.mIndexCount, GL_UNSIGNED_INT, nullptr);
						glDrawElementsBaseVertex(GL_TRIANGLES, draw_buffer.mIndexCount, GL_UNSIGNED_INT, nullptr, draw_buffer.mVertexOffset);
					}
					glBindVertexArray(0);
					// Flush command.
					glFlush();
				}

			};

		}

	};

}
