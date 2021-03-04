#pragma once
#ifndef CRAFT_ENGINE_GUI_OGL_WIDGET_H_
#define CRAFT_ENGINE_GUI_OGL_WIDGET_H_
/*
* 
* 
* 
*/
#include "../../advance/opengl/OpenGLSystem.h"
#include "../widgets/Widget.h"

namespace CraftEngine
{
	namespace gui
	{


		class OpenGLWidget :public Widget
		{
		private:
			using vec2 = CraftEngine::math::vec2;
			using vec3 = CraftEngine::math::vec3;
			using mat3 = CraftEngine::math::mat3;
			using mat2 = CraftEngine::math::mat2;
			using vec4 = CraftEngine::math::vec4;
			typedef opengl::ImageTexture Image;
			Image m_image;
			HandleImage m_imageHandle;

			uint32_t m_renderFuncKey = 0xFFFFFFFF;
			uint32_t m_resizeFuncKey = 0xFFFFFFFF;

		public:

			OpenGLWidget(Widget* parent): Widget(parent)
			{

			}

			~OpenGLWidget()
			{
				clearResizeFunc();
				clearRenderFunc();
			}

			void clearBinding()
			{
				m_imageHandle = HandleImage(nullptr);
			}

			void bindImage(GLuint image)
			{
				clearBinding();
				if (image != 0)
				{
					m_image.mImage = image;
					m_imageHandle = HandleImage(&m_image);
				}
			}

			void onPaintEvent() override
			{
				auto painter = getPainter();
				Widget::onPaintEvent();
				auto this_global_rect = Rect(Point(0), getSize());

				//GuiRenderer::PolygonInfo::Vertex m_backgroundVertexList[6];
				//m_backgroundVertexList[0].position = vec2(0.0f, 0.0f);
				//m_backgroundVertexList[1].position = vec2(1.0f, 0.0f);
				//m_backgroundVertexList[2].position = vec2(1.0f, 1.0f);
				//m_backgroundVertexList[3].position = vec2(1.0f, 1.0f);
				//m_backgroundVertexList[4].position = vec2(0.0f, 1.0f);
				//m_backgroundVertexList[5].position = vec2(0.0f, 0.0f);
				//for (int i = 0; i < 6; i++)
				//	m_backgroundVertexList[i].texcoord = m_backgroundVertexList[i].position;
				//for (int i = 0; i < 6; i++)
				//	m_backgroundVertexList[i].texcoord = m_backgroundVertexList[i].position * 0.0625f * vec2(getSize());

				//GuiRenderer::PolygonInfo info;
				//info.mode = info.ePolygonMode_Triangle;
				//info.vertexCount = 6;
				//info.vertexList = m_backgroundVertexList;
				//info.scale = vec2(getSize());
				//info.translate = vec2(this_global_rect.mOffset);
				//info.rotation = 0;
				//renderer->drawPolygon(info, m_image, global_limit_rect);

				this_global_rect.mY = this_global_rect.mY + this_global_rect.mHeight;
				this_global_rect.mHeight = -this_global_rect.mHeight;
				painter.drawRect(this_global_rect, m_imageHandle);
			}

			void setRenderFunc(std::function<void(void)> func)
			{
				clearRenderFunc();
				auto main_window = getRootWidget();
				m_renderFuncKey = craft_engine_gui_connect_v2(main_window, renderStarted, func);
			}
			void clearRenderFunc()
			{
				auto main_window = getRootWidget();
				if (m_renderFuncKey != 0xFFFFFFFF)
					craft_engine_gui_disconnect(main_window, renderStarted, m_renderFuncKey);
				m_renderFuncKey = 0xFFFFFFFF;
			}
			void setResizeFunc(std::function<void(int, int)> func)
			{
				clearResizeFunc();
				auto main_window = getRootWidget();
				m_resizeFuncKey = craft_engine_gui_connect_v2(main_window, resized, [=](const CraftEngine::gui::Size& size) { func(size.x, size.y); });
			}
			void clearResizeFunc()
			{
				auto main_window = getRootWidget();
				if (m_resizeFuncKey != 0xFFFFFFFF)
					craft_engine_gui_disconnect(main_window, resized, m_resizeFuncKey);
				m_resizeFuncKey = 0xFFFFFFFF;
			}

			void fillMainWindow()
			{
				auto main_window = getRootWidget();
				setRect(main_window->getRect());
			}

		};


	}
}

#endif // CRAFT_ENGINE_GUI_OGL_WIDGET_H_