#pragma once
#ifndef CRAFT_ENGINE_GUI_GRAPHICS_WIDGET_H_
#define CRAFT_ENGINE_GUI_GRAPHICS_WIDGET_H_
#include "./Widget.h"
namespace CraftEngine
{
	namespace gui
	{

		/*
		 Widget

		 
		 
		*/
		class AbstractGraphicsWidget : public Widget
		{
		private:

			GraphicsRenderer* m_renderer;
			core::Thread m_thread;
			std::atomic_bool m_isDrawing = false;
			std::atomic_bool m_shouldDraw = false;
		public:

			virtual ~AbstractGraphicsWidget()
			{
				m_thread.wait();
				m_renderer->clear();
				delete[] m_renderer;
			}

			AbstractGraphicsWidget(const Rect& rect,Widget* parent) : Widget(rect, parent)
			{
				setDragable(true);
				auto main_window = getRootWidget();
				auto renderer = main_window->getRenderer();
				m_renderer = renderer->createGraphicsRenderer(rect.mWidth, rect.mHeight);
			}

			AbstractGraphicsWidget(Widget* parent) : AbstractGraphicsWidget(Rect(0, 0, 400, 400), parent)
			{

			}

			GraphicsRenderer* getGraphRenderer() { return m_renderer; }
			core::Thread& getThread() { return m_thread; }

			virtual bool callDraw()
			{
				if (m_shouldDraw)
				{
					m_isDrawing = true;
					core::Command<void> cmd;
					cmd.wrap(this, &AbstractGraphicsWidget::_Call_Draw);
					m_thread.push(cmd);
					m_shouldDraw = false;
					return true;
				}

				if (!m_isDrawing)
				{
					m_isDrawing = true;
					core::Command<void> cmd;
					cmd.wrap(this, &AbstractGraphicsWidget::_Call_Draw);
					m_thread.push(cmd);
					return true;
				}
				else if(m_shouldDraw == false)
				{
					m_shouldDraw = true;
					return true;
				}
				return false;
			}

		protected:

			virtual void onPaint(GraphicsRenderer* renderer) = 0;

		private:

			void _Call_Draw()
			{
				m_renderer->begin(0);
				onPaint(m_renderer);
				m_renderer->end(0);
				m_isDrawing = false;
			}
			
			virtual void onPaintEvent(const PaintEvent& paintEvent) override
			{
				GuiRenderer* renderer = paintEvent.pRenderer;
				const Point& global_relative_point = paintEvent.globalRelativePoint;
				const Rect& global_limit_rect = paintEvent.globalLimitRect;

				if (m_shouldDraw)
					callDraw();

				Widget::onPaintEvent(paintEvent);
				auto this_global_rect = Rect(global_relative_point + getPos(), getSize());
				renderer->drawRect(this_global_rect, m_renderer->getCurrentColorBuffer(), global_limit_rect);
			}


			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				m_thread.wait();
				m_renderer->resize(resizeEvent.size);
				callDraw();
			}
		};

	}

}





#endif // !CRAFT_ENGINE_GUI_VULKAN_WIDGET_H_


