#pragma once
#ifndef CRAFT_ENGINE_GUI_IMAGE_VIEW_H_
#define CRAFT_ENGINE_GUI_IMAGE_VIEW_H_
#include "./Widget.h"

namespace CraftEngine
{
	namespace gui
	{


		class ImageView :public Widget
		{
		private:
			HandleImage m_image = HandleImage(nullptr);
		public:

			ImageView(const Rect& rect, Widget* p) :Widget(rect, p)
			{
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eLabel));
			}

			ImageView(Widget* p) :ImageView(Rect(0, 0, 80, 80), p)
			{

			}

			void setImage(HandleImage image)
			{
				m_image = image;
			}

			virtual void onPaintEvent() override
			{
				auto painter = getPainter();
				drawBackground();
				if (m_image != nullptr)
					painter.drawRect(Rect(Point(0), getSize()), m_image);
				drawFrame();
			}

		};


	}
}

#endif // CRAFT_ENGINE_IMAGE_VIEW_H_