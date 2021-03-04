#pragma once
#ifndef CRAFT_ENGINE_GUI_TEXTVIEW_H_
#define CRAFT_ENGINE_GUI_TEXTVIEW_H_
#include "./Widget.h"

namespace CraftEngine
{
	namespace gui
	{


		class TextView :public Widget
		{
		private:
			String m_text;
			Size m_textSize;
			Point m_basepoint;
		public:

			TextView(const String& text, const Rect& rect, Widget* p) :Widget(rect, p)
			{
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eLabel));
				setText(text);
			}
			TextView(const String& text, Widget* p) :TextView(text, Rect(0, 0, 80, 22), p)
			{

			}
			TextView(Widget* p) :TextView(String(), Rect(0, 0, 80, 22), p)
			{

			}

			const String& getText()const { return m_text; }

			void setText(const String& str)
			{
				m_text = str;
				_Adjust_Label_Layout();
			}

			virtual Size getMinSizeLimit()const
			{
				return m_textSize;
			}

			virtual void onPaintEvent() override
			{
				Widget::onPaintEvent();
				auto painter = getPainter();
				painter.drawTextLine(m_text, m_basepoint, getFont(), getPalette().mForegroundColor);
			}

		protected:

			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override { _Adjust_Label_Layout(); }

		private:

			void _Adjust_Label_Layout()
			{
				m_textSize = Size(GuiFontSystem::calcFontLineWidth(m_text, getFont()) + getFont().getAlignment().mHorizonOffset, getFont().getSize());
				m_basepoint = GuiFontSystem::calcFontBasePoint(m_text, { Point(0, 0), getSize() }, getFont());
			}
		};


	}
}

#endif // CRAFT_ENGINE_GUI_TEXTVIEW_H_