#pragma once
#ifndef CRAFT_ENGINE_GUI_LABEL_H_
#define CRAFT_ENGINE_GUI_LABEL_H_
#include "./Widget.h"

namespace CraftEngine
{
	namespace gui
	{


		class Label :public Widget
		{
		private:
			String m_text;
			Size m_textSize;
			HandleImage m_icon = HandleImage(nullptr);
			Color m_iconColor;
			Rect m_iconRect;
			Point m_basepoint;
		public:

			Label(const String& text, const Rect& rect, Widget* p) :Widget(rect, p)
			{
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eLabel));
				setText(text);
			}

			Label(const String& text, Widget* p) :Label(text, Rect(0, 0, 80, 22), p)
			{

			}

			Label(Widget* p) :Label(String(), Rect(0, 0, 80, 22), p)
			{

			}

			const String& getText()const { return m_text; }

			void setText(const String& str)
			{
				m_text = str;
				m_textSize = Size(GuiFontSystem::calcFontLineWidth(str, getFont()) + getFont().getAlignment().mHorizonOffset, getFont().getSize());
				_Adjust_Label_Layout();
			}

			void setIcon(HandleImage icon, const Color iconColor)
			{
				m_icon = icon;
				m_iconColor = iconColor;
				_Adjust_Label_Layout();
			}

			void setIcon(HandleImage icon)
			{
				m_icon = icon;
				m_iconColor = getPalette().mForegroundColor;
				_Adjust_Label_Layout();
			}

			virtual Size getMinSizeLimit()const
			{
				return m_textSize;
			}

			virtual void onPaintEvent() override
			{
				drawBackground();
				if (m_icon != nullptr)
					onPaint_drawIcon();
				if (m_text.size() != 0)
					onPaint_drawText();
				drawFrame();
			}
		protected:
			void onPaint_drawIcon()
			{
				auto painter = getPainter();
				if (m_icon != nullptr)
				{
					auto info = GuiRenderSystem::getImageInfo(m_icon);
					auto this_global_rect = Rect(m_iconRect.mOffset, m_iconRect.mSize);
					if (isFocus())
					{
						Color inverse_color = Color(255) - m_iconColor;
						inverse_color.a = m_iconColor.a;
						if (info.mChannels == 4) painter.drawRect(this_global_rect, m_icon);
						else painter.drawSDF(this_global_rect, inverse_color, m_icon);
					}
					else
					{
						if (info.mChannels == 4) painter.drawRect(this_global_rect, m_icon);
						else painter.drawSDF(this_global_rect, m_iconColor, m_icon);
					}
				}
			}

			void onPaint_drawText()
			{
				auto painter = getPainter();
				painter.drawTextLine(m_text, m_basepoint, getFont(), getPalette().mForegroundColor);
			}
		protected:

			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override { Widget::onResizeEvent(resizeEvent); _Adjust_Label_Layout(); }

		private:

			void _Adjust_Label_Layout()
			{
				if (m_icon == nullptr)
					m_basepoint = GuiFontSystem::calcFontBasePoint(m_text, { Point(0, 0), getSize() }, getFont());
				else if (m_text.size() == 0)
				{
					if (getWidth() >= getHeight())
						m_iconRect = Rect((getWidth() - getHeight()) >> 1, 0, getHeight(), getHeight());
					else
						m_iconRect = Rect(0, (getHeight() - getWidth()) >> 1, getWidth(), getWidth());
				}
				else if (m_icon != nullptr)
				{
					if (getWidth() >= getHeight())
					{
						m_iconRect = Rect(0, 0, getHeight(), getHeight());
						Rect text_rect(getHeight(), 0, getWidth() - getHeight(), getHeight());
						m_basepoint = GuiFontSystem::calcFontBasePoint(m_text, text_rect, getFont());
					}
					else
					{
						m_iconRect = Rect(0, 0, getWidth(), getWidth());
						Rect text_rect(getWidth(), 0, getHeight() - getWidth(), getWidth());
						m_basepoint = GuiFontSystem::calcFontBasePoint(m_text, text_rect, getFont());
					}
				}
			}
		};


	}
}

#endif // CRAFT_ENGINE_GUI_LABEL_H_