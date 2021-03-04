#pragma once
#ifndef CRAFT_ENGINE_GUI_SLIDER_H_
#define CRAFT_ENGINE_GUI_SLIDER_H_
#include "./Widget.h"
namespace CraftEngine
{
	namespace gui
	{

		class Slider : public Widget
		{
		private:
			Rect m_limit;
			CraftEngine::core::Callback<void(void)> m_slideCallBack;
			Point m_srcPoint;
			Offset m_realOffset;
		public:
			craft_engine_gui_signal(slid, void(void));

			Slider(Widget* parent) :Widget(parent) 
			{
				setDragable(true);
				setLimit(Rect(-65535, -65535, 65535 * 2, 65535 * 2));
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eSlider));
			}
			void setLimit(const Rect& rect) { m_limit = rect; }
			void setSlideCallBack(const CraftEngine::core::Callback<void(void)> callback) { m_slideCallBack = callback; }
			virtual void onPaintEvent() override
			{
				auto painter = getPainter();
				drawBackground();
				if (isFocus())
					painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundFocusColor);
				else if (isHighlight())
					painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundHighLightColor);
				drawFrame();
			}
		protected:

			virtual void onDrag(const MouseEvent& mouseEvent) override
			{
				auto delta = mouseEvent.offset - getDragOffset();
				auto yoffset = delta.y;
				auto yoffsetMin = m_limit.top() - this->getRect().top();
				auto yoffsetMax = m_limit.bottom() - this->getRect().bottom();
				if (yoffset > yoffsetMax)yoffset = yoffsetMax;
				if (yoffset < yoffsetMin)yoffset = yoffsetMin;
				auto xoffset = delta.x;
				auto xoffsetMin = m_limit.left() - this->getRect().left();
				auto xoffsetMax = m_limit.right() - this->getRect().right();
				if (xoffset > xoffsetMax)xoffset = xoffsetMax;
				if (xoffset < xoffsetMin)xoffset = xoffsetMin;
				m_realOffset = mouseEvent.offset;
				MouseEvent subEvent = mouseEvent;
				subEvent.offset = Offset(xoffset, yoffset) + getDragOffset();
				Widget::onDrag(subEvent);
				m_slideCallBack.call();
				craft_engine_gui_emit(slid, );
			}

			virtual void onDragApply(const MouseEvent& mouseEvent) override
			{
				Widget::onDragApply(mouseEvent);
			}

			virtual void onPressed(const MouseEvent& mouseEvent)override
			{
				m_srcPoint = getPos();
			}
		public:
			void _Update_Drag_Offset()
			{
				this->setDragOffset(this->getPos() - this->m_srcPoint);
			}
			void _Update_Src_point()
			{
				this->m_srcPoint = this->getPos();
			}
		};



		template<typename Type>
		class TemplateHorizontalSlider : public Widget
		{
		private:

			Slider* m_slider;
			Type  m_minValue = 0;
			Type  m_maxValue = 0;
			Type  m_curValue = 0;
			int   m_sliderSize = 12;
			bool  m_inverse = false;
			uint32_t getSilderSize()const { return m_sliderSize; }

			void sliderCallback()
			{
				auto slider_size = getSilderSize();
				auto coord = m_slider->getPos().x;
				const auto total = getSize().x - slider_size;
				if (m_inverse)
					coord = total - coord;
				auto pos = double(coord) / double(total) * (m_maxValue - m_minValue) + m_minValue;
				if (std::is_same<Type, int>::value)
					pos = round(pos);
				pos = CraftEngine::math::clamp((Type)pos, m_minValue, m_maxValue);
				bool value_changed = m_curValue != pos;
				m_curValue = pos;
				coord = total * double(m_curValue - m_minValue) / double(m_maxValue - m_minValue);
				if (m_inverse)
					coord = total - coord;
				m_slider->setPos(Point(coord, 0));
				m_slider->_Update_Drag_Offset();
				if (value_changed)
					craft_engine_gui_emit(slid, m_curValue);
			}

			void updateSlider()
			{
				auto slider_size = getSilderSize();
				auto total = getSize().x - slider_size;
				auto coord = total * double(m_curValue - m_minValue) / double(m_maxValue - m_minValue);
				if (m_inverse)
					coord = total - coord;
				m_slider->setLimit(Rect(Point(0, 0), getSize()));
				m_slider->setPos(Point(coord, 0));
				m_slider->setSize(Size(slider_size, getSize().y));
			}
		public:

			craft_engine_gui_signal(slid, void(Type));

			TemplateHorizontalSlider(const Rect& rect, Widget* parent) :Widget(rect, parent)
			{
				m_slider = new Slider(this);
				m_slider->setSlideCallBack({ this, &TemplateHorizontalSlider::sliderCallback });
				setValue(0, 0, 10);
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eSliderBar));
			}

			TemplateHorizontalSlider(Widget* parent) :TemplateHorizontalSlider(Rect(0, 0, 120, 22), parent)
			{

			}

			Slider* getSlider()
			{
				return m_slider;
			}

			void setSize(const Size& size)
			{
				Widget::setSize(size);
				updateSlider();
			}

			Type getValue()const
			{
				return m_curValue;
			}


			void setInverse(bool enable)
			{
				m_inverse = enable;
				updateSlider();
			}
			/*
			 Note: No signal emitted.
			*/
			void setValue(Type value)
			{
				m_curValue = CraftEngine::math::clamp((Type)value, m_minValue, m_maxValue);
				updateSlider();
			}
			/*
			 Note: No signal emitted.
			*/
			void setValue(Type value, Type min_val, Type max_val)
			{
				if (min_val < max_val) {
					m_inverse = false;
					m_minValue = min_val;
					m_maxValue = max_val;
				}
				else {
					m_inverse = true;
					m_minValue = max_val;
					m_maxValue = min_val;
				}
				m_curValue = CraftEngine::math::clamp((Type)value, m_minValue, m_maxValue);
				updateSlider();
			}

		protected:

			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override { updateSlider(); }

			virtual void onPaintEvent() override
			{
				drawBackground();
				drawChild(m_slider);
				drawFrame();
			}

			virtual void onPressed(const MouseEvent& mouseEvent)override
			{
				int32_t slider_size = getSilderSize();
				auto localPos = getLocalPos(mouseEvent.global);

				if (localPos.x < m_slider->getRect().left())
				{
					int32_t offset = CraftEngine::math::min(m_slider->getRect().left() - localPos.x + slider_size / 2, m_slider->getRect().left());
					m_slider->move(Offset(-offset, 0));
					if (offset != 0)
					{
						m_slider->_Update_Src_point();
						sliderCallback();
					}
				}
				else if (localPos.x > m_slider->getRect().right())
				{
					int32_t offset = CraftEngine::math::min(localPos.x - m_slider->getRect().right() + slider_size / 2, this->getRect().right() - m_slider->getRect().right());
					m_slider->move(Offset(offset, 0));
					if (offset != 0)
					{
						m_slider->_Update_Src_point();
						sliderCallback();
					}
				}
			}

		};





		template<typename Type>
		class TemplateVerticalSlider : public Widget
		{
		private:
			Slider* m_slider;
			Type  m_minValue = 0;
			Type  m_maxValue = 0;
			Type  m_curValue = 0;
			int   m_sliderSize = 12;
			bool  m_inverse = false;
			uint32_t getSilderSize()const { return m_sliderSize; }

			void sliderCallback()
			{
				auto slider_size = getSilderSize();
				auto coord = m_slider->getPos().y;
				const auto total = getSize().y - slider_size;
				if (m_inverse)
					coord = total - coord;
				auto pos = double(coord) / double(total) * (m_maxValue - m_minValue) + m_minValue;
				if (std::is_same<Type, int>::value)
					pos = round(pos);
				pos = CraftEngine::math::clamp((Type)pos, m_minValue, m_maxValue);
				bool value_changed = m_curValue != pos;
				m_curValue = pos;
				coord = total * double(m_curValue - m_minValue) / double(m_maxValue - m_minValue);
				if (m_inverse)
					coord = total - coord;
				m_slider->setPos(Point(0, coord));
				m_slider->_Update_Drag_Offset();
				if (value_changed)
					craft_engine_gui_emit(slid, m_curValue);
			}

			void updateSlider()
			{
				auto slider_size = getSilderSize();
				auto total = getSize().y - slider_size;
				auto coord = total * double(m_curValue - m_minValue) / double(m_maxValue - m_minValue);
				if (m_inverse)
					coord = total - coord;
				m_slider->setLimit(Rect(Point(0, 0), getSize()));
				m_slider->setPos(Point(0, coord));
				m_slider->setSize(Size(getSize().x, slider_size));
			}
		public:

			craft_engine_gui_signal(slid, void(Type));

			TemplateVerticalSlider(const Rect& rect, Widget* parent) :Widget(rect, parent)
			{
				m_slider = new Slider(this);
				m_slider->setSlideCallBack({ this, &TemplateVerticalSlider::sliderCallback });
				setValue(0, 0, 10);
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eSliderBar));
			}

			TemplateVerticalSlider(Widget* parent) :TemplateVerticalSlider(Rect(0, 0, 22, 120), parent)
			{

			}

			Slider* getSlider()
			{
				return m_slider;
			}

			void setSize(const Size& size)
			{
				Widget::setSize(size);
				updateSlider();
			}

			Type getValue()const
			{
				return m_curValue;
			}

			void setInverse(bool enable)
			{
				m_inverse = enable;
				updateSlider();
			}
			void setValue(Type value)
			{
				m_curValue = CraftEngine::math::clamp((Type)value, m_minValue, m_maxValue);
				updateSlider();
			}
			void setValue(Type value, Type min_val, Type max_val)
			{
				if (min_val < max_val){
					m_inverse = false;
					m_minValue = min_val;
					m_maxValue = max_val;
				}else{
					m_inverse = true;
					m_minValue = max_val;
					m_maxValue = min_val;
				}
				m_curValue = CraftEngine::math::clamp((Type)value, m_minValue, m_maxValue);
				updateSlider();
			}
		protected:
			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override { updateSlider(); }


			virtual void onPaintEvent() override
			{
				drawBackground();
				drawChild(m_slider);
				drawFrame();
			}

			virtual void onPressed(const MouseEvent& mouseEvent)override
			{
				int32_t slider_size = getSilderSize();
				auto localPos = getLocalPos(mouseEvent.global);
				if (localPos.y < m_slider->getRect().top())
				{
					int32_t offset = CraftEngine::math::min(m_slider->getRect().top() - localPos.y + slider_size / 2, m_slider->getRect().top());
					m_slider->move(Offset(0, -offset));
					if (offset != 0)
					{
						m_slider->_Update_Src_point();
						sliderCallback();
					}
				}
				else if (localPos.y > m_slider->getRect().bottom())
				{
					int32_t offset = CraftEngine::math::min(localPos.y - m_slider->getRect().bottom() + slider_size / 2, this->getRect().bottom() - m_slider->getRect().bottom());
					m_slider->move(Offset(0, offset));
					if (offset != 0)
					{
						m_slider->_Update_Src_point();
						sliderCallback();
					}
				}
			}

		};




		/*
		 Horizontal slider for integer
		*/
		typedef TemplateHorizontalSlider<int32_t>  HSliderInt;
		/*
		 Horizontal slider for float
		*/
		typedef TemplateHorizontalSlider<float_t>  HSliderFloat;
		/*
		 Vertical slider for integer
		*/
		typedef TemplateVerticalSlider<int32_t> VSliderInt;
		/*
		 Vertical slider for float
		*/
		typedef TemplateVerticalSlider<float_t> VSliderFloat;
		
	}
}

#endif // CRAFT_ENGINE_GUI_SLIDER_H_