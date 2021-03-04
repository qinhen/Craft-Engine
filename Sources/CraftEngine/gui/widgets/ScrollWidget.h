#pragma once
#ifndef CRAFT_ENGINE_GUI_SCROLL_WIDGET_H_
#define CRAFT_ENGINE_GUI_SCROLL_WIDGET_H_
#include "./Widget.h"
#include "./ScrollBar.h"

namespace CraftEngine
{
	namespace gui
	{


		class ScrollWidget : public Widget
		{
		private:
			uint32_t m_scrollBarWidth = 15;
			uint32_t m_scrollStep = 22;
		protected:
			VScrollBar* m_verticalScrollBar;
			HScrollBar* m_horizonScrollBar;
		private:
			void verticalScrollCallback(float cur, float total)
			{
				m_originalPoint.y = CraftEngine::math::clamp(cur + m_scrollLimit.mY, float(m_scrollLimit.top()), float(m_scrollLimit.bottom()));
			}
			void horizonScrollCallback(float cur, float total)
			{
				m_originalPoint.x = CraftEngine::math::clamp(cur + m_scrollLimit.mX, float(m_scrollLimit.left()), float(m_scrollLimit.right()));
			}

			/*
				OriginalPoint: INVERSE-relative-original-point reference child space coordinate system.
				child widgets with position p will be draw in p - original-point.
			*/
			Point  m_originalPoint = Point(0, 0);
			Rect   m_scrollLimit;
			Rect   m_srcScrollRect = Rect(0, 0, 0, 0);
		protected:
			/*
			 0: scroll widget space
			 1: srcoll bar
			*/
			virtual Point getChildOriginal(int groupid = GroupID::gDefaultWidget)const
			{
				switch (groupid)
				{
				case GroupID::gDefaultWidget:
					return -m_originalPoint;
				case GroupID::gScrollBar:
					return Point(0);
				default:
					return Point(0);
				}
			}

		public:

			~ScrollWidget()
			{
				delete m_horizonScrollBar;
				delete m_verticalScrollBar;
			}

			ScrollWidget(const Rect& rect, Widget* parent) : Widget(rect, parent)
			{
				int scroll_bar_width = getScrollBarWidth();
				m_verticalScrollBar = new VScrollBar(Rect(rect.right() - scroll_bar_width, 0, scroll_bar_width, rect.mHeight - scroll_bar_width), nullptr);
				m_verticalScrollBar->setGroupid(1);
				m_verticalScrollBar->bindParentWidget(this);
				m_horizonScrollBar = new HScrollBar(Rect(0, rect.bottom() - scroll_bar_width, rect.mWidth - scroll_bar_width, scroll_bar_width), nullptr);
				m_horizonScrollBar->setGroupid(1);
				m_horizonScrollBar->bindParentWidget(this);

				setScrollArea(Rect(Point(0), rect.mSize));
				setMouseWheelable(true);
				getPalette().mFrameMode = Palette::FrameMode::eFrameMode_Surround;
				
				m_horizonScrollBar->getPalette().mFrameMode = Palette::FrameMode::eFrameMode_Surround;
				m_verticalScrollBar->getPalette().mFrameMode = Palette::FrameMode::eFrameMode_Surround;
				craft_engine_gui_connect(m_verticalScrollBar, scroll, this, verticalScrollCallback);
				craft_engine_gui_connect(m_horizonScrollBar, scroll, this, horizonScrollCallback);
			}

			ScrollWidget(Widget* parent) : ScrollWidget(Rect(0, 0, 400, 400), parent)
			{

			}

			void scroll(int x_offset, int y_offset)
			{
				m_originalPoint.x = CraftEngine::math::clamp(m_originalPoint.x + x_offset, m_scrollLimit.left(), m_scrollLimit.right());
				m_horizonScrollBar->setScrollPos(m_originalPoint.x - m_scrollLimit.mX);
				m_originalPoint.y = CraftEngine::math::clamp(m_originalPoint.y + y_offset, m_scrollLimit.top(), m_scrollLimit.bottom());
				m_verticalScrollBar->setScrollPos(m_originalPoint.y - m_scrollLimit.mY);
			}

			uint32_t getScrollBarWidth()
			{
				return m_scrollBarWidth;
			}

			Rect getScrollArea() const
			{
				return m_srcScrollRect;
			}

			void setScrollArea(const Size& area)
			{
				setScrollArea(Rect(Point(0), area));
			}

			void setScrollArea(const Rect& area)
			{
				m_srcScrollRect = area;
				auto combine_area = area;
				if (area.mOffset.x != 0 || area.mOffset.y != 0)
					combine_area = Rect(0, 0, getWidth(), getHeight()).conjunction(area);
				Size temp_area;
				Size frame_size = getSize();
				temp_area.x = CraftEngine::math::max(combine_area.mWidth - getSize().x, 0);
				temp_area.y = CraftEngine::math::max(combine_area.mHeight - getSize().y, 0);
				m_verticalScrollBar->setHide(true);
				m_horizonScrollBar->setHide(true);

				if (this->getRect().mHeight < combine_area.mHeight)
				{
					m_verticalScrollBar->setHide(false);
					temp_area.x = CraftEngine::math::max(combine_area.mWidth - getSize().x + (int32_t)getScrollBarWidth(), 0);
					if (this->getRect().mWidth < combine_area.mWidth + getScrollBarWidth())
					{
						m_horizonScrollBar->setHide(false);
						temp_area.y = CraftEngine::math::max(combine_area.mHeight - getSize().y + (int32_t)getScrollBarWidth(), 0);
					}
				}
				else if (this->getRect().mWidth < combine_area.mWidth)
				{
					m_horizonScrollBar->setHide(false);
					temp_area.y = CraftEngine::math::max(combine_area.mHeight - getSize().y + (int32_t)getScrollBarWidth(), 0);
					if (this->getRect().mHeight < combine_area.mHeight + getScrollBarWidth())
					{
						m_verticalScrollBar->setHide(false);
						temp_area.x = CraftEngine::math::max(combine_area.mWidth - getSize().x + (int32_t)getScrollBarWidth(), 0);
					}
				}
				m_scrollLimit = Rect(combine_area.mX, combine_area.mY, temp_area.x, temp_area.y);

				if(!m_verticalScrollBar->isHide())
					frame_size.x -= getScrollBarWidth();
				if (!m_horizonScrollBar->isHide())
					frame_size.y -= getScrollBarWidth();

				m_verticalScrollBar->setScrollBarInfo(frame_size.y, m_scrollLimit.mHeight);
				m_horizonScrollBar->setScrollBarInfo(frame_size.x, m_scrollLimit.mWidth);

				auto temp_or = m_originalPoint;

				m_originalPoint.y = CraftEngine::math::clamp(m_verticalScrollBar->getScrollBarCurrentPos() + m_scrollLimit.mY, float(m_scrollLimit.top()), float(m_scrollLimit.bottom()));
				m_originalPoint.x = CraftEngine::math::clamp(m_horizonScrollBar->getScrollBarCurrentPos() + m_scrollLimit.mX, float(m_scrollLimit.left()), float(m_scrollLimit.right()));
				_On_Reset_Scroll_Bar_Layout();

				temp_or -= m_originalPoint;
				scroll(temp_or.x, temp_or.y);
			}

		protected:
			void onPaint_drawScrollBar()
			{		
				if (!m_verticalScrollBar->isHide())
					drawChild(m_verticalScrollBar);
				if (!m_horizonScrollBar->isHide())
					drawChild(m_horizonScrollBar);
			}
		public:
			virtual void onPaintEvent() override
			{
				drawBackground();
				Rect child_rect;
				Rect this_rect = Rect(Offset(0), getSize());
				for (int i = 0; i < getChildrenWidgets().size(); i++)
				{
					if (getChildrenWidgets()[i]->isHide() || getChildrenWidgets()[i]->isPopupable())
						continue;
					child_rect = getChildrenWidgets()[i]->getRect();
					child_rect.mOffset += getChildOriginal();
					if (Rect::haveIntersection(this_rect, child_rect))
						drawChild(getChildrenWidgets()[i]);
				}
				onPaint_drawScrollBar();
				drawFrame();
			}

		protected:	
			virtual Widget* onDetectScrollBar(const MouseEvent& mouseEvent)
			{
				Widget* hit;
				MouseEvent subEvent = mouseEvent;
				subEvent.local -= (getPos() + getChildOriginal(GroupID::gScrollBar));
				hit = m_horizonScrollBar->onMouseEvent(subEvent);
				if (hit != nullptr)
				{
					return hit;
				}
				hit = m_verticalScrollBar->onMouseEvent(subEvent);
				if (hit != nullptr)
				{
					return hit;
				}
				return nullptr;
			}

		public:
			virtual Widget* onDetect(const MouseEvent& mouseEvent)
			{
				if (isHide() || isTransparent() || (isPopupable() && !isNowPopup()) || !getRect().inside(mouseEvent.local))
					return nullptr;

				Widget* hit;

				hit = onDetectScrollBar(mouseEvent);
				if (hit != nullptr)
					return hit;

				MouseEvent subEvent = mouseEvent;
				for (int i = getChildrenWidgets().size() - 1; i >= 0; i--)
				{
					subEvent.local = mouseEvent.local - getPos() - getChildOriginal(getChildrenWidgets()[i]->getGroupid());
					hit = getChildrenWidgets()[i]->onMouseEvent(subEvent);
					if (hit != nullptr)
						return hit;
				}
				return this;
			}

		protected:

			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				setScrollArea(m_srcScrollRect);
			}

			virtual void onWheel(const MouseEvent& mouseEvent)
			{
				if (isMouseWheelable())
				{
					//const int ylen = getFont().getSize() * FontSystem::getGlobalFontYStandardLineHeight(getFont().getFontID()) * FontSystem::getGlobalFontScalar(getFont().getFontID());//
					const int ylen = _On_Get_Wheel_Step();
					if (KeyBoard::isKeyDown(KeyBoard::eKey_Shift))
					{
						m_originalPoint.x = CraftEngine::math::clamp(m_originalPoint.x + (mouseEvent.offset.y < 0 ? ylen : -ylen), m_scrollLimit.left(), m_scrollLimit.right());
						m_horizonScrollBar->setScrollPos(m_originalPoint.x - m_scrollLimit.mX);
					}
					else
					{
						m_originalPoint.y = CraftEngine::math::clamp(m_originalPoint.y + (mouseEvent.offset.y < 0 ? ylen : -ylen), m_scrollLimit.top(), m_scrollLimit.bottom());
						m_verticalScrollBar->setScrollPos(m_originalPoint.y - m_scrollLimit.mY);
					}
				}
			}

		protected:
			virtual int _On_Get_Wheel_Step()
			{
				return m_scrollStep;
			}

			virtual void _On_Reset_Scroll_Bar_Layout()
			{
				auto scroll_bar_width = getScrollBarWidth();
				if (m_horizonScrollBar->isHide())
					m_verticalScrollBar->setRect(Rect(getRect().mWidth - scroll_bar_width, 0, scroll_bar_width, getRect().mHeight));
				else if (m_verticalScrollBar->isHide())
					m_horizonScrollBar->setRect(Rect(0, getRect().mHeight - scroll_bar_width, getRect().mWidth, scroll_bar_width));
				else
				{
					m_verticalScrollBar->setRect(Rect(getRect().mWidth - scroll_bar_width, 0, scroll_bar_width, getRect().mHeight - scroll_bar_width));
					//m_horizonScrollBar->setRect(Rect(0, getRect().mHeight - scroll_bar_width, getRect().mWidth - scroll_bar_width, scroll_bar_width));
					m_horizonScrollBar->setRect(Rect(0, getRect().mHeight - scroll_bar_width, getRect().mWidth, scroll_bar_width));
				}
			}

		};


	}

}


#endif // !CRAFT_ENGINE_GUI_SCROLL_WIDGET_H_


