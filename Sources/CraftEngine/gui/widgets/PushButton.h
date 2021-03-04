#pragma once
#ifndef CRAFT_ENGINE_GUI_PUSH_BUTTON_H_
#define CRAFT_ENGINE_GUI_PUSH_BUTTON_H_
#include "./Label.h"

namespace CraftEngine
{
	namespace gui
	{


		class PushButton :public Label
		{
		public:
			craft_engine_gui_signal(clicked, void(void));

			PushButton(const String& text, const Rect& rect, Widget* p) :Label(String(), rect, p)
			{
				setClickable(true);
				getFont().getAlignment().mHorizonMode = Alignment::eAlignmentMode_Center;
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::ePushButton));
				setText(text);
			}

			PushButton(Widget* p) :PushButton(String(), Rect(0, 0, 150, 25), p) {}
			PushButton(const Rect& rect, Widget* p) :PushButton(String(), rect, p) {}
			PushButton(const String& text, Widget* p) :PushButton(text, Rect(0, 0, 150, 25), p) {}

			virtual void onPaintEvent() override
			{
				auto painter = getPainter();
				drawBackground();
				if (isFocus())
					painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundFocusColor);
				else if (isHighlight())
					painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundHighLightColor);
				Label::onPaint_drawIcon();
				Label::onPaint_drawText();
				drawFrame();
			}
		protected:
			virtual void onClicked(const MouseEvent& mouseEvent)override
			{
				if (isClickable())
					craft_engine_gui_emit(clicked);
			}

		};


	}
}

#endif // CRAFT_ENGINE_GUI_PUSH_BUTTON_H_