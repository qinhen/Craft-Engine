#pragma once
#ifndef CRAFT_ENGINE_GUI_PROGRESS_BAR_H_
#define CRAFT_ENGINE_GUI_PROGRESS_BAR_H_
#include "./Widget.h"
namespace CraftEngine
{
	namespace gui
	{



		class ProgressBar : public Widget
		{
		private:
			
			float m_curProgress;
			String m_progressText;
			Point m_basepoint;

			Rect m_progressRect;

			void _On_Progress_Changed()
			{
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eProgressBar));
				m_progressText = StringTool::fromStdWString(StringTool::fromValue(int(m_curProgress * 100)) + L"%");
				m_basepoint = GuiFontSystem::calcFontBasePoint(m_progressText, getRect(), getFont());
				m_progressRect = getRect();
				m_progressRect.mWidth = m_progressRect.mWidth * m_curProgress;
			}

		public:


			void setProgress(float value)
			{
				m_curProgress = CraftEngine::math::clamp(value, 0.0f, 1.0f);
				_On_Progress_Changed();
			}

			ProgressBar(Widget* parent) : Widget(parent)
			{
				setProgress(0);
			}

			virtual void onPaintEvent() override
			{
				auto painter = getPainter();
				drawBackground();
				painter.drawRect(m_progressRect, getPalette().mBackgroundHighLightColor);
				painter.drawTextLine(m_progressText, m_basepoint, getFont(), getPalette().mForegroundColor);
				drawFrame();
			}

		protected:

			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				_On_Progress_Changed();
			}

		};





	}
}

#endif // CRAFT_ENGINE_GUI_PROGRESS_BAR_H_