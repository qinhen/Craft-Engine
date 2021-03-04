#pragma once
#include "./Widget.h"

namespace CraftEngine
{
	namespace gui
	{

		class RichTextLine :public Widget
		{
		protected:
		
			String    m_text;
			StringRef m_highlightText;
			String    m_placeholderText;

			Point   m_basepoint;
			Point   m_placeholderTextBasePont;
			Point   m_highlightBasePont;

			Rect    m_highlightRect;
			int32_t m_hightlightStartIndex;
			int32_t m_hightlightEndIndex;

			int32_t m_pointerCharIndex;
			Rect    m_cursorRect;
			Timer   m_timer;

			RichTextFormat m_format;
			bool m_drawCursor = false;
			bool m_editable = true;
		private:
			void _Adjust_Text()
			{
				m_basepoint = GuiFontSystem::calcFontBasePoint(getText(), getRect(), getFont());
				_Adjust_Cursor();
			}

			void _Adjust_Placeholder_Text()
			{
				m_placeholderTextBasePont = GuiFontSystem::calcFontBasePoint(m_placeholderText, { Point(0, 0), getSize() }, getFont());
			}

			void _Adjust_Cursor()
			{
				StringRef str = StringRef(m_text, m_pointerCharIndex, 0);
				m_cursorRect = GuiFontSystem::calcFontCursorRect(str, m_basepoint, getFont());
			}

			void setCursor(uint32_t index)
			{
				m_pointerCharIndex = index;
				_Adjust_Cursor();
			}
		public:

			craft_engine_gui_signal(textChanged, void(const String&));
			craft_engine_gui_signal(editFinished, void(const String&));
			craft_engine_gui_signal(enterPressed, void(const String&));
			craft_engine_gui_signal(editReady, void());


			RichTextLine(const Rect& rect, Widget* p) :Widget(rect, p)
			{
				// getFont().getAlignment().mHorizonMode = Alignment::AlignmentMode::eAlignmentMode_Center;
				setCharAcceptable(true);
				setClickable(true);
				setDragable(true);
				setCursorType(AbstractCursor::SystemCursorType::eEditCursor);
				//setText(String::fromStdWString(L"ÇëÊäÈë:"));
				m_pointerCharIndex = 0;
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eTextInput));
				_Adjust_Text();
			}

			RichTextLine(Widget* p) :RichTextLine(Rect(0, 0, 150, 22), p)
			{

			}

			void setEditable(bool enable) { m_editable = enable; }



			void setPlaceholderText(const String& str)
			{
				m_placeholderText = str;
				_Adjust_Placeholder_Text();
			}

			/*
			 Note: Emit signal [textChanged(const String& text)]
			*/
			void setText(const String& text)
			{
				m_text = text;
				_Adjust_Text();
				setCursor(m_text.size());
				craft_engine_gui_emit(textChanged, getText());
			}
			const String& getText()const { return m_text; }
			const RichTextFormat& getTextFormat() const { return m_format; }
			RichTextFormat& getTextFormat() { return m_format; }
			void setTextFormat(const RichTextFormat& format) { m_format = format; }


			virtual void onPaintEvent() override
			{
				auto painter = getPainter();
				drawBackground();
				if (m_editable)
				{
					if (m_text.size() == 0)
						painter.drawTextLine(m_placeholderText, m_placeholderTextBasePont, getFont(), getPalette().mForegroundDisableColor);
					else
						painter.drawRichTextLine(m_text, m_basepoint, getFont(), m_format);
					if (isInputFocus())
					{
						if (m_highlightText.size() != 0)
						{
							painter.drawRect(m_highlightRect, getPalette().mBackgroundHighLightColor);
							auto hFormat = m_format;
							hFormat.setColor(getPalette().mForegroundHighLightColor);
							hFormat.setEdgeColor(getPalette().mForegroundHighLightColor);
							painter.drawRichTextLine(m_highlightText.data(), m_highlightText.size(), m_highlightBasePont, getFont(), hFormat);
							//renderer->drawTextLine(m_highlightText.data(), m_highlightText.size(), hightlight_basepoint, getFont(), getPalette().mForegroundHighLightColor, child_global_limit_rect);
						}
						if (!m_timer.isValid())
						{
							m_timer.setDuration(500);
							m_timer.setCallback(CraftEngine::core::Callback<void(float, float)>([=](float, float) { m_drawCursor = !m_drawCursor; sendRepaintEvent(); })); // 
							auto main_window = this->getRootWidget();
							if (main_window != nullptr)
								m_timer.startTimer(main_window->getSyncTimerSystem());
						}
						if (m_drawCursor)
						{
							painter.drawRect(m_cursorRect, getPalette().mForegroundColor);
						}
					}
				}
				else
				{
					painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundDisableColor);
					painter.drawRichTextLine(m_text, m_basepoint, getFont(), m_format);
				}
				drawFrame();
			}


			void selectText(const Point start_point, const Point end_point)
			{
				auto start_index = GuiFontSystem::calcFontPointerIndex(m_text, start_point.x, getFont());
				auto end_index = GuiFontSystem::calcFontPointerIndex(m_text, end_point.x, getFont());
				auto min_index = CraftEngine::math::min(start_index, end_index);
				auto max_index = CraftEngine::math::max(start_index, end_index);
				m_highlightText = StringRef(m_text, max_index - min_index, min_index);
				auto woffs = GuiFontSystem::calcFontLineWidth(StringRef(m_text, min_index, 0), getFont());
				auto w = GuiFontSystem::calcFontLineWidth(m_highlightText, getFont());
				m_highlightBasePont.x = m_basepoint.x + woffs;
				m_highlightBasePont.y = m_basepoint.y;
				m_highlightRect.mX = m_highlightBasePont.x;
				m_highlightRect.mY = m_cursorRect.mY;
				m_highlightRect.mWidth = w;
				m_highlightRect.mHeight = m_cursorRect.mHeight;
				setCursor(end_index);
				m_hightlightStartIndex = min_index;
				m_hightlightEndIndex = max_index;
			}

			void selectAllText()
			{
				selectText(Point(-65535), Point(65535)); // 
			}

		protected:

			virtual void onAcceptChar(wchar_t c)override
			{
				if (!m_editable)
					return;
				bool text_change = false;
				if (m_highlightText.size() != 0)
				{
					m_text.erase(m_hightlightStartIndex, m_hightlightEndIndex - m_hightlightStartIndex);
					m_pointerCharIndex = m_hightlightStartIndex;
					text_change = true;
				}
				if (c == L'\b')
				{
					if (m_pointerCharIndex > 0 && m_highlightText.size() == 0)
					{
						m_text.erase(--m_pointerCharIndex, 1);
						text_change = true;
					}
				}
				else if (c == L'\r')
				{
					craft_engine_gui_emit(enterPressed, getText());
				}
				else if (!isascii(c) || ispunct(c) || isalpha(c) || isdigit(c) || c == L'\x20')
				{
					m_text.insert(m_pointerCharIndex++, 1, c);
					text_change = true;
				}
				m_highlightText = StringRef();
				m_hightlightStartIndex = 0;
				m_hightlightEndIndex = 0;
				_Adjust_Text();
				if (text_change)
					craft_engine_gui_emit(textChanged, getText());
			}

			virtual void onPressed(const MouseEvent& mouseEvent)
			{
				if (!m_editable)
					return;
				m_highlightText = StringRef();
				m_hightlightStartIndex = 0;
				m_hightlightEndIndex = 0;
				auto subEvent = mouseEvent;
				subEvent.type = MouseEvent::eReleased;
				onReleased(subEvent);
			}

			virtual void onReleased(const MouseEvent& mouseEvent)
			{
				if (!m_editable)
					return;
				auto localPos = getLocalPos(mouseEvent.global);
				int x = localPos.x - m_basepoint.x;
				int index = GuiFontSystem::calcFontPointerIndex(m_text, x, getFont());
				setCursor(index);
			}

			virtual void onDrag(const MouseEvent& mouseEvent)
			{
				if (!m_editable)
					return;
				auto local_point = getLocalPos(mouseEvent.global);
				auto start_point = local_point - m_basepoint;
				auto end_point = start_point + mouseEvent.offset;
				selectText(start_point, end_point);
			}

			virtual void onKeyBoardEvent(KeyboardEvent keyboardEvent)
			{
				if (!m_editable)
					return;
				if (keyboardEvent.down)
				{
					if (keyboardEvent.key == (KeyBoard::eKey_Left) && m_pointerCharIndex > 0)
					{
						setCursor(--m_pointerCharIndex);
						m_drawCursor = true;
					}
					else if (keyboardEvent.key == (KeyBoard::eKey_Right) && m_pointerCharIndex < m_text.size())
					{
						setCursor(++m_pointerCharIndex);
						m_drawCursor = true;
					}
				}
			}

			virtual void onFocusIn(const MouseEvent& mouseEvent)override
			{
				if (m_editable)
					craft_engine_gui_emit(editReady, );
			}
			virtual void onFocusOut(const MouseEvent& mouseEvent)override
			{
				if (m_editable)
					craft_engine_gui_emit(editFinished, getText());
			}
			virtual void   onResizeEvent(const ResizeEvent& resizeEvent) override { _Adjust_Text(); _Adjust_Placeholder_Text(); }
			virtual String onTextCopy() { return String(); }
			virtual void   onTextPause(const String& text) {};


		};






	}
}
