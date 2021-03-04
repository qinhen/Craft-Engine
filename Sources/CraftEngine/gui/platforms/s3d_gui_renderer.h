#pragma once

#include "../Common.h"

#include "./s3d_gui_pipeline.h"

namespace CraftEngine
{
	namespace gui
	{

		namespace _Soft3D_Render_System_Detail
		{
			constexpr int MAX_VERTEX_COUNT = 1024;

			class AbstractSoft3DGuiRenderer : public CraftEngine::gui::AbstractGuiRenderer
			{
			public:
				int m_width;
				int m_height;
				core::ArrayList<soft3d::Image> m_swapchainImages;
				core::ArrayList<soft3d::FrameBuffer> m_swapchainFramebuffers;
				soft3d::Device m_device;
				soft3d::GraphicsContext m_context;
				int m_framebufferIndex;
				Soft3DGuiRendererPipeline m_pipeline;

				using Buffer = soft3d::Buffer;
				using Image = soft3d::Image;
				using Memory = soft3d::Memory;
				using Sampler = soft3d::Sampler;
				Buffer m_renderInfoBuffer;
				Sampler m_sampler;
				Buffer m_rectVertexBuffer;
				Buffer m_rectIndexBuffer;
				HandleImage m_pureWhiteImage = HandleImage(nullptr);
			public:

				void setFrameBufferIndex(int index)
				{
					m_framebufferIndex = index;
				}

				void init(
					int initWidth,
					int initHeight,
					const core::ArrayList<soft3d::Image>& swapchainImages,
					int multiSampleCount
				)
				{
					m_width = initWidth;
					m_height = initHeight;

					m_swapchainFramebuffers.resize(swapchainImages.size());
					for (int i = 0; i < swapchainImages.size(); i++)
					{
						m_swapchainFramebuffers[i] = soft3d::createFrameBuffer();
					}
					m_pipeline.init();
					m_pipeline.create();

					m_renderInfoBuffer = soft3d::createBuffer(sizeof(Soft3DGuiRendererPipeline::RenderInfo));
					m_renderInfoBuffer.bindMemory(soft3d::createMemory(sizeof(Soft3DGuiRendererPipeline::RenderInfo)), 0);
					m_sampler = soft3d::createSampler(
						soft3d::SamplerFilterType::eLinear, soft3d::SamplerAddressMode::eRepeat,
						soft3d::SamplerBorderColor::eWhiteFloat, soft3d::SamplerMipmapMode::eNearestMipmap,
						0.0f, 32.0f
					);
					const float vertices[8] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
					m_rectVertexBuffer = soft3d::createBuffer(sizeof(vertices));
					m_rectVertexBuffer.bindMemory(soft3d::createMemory(sizeof(vertices)), 0);
					m_rectVertexBuffer.write(vertices, sizeof(vertices), 0);
					const uint32_t indices[6] = { 0,1,2,2,3,0 };
					m_rectIndexBuffer = soft3d::createBuffer(sizeof(indices));
					m_rectIndexBuffer.bindMemory(soft3d::createMemory(sizeof(indices)), 0);
					m_rectIndexBuffer.write(indices, sizeof(indices), 0);


					uint32_t whiteImageData[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
					m_pureWhiteImage = GuiRenderSystem::createImage(whiteImageData, 2 * 2 * 4, 2, 2);
					m_device = soft3d::createDevice();
					m_context = soft3d::createGraphicsContext(m_device);

					resetRenderTarget(initWidth, initHeight, swapchainImages);
				}


				virtual void clear()override
				{
					m_pipeline.clear();
					soft3d::destroySampler(m_sampler);
					soft3d::destroyMemory(m_renderInfoBuffer.memory());
					soft3d::destroyBuffer(m_renderInfoBuffer);
					soft3d::destroyMemory(m_rectVertexBuffer.memory());
					soft3d::destroyBuffer(m_rectVertexBuffer);
					soft3d::destroyMemory(m_rectIndexBuffer.memory());
					soft3d::destroyBuffer(m_rectIndexBuffer);

					GuiRenderSystem::deleteImage(m_pureWhiteImage);

					for (auto& it : m_swapchainFramebuffers)
					{
						soft3d::destroyFrameBuffer(it);
					}
					m_swapchainFramebuffers.clear();
					soft3d::destroyGraphicsContext(m_context);
					soft3d::destroyDevice(m_device);
				}


				void resetRenderTarget(
					int w,
					int h,
					const core::ArrayList<soft3d::Image>& swapchainImages
				) {
					m_swapchainImages = swapchainImages;
					m_width = w;
					m_height = h;
					for (int i = 0; i < m_swapchainFramebuffers.size(); i++)
					{
						m_swapchainFramebuffers[i].clearBindings();
						m_swapchainFramebuffers[i].bindColorTarget(m_swapchainImages[i]);
					}
				}

				void wait()
				{
					m_device.waitDevice();
				}

				void stop()
				{
					m_device.stopDevice();
				}

				// Í¨¹ý GuiRenderer ¼Ì³Ð
				virtual void drawRect(const Rect& rect, const Color& color, const Rect& boundary) override
				{
					m_context.bindPipeline(m_pipeline.mPipelines[0]);
					m_context.setScissor(*((soft3d::Scissor*)&boundary));
					Soft3DGuiRendererPipeline::PushContant0 push_constants;
					push_constants.ltwh = rect;
					push_constants.color = *((uint32_t*)&color);
					m_context.pushConstants(&push_constants, sizeof(push_constants));
					m_context.drawIndex(6, 1, 0, 0, 0);
				}

				virtual void drawRect(const Rect& rect, const CraftEngine::gui::HandleImage image, const Rect& boundary) override
				{
					if (image.unused != nullptr)
					{
						auto pimage = (Image*)image.unused;
						m_context.bindPipeline(m_pipeline.mPipelines[1]);
						m_context.setScissor(*((soft3d::Scissor*)&boundary));
						Soft3DGuiRendererPipeline::PushContant1 push_constants;
						push_constants.ltwh = rect;
						m_context.bindTexture(*pimage, 0);
						m_context.pushConstants(&push_constants, sizeof(push_constants));
						m_context.drawIndex(6, 1, 0, 0, 0);
					}
				}


				virtual void drawTextLine(const Char* pStr, uint32_t count, const Point& point, Font& font, const Color& color, const Rect& boundary) override
				{
					if (count <= 0)
						return;
					float scalar = font.getSize() * GuiFontSystem::getGlobalFontScalar(font.getFontID());
					int interval = font.getInterval();

					m_context.setScissor(*((soft3d::Scissor*)&boundary));
					Soft3DGuiRendererPipeline::PushContant2 push_constants;
					push_constants.color = *((uint32_t*)&color);
					Point cp = point;
					m_context.bindPipeline(m_pipeline.mPipelines[2]);

					const Image* font_image = (Image*)(GuiFontSystem::getGlobalFontImage(font.getFontID()).unused);
					m_context.bindTexture(*font_image, 0);
					m_context.enableNoBlock(true);
					for (int i = 0; i < count; i++)
					{
						auto& slot = GuiFontSystem::getGlobalFontUnit(pStr[i], font.getFontID());
						push_constants.ltwh = math::vec4(cp.x + slot.xoffset * scalar, cp.y + slot.yoffset * scalar, slot.width * scalar, slot.height * scalar);
						push_constants.uvltwh = Rect(slot.xcoord, slot.ycoord, slot.width, slot.height);
						m_context.pushConstants(&push_constants, sizeof(push_constants));
						m_context.drawIndex(6, 1, 0, 0, 0);
						cp.x += GuiFontSystem::step_forward(slot.xadvance, scalar, interval); // 
					}
					m_context.enableNoBlock(false);
				}


				virtual void drawColorTextLine(const Char* pStr, uint32_t count, const Point& point, Font& font, const Color* colorList, const Rect& boundary) override
				{
					if (count <= 0)
						return;
					float scalar = font.getSize() * GuiFontSystem::getGlobalFontScalar(font.getFontID());
					int interval = font.getInterval();

					m_context.setScissor(*((soft3d::Scissor*)&boundary));
					Soft3DGuiRendererPipeline::PushContant2 push_constants;
					Point cp = point;
					m_context.bindPipeline(m_pipeline.mPipelines[2]);

					const Image* font_image = (Image*)(GuiFontSystem::getGlobalFontImage(font.getFontID()).unused);
					m_context.bindTexture(*font_image, 0);
					m_context.enableNoBlock(true);
					for (int i = 0; i < count; i++)
					{
						auto& slot = GuiFontSystem::getGlobalFontUnit(pStr[i], font.getFontID());
						push_constants.ltwh = math::vec4(cp.x + slot.xoffset * scalar, cp.y + slot.yoffset * scalar, slot.width * scalar, slot.height * scalar);
						push_constants.uvltwh = Rect(slot.xcoord, slot.ycoord, slot.width, slot.height);
						push_constants.color = *((uint32_t*)&colorList[i]);
						m_context.pushConstants(&push_constants, sizeof(push_constants));
						m_context.drawIndex(6, 1, 0, 0, 0);
						cp.x += GuiFontSystem::step_forward(slot.xadvance, scalar, interval); // 
					}
					m_context.enableNoBlock(false);
				}
				virtual void drawPaletteTextLine(const Char* pStr, uint32_t count, const Point& point, Font& font, const Color* colorList, const uint16_t* colorIdxList, const Rect& boundary)override
				{
					if (count <= 0)
						return;
					float scalar = font.getSize() * GuiFontSystem::getGlobalFontScalar(font.getFontID());
					int interval = font.getInterval();

					m_context.setScissor(*((soft3d::Scissor*)&boundary));
					Soft3DGuiRendererPipeline::PushContant2 push_constants;
					Point cp = point;
					m_context.bindPipeline(m_pipeline.mPipelines[2]);

					const Image* font_image = (Image*)(GuiFontSystem::getGlobalFontImage(font.getFontID()).unused);
					m_context.bindTexture(*font_image, 0);
					m_context.enableNoBlock(true);
					for (int i = 0; i < count; i++)
					{
						auto& slot = GuiFontSystem::getGlobalFontUnit(pStr[i], font.getFontID());
						push_constants.ltwh = math::vec4(cp.x + slot.xoffset * scalar, cp.y + slot.yoffset * scalar, slot.width * scalar, slot.height * scalar);
						push_constants.uvltwh = Rect(slot.xcoord, slot.ycoord, slot.width, slot.height);
						push_constants.color = *((uint32_t*)&colorList[colorIdxList[i]]);
						m_context.pushConstants(&push_constants, sizeof(push_constants));
						m_context.drawIndex(6, 1, 0, 0, 0);
						cp.x += GuiFontSystem::step_forward(slot.xadvance, scalar, interval); // 
					}
					m_context.enableNoBlock(false);
				}


				virtual void drawRectFrame(const Rect& rect, const Color& color, const Rect& boundary) override
				{
					m_context.bindPipeline(m_pipeline.mPipelines[3]);
					m_context.setScissor(*((soft3d::Scissor*)&boundary));
					Soft3DGuiRendererPipeline::PushContant0 push_constants;
					push_constants.ltwh = rect;
					push_constants.color = *((uint32_t*)&color);
					m_context.pushConstants(&push_constants, sizeof(push_constants));
					m_context.enableNoBlock(true);
					m_context.drawIndex(6, 1, 0, 0, 0);
					m_context.enableNoBlock(false);
				}


				virtual void drawSDF(const Rect& rect, const Color& color, const HandleImage image, const Rect& boundary)
				{
					if (image.unused != nullptr)
					{
						auto pimage = (Image*)image.unused;
						m_context.bindPipeline(m_pipeline.mPipelines[4]);
						m_context.setScissor(*((soft3d::Scissor*)&boundary));
						Soft3DGuiRendererPipeline::PushContant0 push_constants;
						push_constants.ltwh = rect;
						push_constants.color = *((uint32_t*)&color);
						m_context.bindTexture(*pimage, 0);
						m_context.pushConstants(&push_constants, sizeof(push_constants));
						m_context.drawIndex(6, 1, 0, 0, 0);
					}
				}

				virtual void drawRichTextLine(const Char* pStr, uint32_t count, const Point& point, Font& font, const RichTextFormat& format, const Rect& boundary)
				{
					
				}




				struct Vertex
				{
					math::vec2 position;
					math::vec2 texcoord;
					Color color;
				};

				virtual void drawPolygon(const PolygonInfo& info, const Color& color, const Rect& boundary) override
				{
					soft3d::Buffer vbuf;
					soft3d::Buffer ibuf;
					copyPolygonData(info, vbuf, ibuf);
					m_context.bindVertexBuffer(vbuf);
					m_context.bindIndexBuffer(ibuf);

					for (int i = 0; i < info.vertexCount; i++)
						((Vertex*)vbuf.data())[i].color = color;

					int pipelineIndex = 6;
					switch (info.mode)
					{
					case PolygonInfo::ePolygonMode_Triangle:pipelineIndex = 6;
						break;
					case PolygonInfo::ePolygonMode_TriangleFrame:pipelineIndex = 7;
						break;
					case PolygonInfo::ePolygonMode_Lines:pipelineIndex = 8;
						break;
					case PolygonInfo::ePolygonMode_LineStrip:pipelineIndex = 9;
						break;
					case PolygonInfo::ePolygonMode_Points:pipelineIndex = 10;
						break;
					default:
						break;
					}
					m_context.bindPipeline(m_pipeline.mPipelines[pipelineIndex]);
					m_context.setScissor(*((soft3d::Scissor*)&boundary));

					Soft3DGuiRendererPipeline::PushContant6 push_constants;
					math::mat3 model = math::mat3();
					model = math::rotate2D(info.rotation, model);
					model = math::scale2D(info.scale, model);
					model = math::translate2D(info.translate, model);
					push_constants.model = model;
					const Image* pimage = (Image*)(m_pureWhiteImage.unused);
					m_context.bindTexture(*pimage, 0);
					m_context.pushConstants(&push_constants, sizeof(push_constants));

					//if (info.lineWidth > 1)
					//	vkCmdSetLineWidth(m_cmdBuffer, info.lineWidth);
					//m_context.enableNoBlock(true);
					if (info.indexList != nullptr)
						m_context.drawIndex(info.indexCount, 1, 0, 0, 0);
					else
						m_context.drawVertex(info.vertexCount, 1, 0, 0);
					//m_context.enableNoBlock(false);

					freePolygonData(vbuf, ibuf);
					m_context.bindVertexBuffer(m_rectVertexBuffer);
					m_context.bindIndexBuffer(m_rectIndexBuffer);
				}


				virtual void drawPolygon(const PolygonInfo& info, const Rect& boundary) override
				{
					soft3d::Buffer vbuf;
					soft3d::Buffer ibuf;
					copyPolygonData(info, vbuf, ibuf);
					m_context.bindVertexBuffer(vbuf);
					m_context.bindIndexBuffer(ibuf);

					int pipelineIndex = 6;
					switch (info.mode)
					{
					case PolygonInfo::ePolygonMode_Triangle:pipelineIndex = 6;
						break;
					case PolygonInfo::ePolygonMode_TriangleFrame:pipelineIndex = 7;
						break;
					case PolygonInfo::ePolygonMode_Lines:pipelineIndex = 8;
						break;
					case PolygonInfo::ePolygonMode_LineStrip:pipelineIndex = 9;
						break;
					case PolygonInfo::ePolygonMode_Points:pipelineIndex = 10;
						break;
					default:
						break;
					}
					m_context.bindPipeline(m_pipeline.mPipelines[pipelineIndex]);
					m_context.setScissor(*((soft3d::Scissor*)&boundary));

					Soft3DGuiRendererPipeline::PushContant6 push_constants;
					math::mat3 model = math::mat3();
					model = math::rotate2D(info.rotation, model);
					model = math::scale2D(info.scale, model);
					model = math::translate2D(info.translate, model);
					push_constants.model = model;
					const Image* pimage = (Image*)(m_pureWhiteImage.unused);
					m_context.bindTexture(*pimage, 0);
					m_context.pushConstants(&push_constants, sizeof(push_constants));

					//if (info.lineWidth > 1)
					//	vkCmdSetLineWidth(m_cmdBuffer, info.lineWidth);
					//m_context.enableNoBlock(true);
					if (info.indexList != nullptr)
						m_context.drawIndex(info.indexCount, 1, 0, 0, 0);
					else
						m_context.drawVertex(info.vertexCount, 1, 0, 0);
					//m_context.enableNoBlock(false);

					freePolygonData(vbuf, ibuf);
					m_context.bindVertexBuffer(m_rectVertexBuffer);
					m_context.bindIndexBuffer(m_rectIndexBuffer);
				}


				virtual void drawPolygon(const PolygonInfo& info, const HandleImage image, const Rect& boundary)
				{
					soft3d::Buffer vbuf;
					soft3d::Buffer ibuf;
					copyPolygonData(info, vbuf, ibuf);
					m_context.bindVertexBuffer(vbuf);
					m_context.bindIndexBuffer(ibuf);

					int pipelineIndex = 6;
					switch (info.mode)
					{
					case PolygonInfo::ePolygonMode_Triangle:pipelineIndex = 6;
						break;
					case PolygonInfo::ePolygonMode_TriangleFrame:pipelineIndex = 7;
						break;
					case PolygonInfo::ePolygonMode_Lines:pipelineIndex = 8;
						break;
					case PolygonInfo::ePolygonMode_LineStrip:pipelineIndex = 9;
						break;
					case PolygonInfo::ePolygonMode_Points:pipelineIndex = 10;
						break;
					default:
						break;
					}
					m_context.bindPipeline(m_pipeline.mPipelines[pipelineIndex]);
					m_context.setScissor(*((soft3d::Scissor*)&boundary));

					Soft3DGuiRendererPipeline::PushContant6 push_constants;
					math::mat3 model = math::mat3();
					model = math::rotate2D(info.rotation, model);
					model = math::scale2D(info.scale, model);
					model = math::translate2D(info.translate, model);
					push_constants.model = model;
					const Image* pimage = (Image*)(image.unused);
					m_context.bindTexture(*pimage, 0);
					m_context.pushConstants(&push_constants, sizeof(push_constants));

					//if (info.lineWidth > 1)
					//	vkCmdSetLineWidth(m_cmdBuffer, info.lineWidth);
					//m_context.enableNoBlock(true);
					if (info.indexList != nullptr)
						m_context.drawIndex(info.indexCount, 1, 0, 0, 0);
					else
						m_context.drawVertex(info.vertexCount, 1, 0, 0);
					//m_context.enableNoBlock(false);

					freePolygonData(vbuf, ibuf);
					m_context.bindVertexBuffer(m_rectVertexBuffer);
					m_context.bindIndexBuffer(m_rectIndexBuffer);
				}


			private:
				void freePolygonData(soft3d::Buffer& vbuf, soft3d::Buffer& ibuf)
				{
					soft3d::destroyMemory(vbuf.memory());
					soft3d::destroyMemory(ibuf.memory());
					soft3d::destroyBuffer(vbuf);
					soft3d::destroyBuffer(ibuf);
				}
				void copyPolygonData(const PolygonInfo& info, soft3d::Buffer& vbuf, soft3d::Buffer& ibuf)
				{
					auto vertex_buf_size = info.vertexCount * sizeof(Vertex);
					auto index_buf_size = info.indexCount * sizeof(uint32_t);
					vbuf = soft3d::createBuffer(vertex_buf_size);
					ibuf = soft3d::createBuffer(index_buf_size);
					vbuf.bindMemory(soft3d::createMemory(vertex_buf_size), 0);
					ibuf.bindMemory(soft3d::createMemory(index_buf_size), 0);

					if (info.mixBuffer != nullptr)
					{
						memcpy(vbuf.data(), info.mixBuffer, sizeof(info.mixBuffer[0]) * info.vertexCount);
					}
					else
					{
						if (info.posBuffer != nullptr)
						{
							for (int i = 0; i < info.vertexCount; i++)
								((Vertex*)vbuf.data())[i].position = info.posBuffer[i];
						}
						if (info.texBuffer != nullptr)
						{
							for (int i = 0; i < info.vertexCount; i++)
								((Vertex*)vbuf.data())[i].texcoord = info.texBuffer[i];
						}
						else
						{
							for (int i = 0; i < info.vertexCount; i++)
								((Vertex*)vbuf.data())[i].texcoord = math::vec2(0.5f);
						}
						if (info.colBuffer != nullptr)
						{
							for (int i = 0; i < info.vertexCount; i++)
								((Vertex*)vbuf.data())[i].color = info.colBuffer[i];
						}
						else
						{
							for (int i = 0; i < info.vertexCount; i++)
								((Vertex*)vbuf.data())[i].color = Color(255);
						}
					}

					if (info.indexList != nullptr)
					{
						for (int i = 0; i < info.indexCount; i++)
							((uint32_t*)ibuf.data())[i] = info.indexList[i];
					}
				}
			};




			class Soft3DGuiRenderer : public AbstractSoft3DGuiRenderer
			{
			public:

				virtual void begin(void* userData)
				{

					m_context.bindFrameBuffer(m_swapchainFramebuffers[m_framebufferIndex]);
					m_context.bindSampler(m_sampler, 0);
					m_context.bindBuffer(m_renderInfoBuffer, 0);

					Soft3DGuiRendererPipeline::RenderInfo info;
					info.renderArea.x = m_width;
					info.renderArea.y = m_height;
					memcpy(m_renderInfoBuffer.data(), &info, sizeof(info));

					m_context.bindVertexBuffer(m_rectVertexBuffer);
					m_context.bindIndexBuffer(m_rectIndexBuffer);
					//auto framebuffer_data = (soft3d::detail::FrameBufferData*)m_swapchainFramebuffers[m_framebufferIndex].handle();
					auto& background_color = GuiColorStyle::getBackgroundColor();
					soft3d::imgClear(m_swapchainImages[m_framebufferIndex], math::vec4(background_color) * (1.0f / 255.0f));
/*					soft3d::imgSetZero(m_swapchainImages[m_framebufferIndex]);	*/		
				}

				virtual void end(void* userData)
				{

				}

			};





		}

	};

}
