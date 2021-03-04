/*
 

	Latest revision history:
		(2020-04-28) add control button for scrollbar
*/
#pragma once
#ifndef CRAFT_ENGINE_GUI_SCROLL_BAR_H_
#define CRAFT_ENGINE_GUI_SCROLL_BAR_H_
#include "./Slider.h"
#include "./PushButton.h"
namespace CraftEngine
{
	namespace gui
	{



		class VScrollBar : public Widget
		{
		private:

			PushButton* m_upButton;
			PushButton* m_downButton;

			Slider* m_slider;
			float m_curPos = 0;
			float m_frameLen = 0;
			float m_totalLen = 0;
			int32_t m_stepLength = 20;

			uint32_t getSilderSize()
			{
				if (m_frameLen == 0)
					return getSize().y;
				return m_frameLen == 0 ? getSize().y - 2 * getWidth(): CraftEngine::math::max((getSize().y - 2 * getWidth())* m_frameLen / (m_totalLen + m_frameLen), 15.0f);
			}

			void sliderCallback()
			{
				auto coord = m_slider->getPos().y - getWidth();
				auto sliderSize = getSilderSize();
				auto total = getSize().y - sliderSize - 2 * getWidth();
				auto pos = coord * m_totalLen / (total > 0 ? total : 1);
				m_curPos = CraftEngine::math::clamp(pos, 0.0f, m_totalLen);
				craft_engine_gui_emit(scroll, m_curPos, m_totalLen);
			}

			void updateSlider()
			{
				auto sliderSize = getSilderSize();
				auto total = getSize().y - sliderSize - 2 * getWidth();
				auto coord = getWidth() + total * m_curPos / (m_totalLen > 0 ? m_totalLen : 0);
				m_slider->setLimit(Rect(Point(0, getWidth()), Size(getWidth(), getHeight() - 2 * getWidth())));
				m_slider->setPos(Point(0, coord));
				m_slider->setSize(Size(getSize().x, sliderSize));

				m_upButton->setRect(Rect(0, 0, getWidth(), getWidth()));
				m_downButton->setRect(Rect(0, getHeight() - getWidth(), getWidth(), getWidth()));
			}

			void onUpButtonClicked()
			{
				if (m_curPos > 0)
				{
					int32_t offset = CraftEngine::math::min(m_stepLength, m_slider->getRect().top() - getWidth());
					if (offset > 0)
					{
						m_slider->move(Offset(0, -offset));
						sliderCallback();
					}
				}
			}
			void onDownButtonClicked()
			{
				if (m_curPos < m_totalLen)
				{
					int32_t offset = CraftEngine::math::min(m_stepLength, this->getRect().mHeight - m_slider->getRect().bottom() - getWidth());
					if (offset > 0)
					{
						m_slider->move(Offset(0, offset));
						sliderCallback();
					}
				}
			}
		public:

			craft_engine_gui_signal(scroll, void(float, float));

			VScrollBar(const Rect& rect, Widget* parent) :Widget(rect, parent)
			{
				m_slider = new Slider(this);
				m_upButton = new PushButton(this);
				m_downButton = new PushButton(this);
				craft_engine_gui_connect(m_upButton, clicked, this, onUpButtonClicked);
				craft_engine_gui_connect(m_downButton, clicked, this, onDownButtonClicked);
				m_slider->setSlideCallBack({ this, &VScrollBar::sliderCallback });
				setScrollBarInfo(rect.mHeight, rect.mHeight);
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eScorllBar));
				m_upButton->setIcon(GuiAssetsManager::loadImage("Triangle_Up"), getPalette().mForegroundColor);
				m_downButton->setIcon(GuiAssetsManager::loadImage("Triangle_Down"), getPalette().mForegroundColor);
			}

			VScrollBar(Widget* parent) :VScrollBar(Rect(0, 0, 20, 100), parent)
			{

			}

			~VScrollBar()
			{
				delete m_slider;
			}

			void setScrollPos(float pos)
			{
				m_curPos = CraftEngine::math::clamp(pos, 0.0f, m_totalLen);
				updateSlider();
			}

			void setScrollBarInfo(float frameLen, float totalLen)
			{
				m_frameLen = CraftEngine::math::clamp(frameLen, 0.0f, 1.0E+16f);
				m_totalLen = CraftEngine::math::clamp(totalLen, 0.0f, 1.0E+16f);
				m_curPos = CraftEngine::math::clamp(m_curPos, 0.0f, m_totalLen);
				updateSlider();
			}

			float getScrollBarCurrentPos()const { return m_curPos; }
			float getScrollBarFrameLen()const { return m_frameLen; }
			float getScrollBarTotalLen()const { return m_totalLen; }

		protected:

			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				updateSlider();
			}

			virtual void onPressed(const MouseEvent& mouseEvent)override
			{
				int32_t slider_size = getSilderSize();
				auto localPos = getLocalPos(mouseEvent.global);
				if (localPos.y < m_slider->getRect().top())
				{
					int32_t offset = CraftEngine::math::min(m_slider->getRect().top() - localPos.y + slider_size / 2, m_slider->getRect().top() - getWidth());
					if (offset > 0)
					{
						m_slider->move(Offset(0, -offset));
						sliderCallback();
					}
				}
				else if (localPos.y > m_slider->getRect().bottom())
				{
					int32_t offset = CraftEngine::math::min(localPos.y - m_slider->getRect().bottom() + slider_size / 2, this->getRect().mHeight - m_slider->getRect().bottom() - getWidth());
					if (offset > 0)
					{
						m_slider->move(Offset(0, offset));
						sliderCallback();
					}
				}
			}

			//virtual Widget* onMoveIn(const MouseEvent& mouseEvent)override { m_slider->setHighlight(true); return this; }
			//virtual void onMoveOut(const MouseEvent& mouseEvent)override { m_slider->setHighlight(false); }
			//virtual Widget* onFocusIn(const MouseEvent& mouseEvent)override { return this; }
			//virtual void onFocusOut(const MouseEvent& mouseEvent)override { }
		};



		class HScrollBar : public Widget
		{
		private:

			PushButton* m_upButton;
			PushButton* m_downButton;
			Slider* m_slider;
			float m_curPos = 0;
			float m_frameLen = 0;
			float m_totalLen = 0;
			int32_t m_stepLength = 20;

			uint32_t getSilderSize()
			{
				if (m_frameLen == 0)
					return getSize().x - 2 * getHeight();
				return m_frameLen == 0 ? getSize().x - 2 * getHeight() : CraftEngine::math::max((getSize().x - 2 * getHeight()) * m_frameLen / (m_totalLen + m_frameLen), 15.0f);
			}

			void sliderCallback()
			{
				auto coord = m_slider->getPos().x - getHeight();
				auto sliderSize = getSilderSize();
				auto total = getSize().x - sliderSize - 2 * getHeight();
				auto pos = coord * m_totalLen / (total > 0 ? total : 1);
				m_curPos = CraftEngine::math::clamp(pos, 0.0f, m_totalLen);
				craft_engine_gui_emit(scroll, m_curPos, m_totalLen);
			}

			void updateSlider()
			{
				auto sliderSize = getSilderSize();
				auto total = getSize().x - sliderSize - 2 * getHeight();
				auto coord = getHeight() + total * m_curPos / (m_totalLen > 0 ? m_totalLen : 0);
				m_slider->setLimit(Rect(Point(getHeight(), 0), Size(getWidth() - 2 * getHeight(), getHeight())));
				m_slider->setPos(Point(coord, 0));
				m_slider->setSize(Size(sliderSize, getSize().y));
				m_upButton->setRect(Rect(0, 0, getHeight(), getHeight()));
				m_downButton->setRect(Rect(getWidth() - getHeight(), 0, getHeight(), getHeight()));
			}

			void onUpButtonClicked()
			{
				if (m_curPos > 0)
				{
					int32_t offset = CraftEngine::math::min(m_stepLength, m_slider->getRect().left() - getHeight());
					if (offset > 0)
					{
						m_slider->move(Offset(-offset, 0));
						sliderCallback();
					}
				}
			}
			void onDownButtonClicked()
			{
				if (m_curPos < m_totalLen)
				{
					int32_t offset = CraftEngine::math::min(m_stepLength, this->getRect().mWidth - m_slider->getRect().right() - getHeight());
					if (offset > 0)
					{
						m_slider->move(Offset(offset, 0));
						sliderCallback();
					}
				}
			}

		public:

			craft_engine_gui_signal(scroll, void(float, float));

			HScrollBar(const Rect& rect, Widget* parent) :Widget(rect, parent)
			{
				m_slider = new Slider(this);
				m_slider->setSlideCallBack(core::Callback<void(void)>(this, &HScrollBar::sliderCallback));
				m_upButton = new PushButton(this);
				m_downButton = new PushButton(this);
				craft_engine_gui_connect(m_upButton, clicked, this, onUpButtonClicked);
				craft_engine_gui_connect(m_downButton, clicked, this, onDownButtonClicked);
				setScrollBarInfo(rect.mWidth, rect.mWidth);
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eScorllBar));
				m_upButton->setIcon(GuiAssetsManager::loadImage("Triangle_Left"), getPalette().mForegroundColor);
				m_downButton->setIcon(GuiAssetsManager::loadImage("Triangle_Right"), getPalette().mForegroundColor);
			}

			HScrollBar(Widget* parent) :HScrollBar(Rect(0, 0, 100, 20), parent)
			{

			}

			~HScrollBar()
			{
				delete m_slider;
			}

			void setScrollPos(float pos)
			{
				m_curPos = CraftEngine::math::clamp(pos, 0.0f, m_totalLen);
				updateSlider();
			}

			void setScrollBarInfo(float frameLen, float totalLen)
			{
				m_frameLen = CraftEngine::math::clamp(frameLen, 0.0f, 10000000.0f);
				m_totalLen = CraftEngine::math::clamp(totalLen, 0.0f, 10000000.0f);
				m_curPos = CraftEngine::math::clamp(m_curPos, 0.0f, m_totalLen);
				updateSlider();
			}

			float getScrollBarCurrentPos()const { return m_curPos; }
			float getScrollBarFrameLen()const { return m_frameLen; }
			float getScrollBarTotalLen()const { return m_totalLen; }

		protected:
			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				updateSlider();
			}

			virtual void onPressed(const MouseEvent& mouseEvent)override
			{
				int32_t slider_size = getSilderSize();
				auto localPos = getLocalPos(mouseEvent.global);

				if (localPos.x < m_slider->getRect().left())
				{
					int32_t offset = CraftEngine::math::min(m_slider->getRect().left() - localPos.x + slider_size / 2, m_slider->getRect().left() - getHeight());
					if (offset > 0)
					{
						m_slider->move(Offset(-offset, 0));
						sliderCallback();
					}
				}
				else if (localPos.x > m_slider->getRect().right())
				{
					int32_t offset = CraftEngine::math::min(localPos.x - m_slider->getRect().right() + slider_size / 2, this->getRect().mWidth - m_slider->getRect().right() - getHeight());
					if (offset > 0)
					{
						m_slider->move(Offset(offset, 0));
						sliderCallback();
					}
				}
			}
			//virtual Widget* onMoveIn(const MouseEvent& mouseEvent)override { m_slider->setHighlight(true); return this; }
			//virtual void onMoveOut(const MouseEvent& mouseEvent)override { m_slider->setHighlight(false); }
			//virtual Widget* onFocusIn(const MouseEvent& mouseEvent)override { return this; }
			//virtual void onFocusOut(const MouseEvent& mouseEvent)override { }
		};





	}
}

#endif // CRAFT_ENGINE_GUI_SCROLL_BAR_H_