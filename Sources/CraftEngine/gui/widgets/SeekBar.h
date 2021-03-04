#pragma once
#ifndef CRAFT_ENGINE_GUI_SEEK_BAR_H_
#define CRAFT_ENGINE_GUI_SEEK_BAR_H_
#include "./Slider.h"
namespace CraftEngine
{
	namespace gui
	{


		class HSeekBar : public Widget
		{
		private:
			Slider* m_slider;
			double  m_curPercentage = 0;

			void sliderCallback()
			{
				auto slider_size = getSilderSize();
				auto coord = m_slider->getPos().x;
				auto total = getSize().x - slider_size;
				auto pos = double(coord) / double(total);
				m_curPercentage = CraftEngine::math::clamp(pos, 0.0, 1.0);
				craft_engine_gui_emit(slid, m_curPercentage);
			}

			void updateSlider()
			{
				auto slider_size = getSilderSize();
				auto total = getSize().x - slider_size;
				auto coord = total * m_curPercentage;
				m_slider->setLimit(Rect(Point(0, 0), getSize()));
				m_slider->setPos(Point(coord, 0));
				m_slider->setSize(Size(slider_size, getSize().y));
			}
		public:

			craft_engine_gui_signal(slid, void(double));

			HSeekBar(const Rect& rect, Widget* parent) :Widget(rect, parent)
			{
				m_slider = new Slider(this);
				m_slider->setSlideCallBack({ this, &HSeekBar::sliderCallback });
				setPercentage(0);
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eSeekBar));
			}

			HSeekBar(Widget* parent) :HSeekBar(Rect(0, 0, 100, 20), parent)
			{

			}

			template<class SliderType>
			HSeekBar(const Rect& rect, Widget* parent) :Widget(rect, parent)
			{
				m_slider = new SliderType(this);
				m_slider->setDragable(true);
				m_slider->getPalette().mFillMode = Palette::FillMode::eFillMode_Color;
				//m_slider->getPalette().mBackgroundColor.a = 200;

				m_slider->setSlideCallBack({ this, &HSeekBar::sliderCallback });
				this->getPalette().mBackgroundFocusColor = this->getPalette().mBackgroundHighLightColor;
				this->getPalette().mFillMode = Palette::FillMode::eFillMode_Color;
				this->getPalette().mBackgroundColor.a = 150;
				setPercentage(0);
			}

			void setSize(const Size& size)
			{
				Widget::setSize(size);
				updateSlider();
			}

			double getPercentage()const
			{
				return m_curPercentage;
			}

			void setPercentage(double percentage)
			{
				m_curPercentage = CraftEngine::math::clamp(percentage, 0.0, 1.0);
				updateSlider();
			}

		protected:

			virtual uint32_t getSilderSize()const { return 10; }

			virtual void onPressed(const MouseEvent& mouseEvent)override
			{
				int32_t slider_size = getSilderSize();
				auto localPos = getLocalPos(mouseEvent.global);

				if (localPos.x < m_slider->getRect().left())
				{
					int32_t offset = CraftEngine::math::min(m_slider->getRect().left() - localPos.x + slider_size / 2, m_slider->getRect().left());
					m_slider->move(Offset(-offset, 0));
					if (offset != 0)
						sliderCallback();
				}
				else if (localPos.x > m_slider->getRect().right())
				{
					int32_t offset = CraftEngine::math::min(localPos.x - m_slider->getRect().right() + slider_size / 2, this->getRect().right() - m_slider->getRect().right());
					m_slider->move(Offset(offset, 0));
					if (offset != 0)
						sliderCallback();
				}
			}

			virtual void onPaintEvent() override
			{
				auto painter = getPainter();
				drawBackground();
				//if (isFocus())
				//	painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundFocusColor);
				//else if (isHighlight())
				//	painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundHighLightColor);

				int32_t slider_size = getSilderSize();
				auto track_global_rect = Rect(Point(0), getSize());
				track_global_rect.mHeight = 4;
				track_global_rect.mWidth -= slider_size;
				track_global_rect.mX += slider_size / 2;
				track_global_rect.mY += getSize().y / 2 - track_global_rect.mHeight / 2;
				painter.drawRect(track_global_rect, getPalette().mBackgroundFocusColor);

				drawChild(m_slider);
				drawFrame();
			}
			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override { updateSlider(); }
		};





		class VSeekBar : public Widget
		{
		private:
			Slider* m_slider;
			double   m_curPercentage = 0;

			uint32_t getSilderSize()const { return 10; }

			void sliderCallback()
			{
				auto slider_size = getSilderSize();
				auto coord = m_slider->getPos().y;
				auto total = getSize().y - slider_size;
				auto pos = double(coord) / double(total);
				m_curPercentage = CraftEngine::math::clamp(pos, 0.0, 1.0);
				craft_engine_gui_emit(slid, m_curPercentage);
			}

			void updateSlider()
			{
				auto slider_size = getSilderSize();
				auto total = getSize().y - slider_size;
				auto coord = total * m_curPercentage;
				m_slider->setLimit(Rect(Point(0, 0), getSize()));
				m_slider->setPos(Point(0, coord));
				m_slider->setSize(Size(getSize().x, slider_size));
			}
		public:

			craft_engine_gui_signal(slid, void(float));

			VSeekBar(const Rect& rect, Widget* parent) :Widget(rect, parent)
			{
				m_slider = new Slider(this);
				m_slider->setSlideCallBack({ this, &VSeekBar::sliderCallback });
				setPercentage(0);
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eSeekBar));
			}

			VSeekBar(Widget* parent) :VSeekBar(Rect(0, 0, 20, 100), parent)
			{

			}

			template<class SliderType>
			VSeekBar(const Rect& rect, Widget* parent) :Widget(rect, parent)
			{
				m_slider = new SliderType(this);
				m_slider->setSlideCallBack({ this, &VSeekBar::sliderCallback });
				this->getPalette().mBackgroundFocusColor = this->getPalette().mBackgroundHighLightColor;
				this->getPalette().mFillMode = Palette::FillMode::eFillMode_Color;
				this->getPalette().mBackgroundColor.a = 150;
				setPercentage(0);
			}

			void setSize(const Size& size)
			{
				Widget::setSize(size);
				updateSlider();
			}

			double getPercentage()const
			{
				return m_curPercentage;
			}

			void setPercentage(double percentage)
			{
				m_curPercentage = CraftEngine::math::clamp(percentage, 0.0, 1.0);
				updateSlider();
			}

		protected:
			virtual void onPressed(const MouseEvent& mouseEvent)override
			{
				int32_t slider_size = getSilderSize();
				auto localPos = getLocalPos(mouseEvent.global);
				if (localPos.y < m_slider->getRect().top())
				{
					int32_t offset = CraftEngine::math::min(m_slider->getRect().top() - localPos.y + slider_size / 2, m_slider->getRect().top());
					m_slider->move(Offset(0, -offset));
					if (offset != 0)
						sliderCallback();
				}
				else if (localPos.y > m_slider->getRect().bottom())
				{
					int32_t offset = CraftEngine::math::min(localPos.y - m_slider->getRect().bottom() + slider_size / 2, this->getRect().bottom() - m_slider->getRect().bottom());
					m_slider->move(Offset(0, offset));
					if (offset != 0)
						sliderCallback();
				}
			}

			virtual void onPaintEvent() override
			{
				auto painter = getPainter();
				drawBackground();
				//if (isFocus())
				//	painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundFocusColor);
				//else if (isHighlight())
				//	painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundHighLightColor);

				int32_t slider_size = getSilderSize();
				auto track_global_rect = Rect(Point(0), getSize());
				track_global_rect.mWidth = 4;
				track_global_rect.mHeight -= slider_size;
				track_global_rect.mY += slider_size / 2;
				track_global_rect.mX += getSize().x / 2 - track_global_rect.mWidth / 2;
				painter.drawRect(track_global_rect, getPalette().mBackgroundFocusColor);

				drawChild(m_slider);
				drawFrame();
			}
			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override { updateSlider(); }
		};



	}
}

#endif // CRAFT_ENGINE_GUI_SLIDER_H_