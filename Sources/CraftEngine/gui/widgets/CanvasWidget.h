#pragma once
#include "./Widget.h"

namespace CraftEngine
{
	namespace gui
	{

		class CanvasWidget : public Widget
		{
		private:
			AbstractGuiRenderer* m_canvasRenderer;
			AbstractGuiCanvas* m_canvas;
			bool m_canvasUpdateEveryFrame = true;
		public:
			CanvasWidget(Widget* parent) : Widget(parent)
			{
				m_canvas = GuiRenderSystem::createGuiCanvas(getSize());
				m_canvasRenderer = m_canvas->getGuiRenderer();
			}
			~CanvasWidget()
			{
				GuiRenderSystem::deleteGuiCanvas(m_canvas);
			}

			void draw()
			{
				m_canvasRenderer->begin(nullptr);
				//drawBackground();
				drawAllChildren();
				m_canvasRenderer->end(nullptr);
			}

			virtual void onPaintEvent() override
			{
				if (m_canvasUpdateEveryFrame)
					draw();
				auto painter = getPainter();
				painter.drawRect(Rect(Point(0), getSize()), m_canvas->getFramebufferImage());
				drawFrame();
			}
		protected:
			virtual void drawAllChildren()
			{
				PaintEvent subEvent;
				subEvent.pRenderer = m_canvasRenderer;
				subEvent.globalRelativePoint = getChildOriginal(GroupID::gDefaultWidget);
				subEvent.globalLimitRect = Rect(Point(0), getSize());
				subEvent.globalAlpha = 255;
				for (auto child : getChildrenWidgets())
				{
					if (child->isHide() || child->isPopupable())
						continue;
					child->processPaintEvent(subEvent);
					child->onPaintEvent();
				}
			}
			virtual void drawChild(Widget* widget)
			{
				PaintEvent subEvent;
				subEvent.pRenderer = m_canvasRenderer;
				subEvent.globalRelativePoint = getChildOriginal(widget->getGroupid());
				subEvent.globalLimitRect = Rect(Point(0), getSize());
				subEvent.globalAlpha = 255;
				widget->processPaintEvent(subEvent);
				widget->onPaintEvent();
			}
			virtual void drawChild2(Widget* widget)
			{
				PaintEvent subEvent;
				subEvent.pRenderer = m_canvasRenderer;
				subEvent.globalRelativePoint = getChildOriginal(widget->getGroupid());
				subEvent.globalLimitRect = Rect(Point(0), getSize());
				subEvent.globalAlpha = 255;
				widget->processPaintEvent(subEvent);
				widget->onPaintEvent();
			}
		protected:
			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				GuiRenderSystem::deleteGuiCanvas(m_canvas);
				m_canvas = GuiRenderSystem::createGuiCanvas(getSize());
				m_canvasRenderer = m_canvas->getGuiRenderer();
			}
		private:


		};





	}

}
