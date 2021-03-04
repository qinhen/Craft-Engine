#pragma once
#ifndef CRAFT_ENGINE_GUI_TEXT_BOX_H_
#define CRAFT_ENGINE_GUI_TEXT_BOX_H_
#include "./ScrollWidget.h"

namespace CraftEngine
{
	namespace gui
	{

		
		class TextBox : public ScrollWidget
		{
		private:

			String m_text;

			struct LineInfo
			{
				StringRef string;
				int32_t charIndexStart;
				int32_t charIndexEnd;
				Point   basePoint;
				Rect    highlightRect;
			};

			CraftEngine::core::ArrayList<LineInfo> m_lineInfos;

			int m_cursorIndex = 0;
			Rect m_cursorRect;
			Timer m_timer;
			bool m_draw = false;
			bool m_isAutoWarp = true;

			Point m_tempPressPoint;
			StringRef m_highlightText;
			CraftEngine::core::ArrayList<LineInfo> m_highlightLineInfo;
			int m_hightlightStartIndex = 0;
			int m_hightlightEndIndex = 0;


			int _Get_YLen()
			{
				return GuiFontSystem::calcFontLineHeight(getFont());
			}


			void _Adjust_Text()
			{
				if (m_text.size() > 0)
				{
					m_lineInfos.resize(1);
					m_lineInfos[0].string = StringRef();
					m_lineInfos[0].basePoint = Point(0, 0);
					m_lineInfos[0].charIndexStart = 0;
					m_lineInfos[0].charIndexEnd = 0;
				}
				else
				{
					m_lineInfos.resize(0);
					setScrollArea(Size(0, 0));
					return;
				}

				const int xlen = getRect().mWidth - getScrollBarWidth() - 4;
				const int ylen = _Get_YLen(); //
				int width_acc = 0;
				float scalar = getFont().getSize() * GuiFontSystem::getGlobalFontScalar(getFont().getFontID());
				int interval = getFont().getInterval();
				int xadvance = 0;
				int cur_line_index = 0;
				int max_x_width = 0;
				Rect line_rect = Rect(0, 0, xlen, ylen);

				for (int i = 0; i < m_text.size(); i++)
				{
					if (m_text[i] == L'\n' || m_text[i] == L'\r')
					{
						m_lineInfos[cur_line_index].charIndexEnd = i;
						m_lineInfos[cur_line_index].string = StringRef(m_text, m_lineInfos[cur_line_index].charIndexEnd - m_lineInfos[cur_line_index].charIndexStart, m_lineInfos[cur_line_index].charIndexStart);
						m_lineInfos[cur_line_index].basePoint = GuiFontSystem::calcFontBasePoint(m_lineInfos[cur_line_index].string, line_rect, getFont());
						cur_line_index++;
						m_lineInfos.resize(cur_line_index + 1);
						m_lineInfos[cur_line_index].charIndexStart = i + 1;
						m_lineInfos[cur_line_index].charIndexEnd = i + 1;

						line_rect.mY += ylen;

						width_acc = 0;
						continue;
					}
					
					xadvance = GuiFontSystem::GuiFontSystem::step_forward(GuiFontSystem::getGlobalFontUnit(m_text[i], getFont().getFontID()).xadvance, scalar, interval); // 

					width_acc += xadvance;
					if (max_x_width < width_acc)
						max_x_width = width_acc;

					if (m_isAutoWarp && width_acc > xlen)
					{
						m_lineInfos[cur_line_index].charIndexEnd = i - 1;
						m_lineInfos[cur_line_index].string = StringRef(m_text, m_lineInfos[cur_line_index].charIndexEnd - m_lineInfos[cur_line_index].charIndexStart + 1, m_lineInfos[cur_line_index].charIndexStart);
						m_lineInfos[cur_line_index].basePoint = GuiFontSystem::calcFontBasePoint(m_lineInfos[cur_line_index].string, line_rect, getFont());
						cur_line_index++;
						m_lineInfos.resize(cur_line_index + 1);
						m_lineInfos[cur_line_index].charIndexStart = i;
						m_lineInfos[cur_line_index].charIndexEnd = i;

						line_rect.mY += ylen;

						width_acc = xadvance;
					}

				}

				m_lineInfos[cur_line_index].charIndexEnd = m_text.size() - 1;
				m_lineInfos[cur_line_index].string = StringRef(m_text, m_lineInfos[cur_line_index].charIndexEnd - m_lineInfos[cur_line_index].charIndexStart + 1, m_lineInfos[cur_line_index].charIndexStart);
				m_lineInfos[cur_line_index].basePoint = GuiFontSystem::calcFontBasePoint(m_lineInfos[cur_line_index].string, line_rect, getFont());

				int max_text_ylen = m_lineInfos.size() * (ylen + 1);
				setScrollArea(Size(m_isAutoWarp ? 0 : max_x_width + 4, max_text_ylen));
			}

			void _Set_Cursor(int index)
			{
				m_cursorIndex = index;
				if (m_text.size() == 0)
				{
					const int xlen = getRect().mWidth;
					const int ylen = _Get_YLen(); //
					auto base_point = GuiFontSystem::calcFontBasePoint(StringRef(), Rect(0, 0, xlen, ylen), getFont());
					m_cursorRect = GuiFontSystem::calcFontCursorRect(String(), base_point, getFont());
				}
				else
				{
					int line_index = 0;
					int line_char_index = 0;
					if (index == m_text.size())
					{
						line_index = m_lineInfos.size() - 1;
						line_char_index = index - m_lineInfos[line_index].charIndexStart;
					}
					else
					{
						for (int i = 0; i < m_lineInfos.size(); i++)
						{
							if (index >= m_lineInfos[i].charIndexStart && index <= m_lineInfos[i].charIndexEnd)
							{
								line_index = i;
								break;
							}
						}
						line_index = CraftEngine::math::clamp(line_index, 0, m_lineInfos.size() - 1);
						line_char_index = index - m_lineInfos[line_index].charIndexStart;
					}
					m_cursorRect = GuiFontSystem::calcFontCursorRect(m_lineInfos[line_index].string.substr_ref(line_char_index, 0), m_lineInfos[line_index].basePoint, getFont());
				}
				_On_Cursor_Pos_Changed();
			}

			void _Set_Cursor(int line_index, int line_char_index)
			{
				m_cursorIndex = line_char_index + m_lineInfos[line_index].charIndexStart;
				m_cursorRect = GuiFontSystem::calcFontCursorRect(m_lineInfos[line_index].string.substr_ref(line_char_index, 0), m_lineInfos[line_index].basePoint, getFont());
				_On_Cursor_Pos_Changed();
			}

			void _On_Cursor_Pos_Changed()
			{
				auto point = m_cursorRect.mOffset + getChildOriginal();
				Offset offset = Offset(0);
				if (point.x < 0)
					offset.x = point.x;
				else if(point.x + 2 > getWidth())
					offset.x = point.x + 2 - getWidth();

				if (point.y < 0)
					offset.y = point.y;
				else if (point.y + m_cursorRect.mHeight + 1 > getHeight() && m_cursorRect.mHeight + 1 < getHeight())
					offset.y = point.y + m_cursorRect.mHeight + 1 - getHeight();

				if (offset.x != 0 || offset.y != 0)
					scroll(offset.x, offset.y);
			}

		public:

			craft_engine_gui_signal(textChanged, void(const String&));

			TextBox(const Rect& rect, Widget* parent) :ScrollWidget(rect, parent)
			{
				getFont().getAlignment().mHorizonMode = Alignment::eAlignmentMode_Left;
				setCharAcceptable(true);
				setDragable(true);
				setClickable(true);
				setMouseWheelable(true);
				setCursorType(AbstractCursor::SystemCursorType::eEditCursor);
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eTextInput));
				m_isAutoWarp = true; // 
			}

			TextBox(Widget* parent) :TextBox(Rect(0, 0, 400, 400), parent)
			{

			}

			const String& getText()const
			{
				return m_text;
			}

			void setText(const String& str)
			{
				m_highlightText = StringRef();
				m_hightlightStartIndex = 0;
				m_hightlightEndIndex = 0;
				m_text = str;
				_Adjust_Text();
				_Set_Cursor(m_text.size());
				craft_engine_gui_emit(textChanged, getText());
			}

			void setAutoWarp(bool enable)
			{
				if (m_isAutoWarp != enable)
				{
					m_isAutoWarp = enable; // 
					_Adjust_Text();
				}
			}

			void selectText(const Point start_point, const Point end_point)
			{
				const int ylen = _Get_YLen(); //

				int start_line_index = CraftEngine::math::clamp(start_point.y / ylen, 0, m_lineInfos.size() - 1);
				int start_line_char_index = GuiFontSystem::calcFontPointerIndex(m_lineInfos[start_line_index].string, start_point.x - m_lineInfos[start_line_index].basePoint.x, getFont());
				int end_line_index = CraftEngine::math::clamp(end_point.y / ylen, 0, m_lineInfos.size() - 1);
				int end_line_char_index = GuiFontSystem::calcFontPointerIndex(m_lineInfos[end_line_index].string, end_point.x - m_lineInfos[end_line_index].basePoint.x, getFont());

				auto start_index = m_lineInfos[start_line_index].charIndexStart + start_line_char_index;
				auto end_index = m_lineInfos[end_line_index].charIndexStart + end_line_char_index;
				auto min_index = CraftEngine::math::min(start_index, end_index);
				auto max_index = CraftEngine::math::max(start_index, end_index);

				auto min_line_index = start_line_index;
				auto min_line_char_index = start_line_char_index;
				auto max_line_index = end_line_index;
				auto max_line_char_index = end_line_char_index;
				if (start_line_index > end_line_index)
				{
					min_line_index = end_line_index;
					min_line_char_index = end_line_char_index;
					max_line_index = start_line_index;
					max_line_char_index = start_line_char_index;
				}

				auto temp_highlight_rect_offset = GuiFontSystem::calcFontLineOffset(getFont());
				m_highlightLineInfo.resize(0);
				if (start_line_index == end_line_index)
				{
					m_highlightLineInfo.resize(1);
					m_highlightLineInfo[0].string = StringRef(m_text, max_index - min_index, min_index);
					m_highlightLineInfo[0].basePoint.y = m_lineInfos[start_line_index].basePoint.y;
					m_highlightLineInfo[0].basePoint.x = m_lineInfos[start_line_index].basePoint.x + GuiFontSystem::calcFontLineWidth(m_lineInfos[start_line_index].string.substr_ref(min_index - m_lineInfos[start_line_index].charIndexStart, 0), getFont());

					m_highlightLineInfo[0].highlightRect.mX = m_highlightLineInfo[0].basePoint.x;
					m_highlightLineInfo[0].highlightRect.mY = m_highlightLineInfo[0].basePoint.y + temp_highlight_rect_offset;
					m_highlightLineInfo[0].highlightRect.mWidth = GuiFontSystem::calcFontLineWidth(m_highlightLineInfo[0].string, getFont());
					m_highlightLineInfo[0].highlightRect.mHeight = m_cursorRect.mHeight;
				}
				else
				{
					m_highlightLineInfo.resize(max_line_index - min_line_index + 1);

					m_highlightLineInfo[0].string = m_lineInfos[min_line_index].string.substr_ref(m_lineInfos[min_line_index].string.size() - min_line_char_index, min_line_char_index);
					m_highlightLineInfo[0].basePoint.y = m_lineInfos[min_line_index].basePoint.y;
					m_highlightLineInfo[0].basePoint.x = m_lineInfos[min_line_index].basePoint.x + GuiFontSystem::calcFontLineWidth(m_lineInfos[min_line_index].string.substr_ref(min_line_char_index, 0), getFont());
					m_highlightLineInfo[0].highlightRect.mX = m_highlightLineInfo[0].basePoint.x;
					m_highlightLineInfo[0].highlightRect.mY = m_highlightLineInfo[0].basePoint.y + temp_highlight_rect_offset;
					m_highlightLineInfo[0].highlightRect.mWidth = GuiFontSystem::calcFontLineWidth(m_highlightLineInfo[0].string, getFont());
					m_highlightLineInfo[0].highlightRect.mHeight = m_cursorRect.mHeight;
					for (int i = min_line_index + 1, j = 1; i < max_line_index; i++, j++)
					{
						m_highlightLineInfo[j].string = m_lineInfos[i].string;
						m_highlightLineInfo[j].basePoint = m_lineInfos[i].basePoint;
						m_highlightLineInfo[j].highlightRect.mX = m_highlightLineInfo[j].basePoint.x;
						m_highlightLineInfo[j].highlightRect.mY = m_highlightLineInfo[j].basePoint.y + temp_highlight_rect_offset;
						m_highlightLineInfo[j].highlightRect.mWidth = GuiFontSystem::calcFontLineWidth(m_highlightLineInfo[j].string, getFont());
						m_highlightLineInfo[j].highlightRect.mHeight = m_cursorRect.mHeight;
					}

					auto last_index = m_highlightLineInfo.size() - 1;
					m_highlightLineInfo[last_index].string = m_lineInfos[max_line_index].string.substr_ref(max_line_char_index, 0);
					m_highlightLineInfo[last_index].basePoint.y = m_lineInfos[max_line_index].basePoint.y;
					m_highlightLineInfo[last_index].basePoint.x = m_lineInfos[max_line_index].basePoint.x;
					m_highlightLineInfo[last_index].highlightRect.mX = m_highlightLineInfo[last_index].basePoint.x;
					m_highlightLineInfo[last_index].highlightRect.mY = m_highlightLineInfo[last_index].basePoint.y + temp_highlight_rect_offset;
					m_highlightLineInfo[last_index].highlightRect.mWidth = GuiFontSystem::calcFontLineWidth(m_highlightLineInfo[last_index].string, getFont());
					m_highlightLineInfo[last_index].highlightRect.mHeight = m_cursorRect.mHeight;
				}
				m_hightlightStartIndex = min_index;
				m_hightlightEndIndex = max_index;
				m_highlightText = StringRef(m_text, max_index - min_index, min_index);

				_Set_Cursor(end_line_index, end_line_char_index);
			}

			void selectAllText()
			{
				selectText(Point(-65535), Point(65535)); // 
			}

		protected:


			virtual void onAcceptChar(wchar_t c)override final
			{
				bool text_change = false;
				if (m_highlightText.size() != 0)
				{
					m_text.erase(m_hightlightStartIndex, m_hightlightEndIndex - m_hightlightStartIndex);
					m_cursorIndex = m_hightlightStartIndex;
					text_change = true;
				}
				if (c == L'\b')
				{
					if (m_cursorIndex > 0 && m_highlightText.size() == 0)
					{
						m_text.erase(--m_cursorIndex, 1); //
						text_change = true;
					}
				}
				else if (!isascii(c) || ispunct(c) || isalpha(c) || isdigit(c) || c == L'\x20' || c == L'\n' || c == L'\r')
				{
					m_text.insert(m_cursorIndex++, 1, c);
					text_change = true;
				}

				m_highlightText = StringRef();
				_Adjust_Text();
				_Set_Cursor(m_cursorIndex);
				if (text_change)
					craft_engine_gui_emit(textChanged, getText());
			}

			virtual void onKeyBoardEvent(const KeyboardEvent& keyboardEvent) override
			{
				if (keyboardEvent.down)
				{
					if (keyboardEvent.key == (KeyBoard::eKey_Left) && m_cursorIndex > 0)
					{
						_Set_Cursor(--m_cursorIndex);
						m_draw = true;
					}
					else if (keyboardEvent.key == (KeyBoard::eKey_Right) && m_cursorIndex < m_text.size())
					{
						_Set_Cursor(++m_cursorIndex);
						m_draw = true;
					}
					if (keyboardEvent.key == (KeyBoard::eKey_Up))
					{
						auto local_pos = m_cursorRect.mOffset;
						local_pos.y -= m_cursorRect.mHeight / 2;
						const int ylen = _Get_YLen(); //
						int line_index = CraftEngine::math::clamp(local_pos.y / ylen, 0, m_lineInfos.size() - 1);
						int line_char_index = GuiFontSystem::calcFontPointerIndex(m_lineInfos[line_index].string, local_pos.x - m_lineInfos[line_index].basePoint.x, getFont());
						_Set_Cursor(line_index, line_char_index);
					}
					else if (keyboardEvent.key == (KeyBoard::eKey_Down))
					{
						auto local_pos = m_cursorRect.mOffset;
						local_pos.y += m_cursorRect.mHeight / 2 + m_cursorRect.mHeight;
						const int ylen = _Get_YLen(); //
						int line_index = CraftEngine::math::clamp(local_pos.y / ylen, 0, m_lineInfos.size() - 1);
						int line_char_index = GuiFontSystem::calcFontPointerIndex(m_lineInfos[line_index].string, local_pos.x - m_lineInfos[line_index].basePoint.x, getFont());
						_Set_Cursor(line_index, line_char_index);
					}
				}
			}

			virtual void onPaintEvent() override
			{
				auto text_painter = getPainter(GroupID::gDefaultWidget);
				drawBackground();

				const int y_len = _Get_YLen(); //
				int y_acc = +getChildOriginal().y;

				{
					int min_one = 0;
					int max_one = m_lineInfos.size();
					{
						int lower_bound = m_lineInfos.size() - 1;
						int upper_bound = 0;
						int temp = 0;
						int bottom = 0;
						int pos = -getChildOriginal().y;
						while (upper_bound <= lower_bound)
						{
							temp = (upper_bound + lower_bound) / 2;
							bottom = (temp + 1) * y_len;
							if (pos < bottom)
								lower_bound = temp - 1;
							else
								upper_bound = temp + 1;
						}
						min_one = temp;
					}
					{
						int lower_bound = m_lineInfos.size() - 1;
						int upper_bound = 0;
						int temp = 0;
						int top = 0;
						int pos = -getChildOriginal().y + getHeight();
						while (upper_bound <= lower_bound)
						{
							temp = (upper_bound + lower_bound) / 2;
							top = temp * y_len;
							if (pos < top)
								lower_bound = temp - 1;
							else
								upper_bound = temp + 1;
						}
						max_one = temp + 1;
					}
					min_one = math::clamp(min_one, 0, (int)m_lineInfos.size());
					max_one = math::clamp(max_one, 0, (int)m_lineInfos.size());
					for (int i = min_one; i < max_one; i++)
						text_painter.drawTextLine(m_lineInfos[i].string.data(), m_lineInfos[i].string.size(), m_lineInfos[i].basePoint, getFont(), getPalette().mForegroundColor);
				}
				

				if (isInputFocus())
				{

					if (m_highlightText.size() != 0)
					{
						int min_one = 0;
						int max_one = m_highlightLineInfo.size();
						{
							int lower_bound = m_highlightLineInfo.size() - 1;
							int upper_bound = 0;
							int temp = 0;
							int bottom = 0;
							int pos = -getChildOriginal().y;
							while (upper_bound <= lower_bound)
							{
								temp = (upper_bound + lower_bound) / 2;
								bottom = m_highlightLineInfo[temp].highlightRect.bottom();
								if (pos < bottom)
									lower_bound = temp - 1;
								else
									upper_bound = temp + 1;
							}
							min_one = temp;
						}
						{
							int lower_bound = m_highlightLineInfo.size() - 1;
							int upper_bound = 0;
							int temp = 0;
							int top = 0;
							int pos = -getChildOriginal().y + getHeight();
							while (upper_bound <= lower_bound)
							{
								temp = (upper_bound + lower_bound) / 2;
								top = m_highlightLineInfo[temp].highlightRect.top();
								if (pos < top)
									lower_bound = temp - 1;
								else
									upper_bound = temp + 1;
							}
							max_one = temp + 1;
						}
						min_one = math::clamp(min_one, 0, (int)m_highlightLineInfo.size());
						max_one = math::clamp(max_one, 0, (int)m_highlightLineInfo.size());

						Rect hightlight_rect;
						Point hightlight_basepoint;

						for (int i = min_one; i < max_one; i++)
							text_painter.drawRect(m_highlightLineInfo[i].highlightRect, getPalette().mBackgroundHighLightColor);
						for (int i = min_one; i < max_one; i++)
							text_painter.drawTextLine(m_highlightLineInfo[i].string.data(), m_highlightLineInfo[i].string.size(), m_highlightLineInfo[i].basePoint, getFont(), getPalette().mForegroundHighLightColor);
					}
					if (!m_timer.isValid())
					{
						m_timer.setDuration(500);
						m_timer.setCallback(CraftEngine::core::Callback<void(float, float)>([=](float, float) { m_draw = !m_draw; sendRepaintEvent(); })); // 
						auto main_window = this->getRootWidget();
						if(main_window != nullptr)
						m_timer.startTimer(main_window->getSyncTimerSystem());
					}
					if (m_draw)
					{
						text_painter.drawRect(m_cursorRect, getPalette().mForegroundColor);
					}

				}
				onPaint_drawScrollBar();
				drawFrame();
			}

			virtual void onPressed(const MouseEvent& mouseEvent) override final
			{
				m_tempPressPoint = getLocalPos(mouseEvent.global);	
				m_highlightText = StringRef();
				onReleased(mouseEvent);
			}

			virtual void onReleased(const MouseEvent& mouseEvent) override final
			{
				if (m_lineInfos.size() == 0)
				{
					_Set_Cursor(0);
				}
				else
				{
					auto local_pos = getLocalPos(mouseEvent.global);
					const int ylen = _Get_YLen(); //
					int line_index = CraftEngine::math::clamp(local_pos.y / ylen, 0, m_lineInfos.size() - 1);
					int line_char_index = GuiFontSystem::calcFontPointerIndex(m_lineInfos[line_index].string, local_pos.x - m_lineInfos[line_index].basePoint.x, getFont());
					_Set_Cursor(line_index, line_char_index);
				}
			}

			virtual void onDrag(const MouseEvent& mouseEvent) override final
			{
				if (m_text.size() == 0)
					return;
				auto start_point = m_tempPressPoint;
				auto end_point = getLocalPos(mouseEvent.global) + mouseEvent.offset;
				selectText(start_point, end_point);
			}

			virtual void    onResizeEvent(const ResizeEvent& resizeEvent) override { _Adjust_Text(); }
		};


	}
}

#endif // CRAFT_ENGINE_GUI_TEXT_BOX_H_