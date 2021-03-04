#pragma once
#include "../widgets/ScrollWidget.h"
#include "../widgets/ComboBox.h"
#include "../widgets/Label.h"

namespace CraftEngine
{
	namespace gui
	{

		class ColorTextEdit : public ScrollWidget
		{
		public:
			enum class PaletteIndex
			{
				Default,
				Keyword,
				Number,
				String,
				CharLiteral,
				Punctuation,
				Preprocessor,
				Identifier,
				KnownIdentifier,
				PreprocIdentifier,
				Comment,
				MultiLineComment,
				Background,
				Cursor,
				Selection,
				ErrorMarker,
				Breakpoint,
				LineNumber,
				CurrentLineFill,
				CurrentLineFillInactive,
				CurrentLineEdge,
				Max
			};
			// Represents a character coordinate from the user's point of view,
			// i. e. consider an uniform grid (assuming fixed-width font) on the
			// screen as it is rendered, and each cell has its own coordinate, starting from 0.
			// Tabs are counted as [1..mTabSize] count empty spaces, depending on
			// how many space is necessary to reach the next tab stop.
			// For example, coordinate (1, 5) represents the character 'B' in a line "\tABC", when mTabSize = 4,
			// because it is rendered as "    ABC" on the screen.
			struct Coordinates
			{
				int mLine, mColumn;
				Coordinates() : mLine(0), mColumn(0) {}
				Coordinates(int aLine, int aColumn) : mLine(aLine), mColumn(aColumn)
				{
					assert(aLine >= 0);
					assert(aColumn >= 0);
				}
				static Coordinates Invalid() { static Coordinates invalid(-1, -1); return invalid; }

				bool operator ==(const Coordinates& o) const
				{
					return
						mLine == o.mLine &&
						mColumn == o.mColumn;
				}

				bool operator !=(const Coordinates& o) const
				{
					return
						mLine != o.mLine ||
						mColumn != o.mColumn;
				}

				bool operator <(const Coordinates& o) const
				{
					if (mLine != o.mLine)
						return mLine < o.mLine;
					return mColumn < o.mColumn;
				}

				bool operator >(const Coordinates& o) const
				{
					if (mLine != o.mLine)
						return mLine > o.mLine;
					return mColumn > o.mColumn;
				}

				bool operator <=(const Coordinates& o) const
				{
					if (mLine != o.mLine)
						return mLine < o.mLine;
					return mColumn <= o.mColumn;
				}

				bool operator >=(const Coordinates& o) const
				{
					if (mLine != o.mLine)
						return mLine > o.mLine;
					return mColumn >= o.mColumn;
				}
			};

			struct Identifier
			{
				Coordinates mLocation;
				std::string mDeclaration;
			};
		private:
			using CharList = String;
			using ColorList = core::ArrayList<uint16_t>;

			struct CharAttribute
			{
				bool mComment : 1;
				bool mMultiLineComment : 1;
				bool mPreprocessor : 1;
				CharAttribute() :mComment(false), mMultiLineComment(false), mPreprocessor(false) { sizeof(*this); }
			};

			using AttribList = core::ArrayList<CharAttribute>;
			struct Line
			{
				Point basepoint;
				CharList chars;
				ColorList colors;
				AttribList attribs;
			};
			using Lines = core::ArrayList<Line>;

			typedef std::unordered_map<String, Identifier> Identifiers;
			typedef std::unordered_set<String> Keywords;
			/*typedef std::map<int, std::string> ErrorMarkers;*/
			/*typedef std::unordered_set<int> Breakpoints;*/

			struct LanguageDefinition
			{
				typedef std::pair<String, PaletteIndex> TokenRegexString;
				typedef std::vector<TokenRegexString> TokenRegexStrings;
				typedef bool(*TokenizeCallback)(const Char* in_begin, const Char* in_end, const Char*& out_begin, const Char*& out_end, PaletteIndex& paletteIndex);

				String mName;
				Keywords mKeywords;
				Identifiers mIdentifiers;
				Identifiers mPreprocIdentifiers;
				String mCommentStart, mCommentEnd, mSingleLineComment;
				Char mPreprocChar;
				bool mAutoIndentation;

				TokenizeCallback mTokenize;
				TokenRegexStrings mTokenRegexStrings;

				bool mCaseSensitive;

				LanguageDefinition()
					: mPreprocChar(L'#'), mAutoIndentation(true), mTokenize(nullptr), mCaseSensitive(true)
				{
				}
				static const LanguageDefinition& getLanguage(const std::string& name);
				static const LanguageDefinition& CPlusPlus();
				static const LanguageDefinition& HLSL();
				static const LanguageDefinition& GLSL();
				static const LanguageDefinition& C();
				static const LanguageDefinition& SQL();
				static const LanguageDefinition& AngelScript();
				static const LanguageDefinition& Lua();
			};

			class StatueBarWidget : public Widget
			{
			public:
				Label* m_lineIndexLabel;
				Label* m_columnIndexLabel;
				ComboBox* m_languageTypeComboBox;
				std::function<void(int)> m_comboBoxSeclectionChanged;

				StatueBarWidget(Widget* parent) : Widget(parent)
				{
					getPalette().mFrameMode = Palette::FrameMode::eFrameMode_Surround;
					getPalette().mFillMode = Palette::FillMode::eFillMode_Color;
					m_lineIndexLabel = new Label(this);
					m_columnIndexLabel = new Label(this);
					m_languageTypeComboBox = new ComboBox(this);
					_Adjust_Layout();
					craft_engine_gui_connect_v2(m_languageTypeComboBox, selectionChanged, [=](int i) { m_comboBoxSeclectionChanged(i); });
				}

				void _Adjust_Layout()
				{
					const int c = 3;
					int w = getWidth() / c;
					int lw = getWidth() - w * (c - 1);
					int x = 0;
					m_lineIndexLabel->setRect(Rect(x, 0, w, getHeight()));
					x += w;
					m_columnIndexLabel->setRect(Rect(x, 0, w, getHeight()));
					x += w;
					m_languageTypeComboBox->setRect(Rect(x, 0, lw, getHeight()));
				}
				void setStatueComboxIndex(int i)
				{
					m_languageTypeComboBox->setCurrentIndex(i);
				}
				void setStatueLineIndex(int x)
				{
					m_lineIndexLabel->setText(L"line:" + StringTool::fromValue(x));
				}
				void setStatueColumnIndex(int x)
				{
					m_columnIndexLabel->setText(L"column:" + StringTool::fromValue(x));
				}
				void setLanguageList(const StringList& langs)
				{
					m_languageTypeComboBox->setItems(langs);
				}
				void setLanguageFunc(const std::function<void(int)>& func)
				{
					m_comboBoxSeclectionChanged = func;
				}
				virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
				{
					_Adjust_Layout();
				}
			};



			Lines m_lines;

			Color m_textPalette[21];
			typedef std::vector<std::pair<std::wregex, PaletteIndex>> RegexList;
			LanguageDefinition mLanguageDefinition;
			RegexList mRegexList;
			bool m_CheckComments = true;
			bool m_TextChanged = true;
			int m_colorizeBeginLine = 0;
			int m_maxLineWidth = 0;

			Coordinates m_cursorPos;
			Rect m_cursorRect;
			Timer m_timer;
			bool m_draw = false;

			Point m_tempPressPoint;
			bool m_isEditable = true;
			Point m_tabOffset = Point(60, 22);

			StatueBarWidget* m_statueBar;
		public:

			ColorTextEdit(Widget* parent) : ScrollWidget(parent)
			{
				getFont().setFontID((int)FontSystem::FontIndex::eFixedWidth);
				getFont().getAlignment().mHorizonMode = Alignment::eAlignmentMode_Left;
				setCharAcceptable(true);
				//setDragable(true);
				setClickable(true);
				setMouseWheelable(true);
				setCursorType(AbstractCursor::SystemCursorType::eEditCursor);
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eTextInput));
				m_lines.push_back(Line());
				m_lines[0].basepoint = FontSystem::calcFontBasePoint(StringRef(), Rect(0, 0, getWidth(), _Get_YLen()), getFont());
				setTextPaletteStyle((int)GuiColorStyle::getCurColorSytle());

				m_statueBar = new StatueBarWidget(nullptr);
				m_statueBar->setGroupid(3);
				m_statueBar->bindParentWidget(this);
				//this->bindChildWidget(m_statueBar);
				m_statueBar->setRect(Rect(0, 0, getWidth(), m_tabOffset.y));

				m_statueBar->setStatueLineIndex(0);
				m_statueBar->setStatueColumnIndex(0);

				m_statueBar->setLanguageList({ L"disable",L"C++",L"C",L"GLSL",L"Lua",L"HLSL",L"SQL", });
				m_statueBar->setLanguageFunc([=](int i) {
					switch (i)
					{
					case 0: setLanguage("disable"); break;
					case 1: setLanguage("C++"); break;
					case 2: setLanguage("C"); break;
					case 3: setLanguage("GLSL"); break;
					case 4: setLanguage("Lua"); break;
					case 5: setLanguage("HLSL"); break;
					case 6: setLanguage("SQL"); break;
					default:setLanguage("disable"); break;
					}
				});

				_Update_Cursor_Rect();
			}

			~ColorTextEdit()
			{
				delete m_statueBar;
			}

			void setTextPaletteStyle(int style);

			void setLanguageDefinition(const LanguageDefinition& aLanguageDef)
			{
				mLanguageDefinition = aLanguageDef;
				mRegexList.clear();

				for (auto& r : mLanguageDefinition.mTokenRegexStrings)
					mRegexList.push_back(std::make_pair(std::wregex(r.first, std::regex_constants::optimize), r.second));

				Colorize();
			}
			const LanguageDefinition& getLanguageDefinition() const { return mLanguageDefinition; }

			void setLanguage(const std::string& name)
			{
				auto copy = name;
				for (auto& it : copy)
					it = tolower(it);
				setLanguageDefinition(LanguageDefinition::getLanguage(copy));
				auto selections = { "disable", "c++", "c", "glsl", "lua", "hlsl", "sql", };
				int i = 0;
				for (auto it : selections)
				{
					if (copy == it)
					{
						m_statueBar->setStatueComboxIndex(i);
						return;
					}
					i++;
				}
				m_statueBar->setStatueComboxIndex(0);
			}

			void setEditable(bool enable) { m_isEditable = enable; }
			bool isEditable() { return m_isEditable; }

			void setText(const std::string& aText)
			{
				m_lines.clear();
				m_lines.emplace_back(Line());
				for (auto chr : aText)
				{
					if (chr == '\n' || chr == '\r')
						m_lines.emplace_back(Line());
					else
					{
						m_lines.back().chars.push_back(chr);
						m_lines.back().attribs.emplace_back(CharAttribute());
						m_lines.back().colors.emplace_back((uint16_t)PaletteIndex::Default);
					}
				}
				auto yLen = _Get_YLen();
				m_maxLineWidth = 0;
				for (int i = 0; i < m_lines.size(); i++)
					m_lines[i].basepoint = FontSystem::calcFontBasePoint(m_lines[i].chars, Rect(0, yLen * i, getWidth(), yLen), getFont());
				for (int i = 0; i < m_lines.size(); i++)
				{
					auto w = FontSystem::calcFontLineWidth(m_lines[i].chars, getFont());
					if (w > m_maxLineWidth)
						m_maxLineWidth = w;
				}
				setScrollArea(m_tabOffset + Size(m_maxLineWidth + 100, yLen * (m_lines.size() + 10)));

				m_TextChanged = true;
				m_CheckComments = true;
				m_colorizeBeginLine = 0;

				m_cursorPos = Coordinates();
				//mScrollToTop = true;
				//mUndoBuffer.clear();
				//mUndoIndex = 0;
				Colorize();
			}

			void onPaint_drawStatueBar(const PaintEvent& paintEvent)
			{
				GuiRenderer* renderer = paintEvent.pRenderer;
				const Point& global_relative_point = paintEvent.globalRelativePoint;
				const Rect& global_limit_rect = paintEvent.globalLimitRect;

				auto this_global_rect = Rect(global_relative_point + getPos(), getSize());
				auto this_global_base_point = this_global_rect.mOffset;
				Rect child_global_limit_rect = global_limit_rect.disjunction(this_global_rect);

				PaintEvent subEvent;
				subEvent.pRenderer = paintEvent.pRenderer;
				subEvent.globalRelativePoint = this_global_base_point;
				subEvent.globalLimitRect = child_global_limit_rect;
				if (!m_statueBar->isHide())
					m_statueBar->onPaintEvent(subEvent);
			}

			virtual void onPaintEvent(const PaintEvent& paintEvent) override
			{
				_Frame_Colorize();

				GuiRenderer* renderer = paintEvent.pRenderer;
				const Point& global_relative_point = paintEvent.globalRelativePoint;
				const Rect& global_limit_rect = paintEvent.globalLimitRect;

				auto this_global_rect = Rect(global_relative_point + getPos(), getSize());
				Rect child_global_limit_rect = global_limit_rect.disjunction(this_global_rect);
				auto basepoint = this_global_rect.mOffset;
				auto total_offset = this_global_rect.mOffset + getChildOriginal(0);
				auto vertical_tab_offset = this_global_rect.mOffset + getChildOriginal(1);
				auto horizontal_tab_offset = this_global_rect.mOffset + getChildOriginal(2);
				onPaint_drawBackground(renderer, this_global_rect, global_limit_rect);

				auto range = _Get_Visual_Line_Range();
				auto text_global_limit_rect = child_global_limit_rect;
				text_global_limit_rect.mOffset += m_tabOffset;
				text_global_limit_rect.mSize -= m_tabOffset;
				for (int i = range.x; i < range.y; i++)
				{
					auto basepoint = m_lines[i].basepoint + total_offset;
					renderer->drawPaletteTextLine(m_lines[i].chars.data(), m_lines[i].chars.size(), basepoint, getFont(), m_textPalette, m_lines[i].colors.data(), text_global_limit_rect);
				}

				const int yLen = _Get_YLen();
				if (isInputFocus())
				{
					if (!m_timer.isValid())
					{
						m_timer.setDuration(500);
						m_timer.setCallback(CraftEngine::core::Callback<void(float, float)>([=](float, float) { m_draw = !m_draw; }));
						auto main_window = this->getRootWidget();
						if (main_window != nullptr)
							m_timer.startTimer(main_window->getSyncTimerSystem());
					}
					if (m_draw)
					{
						auto baseCur = m_cursorRect;
						baseCur.mOffset = m_cursorRect.mOffset + total_offset;
						renderer->drawRect(baseCur, getPalette().mForegroundColor, text_global_limit_rect);
					}
					int w = math::max(m_maxLineWidth + 100, getWidth());
					Rect lineFrameRect = Rect(0, m_cursorPos.mLine * yLen, w, yLen);//
					lineFrameRect.mOffset += total_offset;
					renderer->drawRectFrame(lineFrameRect, getPalette().mFrameColor, text_global_limit_rect);
				}

				{
					// vertical tab frame
					Rect verticalBarRect = Rect(0, m_tabOffset.y, m_tabOffset.x, getHeight() - m_tabOffset.y);
					verticalBarRect.mOffset += this_global_rect.mOffset;

					// vertical tab index
					Rect verticalTabRect = Rect(0, 0, m_tabOffset.x, yLen);
					verticalTabRect.mOffset += getChildOriginal(2);
					verticalTabRect.mOffset += this_global_rect.mOffset;
					int baseY = verticalTabRect.mY;
					Font tabFont = getFont();
					tabFont.getAlignment().mHorizonMode = Alignment::eAlignmentMode_Right;
					for (int i = range.x; i < range.y; i++)
					{
						verticalTabRect.mY = baseY + yLen * i;
						String&& lineIdxStr = StringTool::fromValue(i + 1);
						auto basepoint = FontSystem::calcFontBasePoint(lineIdxStr, verticalTabRect, tabFont);
						renderer->drawTextLine(lineIdxStr, basepoint, getFont(), m_textPalette[(int)PaletteIndex::LineNumber], verticalBarRect);
					}

					renderer->drawRectFrame(verticalBarRect, getPalette().mFrameColor, child_global_limit_rect);
				}
				onPaint_drawScrollBar(paintEvent);
				onPaint_drawStatueBar(paintEvent);
				renderer->drawRectFrame(this_global_rect, getPalette().mFrameColor, global_limit_rect);
			}

		protected:
			/*
			 0: scroll widget space
			 1: srcoll bar
			 2: vertical tab
			 3: horizontal tab
			*/
			virtual Point getChildOriginal(int groupid = 0)const
			{
				switch (groupid)
				{
				case 0:
					return ScrollWidget::getChildOriginal(0) + m_tabOffset;
				case 1:
					return Point(0);
				case 2:
					return Point(0, ScrollWidget::getChildOriginal(0).y + m_tabOffset.y);
				case 3:
					return Point(0);
				default:
					return Point(0);
				}
			}

			virtual Widget* onDetectStatueBar(const MouseEvent& mouseEvent)
			{
				Widget* hit;
				MouseEvent subEvent = mouseEvent;
				subEvent.local -= (getPos() + getChildOriginal(3));
				hit = m_statueBar->onMouseEvent(subEvent);
				return hit;
			}

			virtual Widget* onDetect(const MouseEvent& mouseEvent) override
			{
				if (isHide() || isTransparent() || (isPopupable() && !isNowPopup()) || !getRect().inside(mouseEvent.local))
					return nullptr;

				Widget* hit;
				//Point localPos = mouseEvent.local - (getPos() + getChildOriginal(1));
				//Rect verticalBarRect = Rect(0, m_tabOffset.y, m_tabOffset.x, getHeight() - m_tabOffset.y);
				//if (verticalBarRect.inside(localPos))
				//{
				//	return this;
				//}
				hit = onDetectStatueBar(mouseEvent);
				if (hit != nullptr)
					return hit;
				hit = onDetectScrollBar(mouseEvent);
				if (hit != nullptr)
					return hit;

				MouseEvent subEvent = mouseEvent;
				for (int i = getChildrenWidgets().size() - 1; i >= 0; i--)
				{
					subEvent.local = mouseEvent.local - getPos() - getChildOriginal(getChildrenWidgets()[i]->getGroupid());
					hit = getChildrenWidgets()[i]->onMouseEvent(subEvent);
					if (hit != nullptr)
						return hit;
				}
				return this;
			}


			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				ScrollWidget::onResizeEvent(resizeEvent);
				m_statueBar->setRect(Rect(0, 0, getWidth(), m_tabOffset.y));
			}

		private:

			void _Remove_Char(int line, int column)
			{
				if (column == 0 && line > 0)
				{
					auto yLen = _Get_YLen();
					int begin = m_lines[line - 1].chars.size();
					int count = m_lines[line].chars.size();
					String new_str = m_lines[line - 1].chars + m_lines[line].chars;
					m_lines[line - 1].chars.swap(new_str);
					m_lines[line - 1].colors.resize(m_lines[line - 1].chars.size());
					m_lines[line - 1].attribs.resize(m_lines[line - 1].chars.size());
					m_lines[line - 1].basepoint = FontSystem::calcFontBasePoint(m_lines[line - 1].chars, Rect(0, yLen * (line - 1), getWidth(), yLen), getFont());

					auto w = FontSystem::calcFontLineWidth(m_lines[line - 1].chars, getFont());
					if (w > m_maxLineWidth)
						m_maxLineWidth = w;

					for (int i = begin, j = 0; i < begin + count; i++, j++)
					{
						m_lines[line - 1].colors[i] = m_lines[line].colors[j];
						m_lines[line - 1].attribs[i] = m_lines[line].attribs[j];
					}
					for (int i = line + 1; i < m_lines.size(); i++)
					{
						m_lines[i - 1].chars.swap(m_lines[i].chars);
						m_lines[i - 1].colors.swap(m_lines[i].colors);
						m_lines[i - 1].attribs.swap(m_lines[i].attribs);
						m_lines[i - 1].basepoint = m_lines[i].basepoint;
						m_lines[i - 1].basepoint.y -= yLen;
					}
					m_lines.resize(m_lines.size() - 1);
				}
				else if (column > 0) // && m_highlightText.size() == 0)
				{
					auto yLen = _Get_YLen();
					m_lines[line].chars.erase(column - 1, 1); //
					m_lines[line].attribs.erase(column - 1);
					m_lines[line].colors.erase(column - 1);
					m_lines[line].basepoint = FontSystem::calcFontBasePoint(m_lines[line].chars, Rect(0, yLen * (line), getWidth(), yLen), getFont());
				}
			}

			void _Insert_Char(int line, int column, Char c)
			{
				if (c == '\n' || c == '\r')
				{
					auto yLen = _Get_YLen();
					m_lines.push_back(Line());
					for (int i = m_lines.size() - 1; i > line + 1; i--)
					{
						m_lines[i].chars.swap(m_lines[i - 1].chars);
						m_lines[i].colors.swap(m_lines[i - 1].colors);
						m_lines[i].attribs.swap(m_lines[i - 1].attribs);
						m_lines[i].basepoint = m_lines[i - 1].basepoint;
						m_lines[i].basepoint.y += yLen;
					}
					int begin = column;
					int count = m_lines[line].chars.size() - column;
					if (count > 0)
						m_lines[line + 1].chars = m_lines[line].chars.substr(begin, count);
					//else
					//	m_lines[line + 1].chars.clear();
					m_lines[line + 1].attribs.resize(count);
					m_lines[line + 1].colors.resize(count);
					m_lines[line + 1].basepoint = FontSystem::calcFontBasePoint(m_lines[line + 1].chars, Rect(0, yLen * (line + 1), getWidth(), yLen), getFont());

					m_lines[line].chars.resize(begin);
					m_lines[line].attribs.resize(begin);
					m_lines[line].colors.resize(begin);
					m_lines[line].basepoint = FontSystem::calcFontBasePoint(m_lines[line].chars, Rect(0, yLen * (line), getWidth(), yLen), getFont());
				}
				else
				{
					auto yLen = _Get_YLen();
					m_lines[line].chars.insert(m_lines[line].chars.begin() + column, c);
					m_lines[line].attribs.insert(column, CharAttribute());
					m_lines[line].colors.insert(column, (uint16_t)PaletteIndex::Default);
					m_lines[line].basepoint = FontSystem::calcFontBasePoint(m_lines[line].chars, Rect(0, yLen * (line), getWidth(), yLen), getFont());

					auto w = FontSystem::calcFontLineWidth(m_lines[line].chars, getFont());
					if (w > m_maxLineWidth)
						m_maxLineWidth = w;
				}
			}

			virtual int _On_Get_Wheel_Step()
			{
				return _Get_YLen();
			}

			virtual void _On_Reset_Scroll_Bar_Layout()
			{
				auto scroll_bar_width = getScrollBarWidth();
				if (m_horizonScrollBar->isHide())
					m_verticalScrollBar->setRect(Rect(getRect().mWidth - scroll_bar_width, m_tabOffset.y, scroll_bar_width, getRect().mHeight - m_tabOffset.y));
				else if (m_verticalScrollBar->isHide())
					m_horizonScrollBar->setRect(Rect(0, getRect().mHeight - scroll_bar_width, getRect().mWidth, scroll_bar_width));
				else
				{
					m_verticalScrollBar->setRect(Rect(getRect().mWidth - scroll_bar_width, m_tabOffset.y, scroll_bar_width, getRect().mHeight - scroll_bar_width - m_tabOffset.y));
					//m_horizonScrollBar->setRect(Rect(0, getRect().mHeight - scroll_bar_width, getRect().mWidth - scroll_bar_width, scroll_bar_width));
					m_horizonScrollBar->setRect(Rect(0, getRect().mHeight - scroll_bar_width, getRect().mWidth, scroll_bar_width));
				}
			}


			virtual void onAcceptChar(wchar_t c)override final
			{
				if (!isEditable())
					return;
				//if (m_highlightText.size() != 0)
				//{
				//	m_text.erase(m_hightlightStartIndex, m_hightlightEndIndex - m_hightlightStartIndex);
				//	m_cursorIndex = m_hightlightStartIndex;
				//	text_change = true;
				//}
				if (c == L'\b')
				{
					if (m_cursorPos.mColumn == 0 && m_cursorPos.mLine > 0)
					{
						auto next_column = m_lines[m_cursorPos.mLine - 1].chars.size();
						_Remove_Char(m_cursorPos.mLine, m_cursorPos.mColumn);
						m_cursorPos.mLine--;
						m_cursorPos.mColumn = next_column;

						m_TextChanged = true;
						m_CheckComments = true;
						m_colorizeBeginLine = m_cursorPos.mLine;
					}
					else if (m_cursorPos.mColumn > 0) // && m_highlightText.size() == 0)
					{
						_Remove_Char(m_cursorPos.mLine, m_cursorPos.mColumn);
						m_cursorPos.mColumn--;

						m_TextChanged = true;
						m_CheckComments = true;
						m_colorizeBeginLine = m_cursorPos.mLine;
					}
				}
				else if (!isascii(c) || ispunct(c) || isalpha(c) || isdigit(c) || c == L'\x20' || c == L'\n' || c == L'\r' || c == '\t')
				{
					if (c == '\n' || c == '\r')
					{
						_Insert_Char(m_cursorPos.mLine, m_cursorPos.mColumn, c);
						m_cursorPos.mColumn = 0;
						m_cursorPos.mLine++;

						m_TextChanged = true;
						m_CheckComments = true;
						m_colorizeBeginLine = m_cursorPos.mLine - 1;
					}
					else
					{
						_Insert_Char(m_cursorPos.mLine, m_cursorPos.mColumn, c);
						m_cursorPos.mColumn++;

						m_TextChanged = true;
						m_CheckComments = true;
						m_colorizeBeginLine = m_cursorPos.mLine;
					}
				}

				//m_highlightText = StringRef();
				//_Adjust_Text();

				auto yLen = _Get_YLen();
				_Update_Cursor_Rect();
				setScrollArea(m_tabOffset + Size(m_maxLineWidth + 100, yLen * (m_lines.size() + 10)));
				//if (text_change)
				//	craft_engine_gui_emit(textChanged, getText());
			}

			virtual void onPressed(const MouseEvent& mouseEvent) override final
			{
				m_tempPressPoint = getLocalPos(mouseEvent.global);
				//m_highlightText = StringRef();
				onReleased(mouseEvent);
			}

			virtual void onReleased(const MouseEvent& mouseEvent) override final
			{
				if (m_lines.size() == 0)
				{
					_Set_Cursor(0, 0);
				}
				else
				{
					auto local_pos = getLocalPos(mouseEvent.global);
					const int ylen = _Get_YLen(); //
					int line_index = CraftEngine::math::clamp(local_pos.y / ylen, 0, m_lines.size() - 1);
					int line_char_index = FontSystem::calcFontPointerIndex(m_lines[line_index].chars, local_pos.x - m_lines[line_index].basepoint.x, getFont());
					_Set_Cursor(line_index, line_char_index);
				}
			}

			virtual void onKeyBoardEvent(KeyboardEvent keyboardEvent)
			{
				if (keyboardEvent.down)
				{
					if (keyboardEvent.key == (KeyBoard::eKey_Left))
					{
						if (m_cursorPos.mColumn > 0)
							m_cursorPos.mColumn--;
						else if (m_cursorPos.mLine > 0)
						{
							m_cursorPos.mLine--;
							m_cursorPos.mColumn = m_lines[m_cursorPos.mLine].chars.size();
						}
						_Update_Cursor_Rect();
						m_draw = true;
					}
					else if (keyboardEvent.key == (KeyBoard::eKey_Right))
					{
						if (m_cursorPos.mColumn < m_lines[m_cursorPos.mLine].chars.size())
							m_cursorPos.mColumn++;
						else if (m_cursorPos.mLine < m_lines.size() - 1)
						{
							m_cursorPos.mLine++;
							m_cursorPos.mColumn = 0;
						}
						_Update_Cursor_Rect();
						m_draw = true;
					}
					if (keyboardEvent.key == (KeyBoard::eKey_Up))
					{
						auto local_pos = m_cursorRect.mOffset;
						local_pos.y -= m_cursorRect.mHeight / 2;
						const int ylen = _Get_YLen(); //
						int line_index = CraftEngine::math::clamp(local_pos.y / ylen, 0, m_lines.size() - 1);
						int line_char_index = FontSystem::calcFontPointerIndex(m_lines[line_index].chars, local_pos.x - m_lines[line_index].basepoint.x, getFont());
						_Set_Cursor(line_index, line_char_index);
					}
					else if (keyboardEvent.key == (KeyBoard::eKey_Down))
					{
						auto local_pos = m_cursorRect.mOffset;
						local_pos.y += m_cursorRect.mHeight / 2 + m_cursorRect.mHeight;
						const int ylen = _Get_YLen(); //
						int line_index = CraftEngine::math::clamp(local_pos.y / ylen, 0, m_lines.size() - 1);
						int line_char_index = FontSystem::calcFontPointerIndex(m_lines[line_index].chars, local_pos.x - m_lines[line_index].basepoint.x, getFont());
						_Set_Cursor(line_index, line_char_index);
					}
				}
			}


			template<class InputIt1, class InputIt2, class BinaryPredicate>
			bool equals(InputIt1 first1, InputIt1 last1,
				InputIt2 first2, InputIt2 last2, BinaryPredicate p)
			{
				for (; first1 != last1 && first2 != last2; ++first1, ++first2)
				{
					if (!p(*first1, *first2))
						return false;
				}
				return first1 == last1 && first2 == last2;
			}

			void _Update_Cursor_Rect()
			{
				const int line = m_cursorPos.mLine;
				const int column = m_cursorPos.mColumn;
				auto base_point = m_lines[line].basepoint;
				m_cursorRect = FontSystem::calcFontCursorRect(StringRef(m_lines[line].chars.c_str(), column), base_point, getFont());
				m_draw = true;
				if (m_timer.isValid())
					m_timer.restartTimer();
				m_statueBar->setStatueLineIndex(m_cursorPos.mLine);
				m_statueBar->setStatueColumnIndex(m_cursorPos.mColumn);
			}

			void _Set_Cursor(int line_index, int line_char_index)
			{
				m_cursorPos = Coordinates(line_index, line_char_index);
				_Update_Cursor_Rect();
			}

			void CheckComments()
			{
				if (m_CheckComments)
				{

					auto endLine = m_lines.size();
					auto endIndex = 0;
					auto commentStartLine = endLine;
					auto commentStartIndex = endIndex;
					auto withinString = false;
					auto withinSingleLineComment = false;
					auto withinPreproc = false;
					auto firstChar = true;			// there is no other non-whitespace characters in the line before
					auto concatenate = false;		// '\' on the very end of the line
					auto currentLine = 0;
					auto currentIndex = 0;
					while (currentLine < endLine || currentIndex < endIndex)
					{
						auto& line = m_lines[currentLine];

						if (currentIndex == 0 && !concatenate)
						{
							withinSingleLineComment = false;
							withinPreproc = false;
							firstChar = true;
						}

						concatenate = false;

						if (!line.chars.empty())
						{
							//auto& g = line[currentIndex];
							auto c = line.chars[currentIndex];

							if (c != mLanguageDefinition.mPreprocChar && !isspace(c))
								firstChar = false;

							if (currentIndex == (int)line.chars.size() - 1 && line.chars[line.chars.size() - 1] == '\\')
								concatenate = true;

							bool inComment = (commentStartLine < currentLine || (commentStartLine == currentLine && commentStartIndex <= currentIndex));

							if (withinString)
							{
								line.attribs[currentIndex].mMultiLineComment = inComment;

								if (c == '\"')
								{
									if (currentIndex + 1 < (int)line.chars.size() && line.chars[currentIndex + 1] == '\"')
									{
										currentIndex += 1;
										if (currentIndex < (int)line.chars.size())
											line.attribs[currentIndex].mMultiLineComment = inComment;
									}
									else
										withinString = false;
								}
								else if (c == '\\')
								{
									currentIndex += 1;
									if (currentIndex < (int)line.chars.size())
										line.attribs[currentIndex].mMultiLineComment = inComment;
								}
							}
							else
							{
								if (firstChar && c == mLanguageDefinition.mPreprocChar)
									withinPreproc = true;

								if (c == '\"')
								{
									withinString = true;
									line.attribs[currentIndex].mMultiLineComment = inComment;
								}
								else
								{
									auto pred = [](const Char& a, const Char& b) { return a == b; };
									auto from = line.chars.begin() + currentIndex;
									auto& startStr = mLanguageDefinition.mCommentStart;
									auto& singleStartStr = mLanguageDefinition.mSingleLineComment;

									if (singleStartStr.size() > 0 &&
										currentIndex + singleStartStr.size() <= line.chars.size() &&
										equals(singleStartStr.begin(), singleStartStr.end(), from, from + singleStartStr.size(), pred))
									{
										withinSingleLineComment = true;
									}
									else if (!withinSingleLineComment && currentIndex + startStr.size() <= line.chars.size() &&
										equals(startStr.begin(), startStr.end(), from, from + startStr.size(), pred))
									{
										commentStartLine = currentLine;
										commentStartIndex = currentIndex;
									}

									inComment = inComment = (commentStartLine < currentLine || (commentStartLine == currentLine && commentStartIndex <= currentIndex));

									line.attribs[currentIndex].mMultiLineComment = inComment;

									line.attribs[currentIndex].mComment = withinSingleLineComment;

									auto& endStr = mLanguageDefinition.mCommentEnd;
									if (currentIndex + 1 >= (int)endStr.size() &&
										equals(endStr.begin(), endStr.end(), from + 1 - endStr.size(), from + 1, pred))
									{
										commentStartIndex = endIndex;
										commentStartLine = endLine;
									}
								}
							}
							line.attribs[currentIndex].mPreprocessor = withinPreproc;
							currentIndex += 1;// UTF8CharLength(c);
							if (currentIndex >= (int)line.chars.size())
							{
								currentIndex = 0;
								++currentLine;
							}
						}
						else
						{
							currentIndex = 0;
							++currentLine;
						}
					}
					m_CheckComments = false;
				}
			}

			void Colorize()
			{
				CheckComments();
				m_colorizeBeginLine = 0;
			}

			//void ColorizeRange(int begLineIdx, int endLineIdx)
			//{
			//	for (int i = begLineIdx; i < endLineIdx; i++)
			//		ColorizeLine(i);
			//}

			void ColorizeLine(int lineIdx)
			{
				if (m_lines.empty())
					return;

				std::wcmatch results;
				String id;

				auto& line = m_lines[lineIdx];
				const String& buffer = line.chars;

				if (line.chars.empty())
					return;
				for (size_t j = 0; j < line.chars.size(); ++j)
				{
					line.colors[j] = (uint16_t)PaletteIndex::Default;
				}

				const Char* bufferBegin = &buffer.front();
				const Char* bufferEnd = bufferBegin + buffer.size();

				auto last = bufferEnd;

				for (auto first = bufferBegin; first != last; )
				{
					const Char* token_begin = nullptr;
					const Char* token_end = nullptr;
					PaletteIndex token_color = PaletteIndex::Default;

					bool hasTokenizeResult = false;

					if (mLanguageDefinition.mTokenize != nullptr)
					{
						if (mLanguageDefinition.mTokenize(first, last, token_begin, token_end, token_color))
							hasTokenizeResult = true;
					}

					if (hasTokenizeResult == false)
					{
						// todo : remove
						//printf("using regex for %.*s\n", first + 10 < last ? 10 : int(last - first), first);

						for (auto& p : mRegexList)
						{
							if (std::regex_search(first, last, results, p.first, std::regex_constants::match_continuous))
							{
								hasTokenizeResult = true;

								auto& v = *results.begin();
								token_begin = v.first;
								token_end = v.second;
								token_color = p.second;
								break;
							}
						}
					}

					if (hasTokenizeResult == false)
					{
						first++;
					}
					else
					{
						const size_t token_length = token_end - token_begin;

						if (token_color == PaletteIndex::Identifier)
						{
							id.assign(token_begin, token_end);

							// todo : allmost all language definitions use lower case to specify keywords, so shouldn't this use ::tolower ?
							if (!mLanguageDefinition.mCaseSensitive)
								std::transform(id.begin(), id.end(), id.begin(), ::toupper);

							if (!line.attribs[first - bufferBegin].mPreprocessor)
							{
								if (mLanguageDefinition.mKeywords.count(id) != 0)
									token_color = PaletteIndex::Keyword;
								else if (mLanguageDefinition.mIdentifiers.count(id) != 0)
									token_color = PaletteIndex::KnownIdentifier;
								else if (mLanguageDefinition.mPreprocIdentifiers.count(id) != 0)
									token_color = PaletteIndex::PreprocIdentifier;
							}
							else
							{
								if (mLanguageDefinition.mPreprocIdentifiers.count(id) != 0)
									token_color = PaletteIndex::PreprocIdentifier;
							}
						}

						for (size_t j = 0; j < token_length; ++j)
							line.colors[(token_begin - bufferBegin) + j] = (uint16_t)token_color;

						first = token_end;
					}
				}
				for (int i = 0; i < line.chars.size(); i++)
				{
					if (line.attribs[i].mComment)
						line.colors[i] = (uint16_t)PaletteIndex::Comment;
					if (line.attribs[i].mMultiLineComment)
						line.colors[i] = (uint16_t)PaletteIndex::MultiLineComment;
				}
			}

			void _Frame_Colorize()
			{
				if (m_colorizeBeginLine < 0)
					m_colorizeBeginLine = 0;
				if (m_colorizeBeginLine < m_lines.size())
				{
					CheckComments();
					for (int i = 0; i < 5; i++)
					{
						if (m_colorizeBeginLine < m_lines.size())
							ColorizeLine(m_colorizeBeginLine++);
						else
							break;
					}
				}
			}



			Size _Get_Visual_Line_Range()
			{
				const int y_len = _Get_YLen(); //
				int min_one = 0;
				int max_one = m_lines.size();
				{
					int lower_bound = m_lines.size() - 1;
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
					int lower_bound = m_lines.size() - 1;
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
				min_one = math::clamp(min_one, 0, (int)m_lines.size());
				max_one = math::clamp(max_one, 0, (int)m_lines.size());
				return Size(min_one, max_one);
			}
			int _Get_YLen()
			{
				return FontSystem::calcFontLineHeight(getFont());
			}
		};



	}

}





namespace CraftEngine
{
	namespace gui
	{

		namespace _Detail
		{
			static bool TokenizeCStyleString(const Char* in_begin, const Char* in_end, const Char*& out_begin, const Char*& out_end)
			{
				const Char* p = in_begin;

				if (*p == '"')
				{
					p++;

					while (p < in_end)
					{
						// handle end of string
						if (*p == '"')
						{
							out_begin = in_begin;
							out_end = p + 1;
							return true;
						}

						// handle escape character for "
						if (*p == '\\' && p + 1 < in_end && p[1] == '"')
							p++;

						p++;
					}
				}

				return false;
			}

			static bool TokenizeCStyleCharacterLiteral(const Char* in_begin, const Char* in_end, const Char*& out_begin, const Char*& out_end)
			{
				const Char* p = in_begin;

				if (*p == '\'')
				{
					p++;

					// handle escape characters
					if (p < in_end && *p == '\\')
						p++;

					if (p < in_end)
						p++;

					// handle end of character literal
					if (p < in_end && *p == '\'')
					{
						out_begin = in_begin;
						out_end = p + 1;
						return true;
					}
				}

				return false;
			}

			static bool TokenizeCStyleIdentifier(const Char* in_begin, const Char* in_end, const Char*& out_begin, const Char*& out_end)
			{
				const Char* p = in_begin;

				if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
				{
					p++;

					while ((p < in_end) && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_'))
						p++;

					out_begin = in_begin;
					out_end = p;
					return true;
				}

				return false;
			}

			static bool TokenizeCStyleNumber(const Char* in_begin, const Char* in_end, const Char*& out_begin, const Char*& out_end)
			{
				const Char* p = in_begin;

				const bool startsWithNumber = *p >= '0' && *p <= '9';

				if (*p != '+' && *p != '-' && !startsWithNumber)
					return false;

				p++;

				bool hasNumber = startsWithNumber;

				while (p < in_end && (*p >= '0' && *p <= '9'))
				{
					hasNumber = true;

					p++;
				}

				if (hasNumber == false)
					return false;

				bool isFloat = false;
				bool isHex = false;
				bool isBinary = false;

				if (p < in_end)
				{
					if (*p == '.')
					{
						isFloat = true;

						p++;

						while (p < in_end && (*p >= '0' && *p <= '9'))
							p++;
					}
					else if (*p == 'x' || *p == 'X')
					{
						// hex formatted integer of the type 0xef80

						isHex = true;

						p++;

						while (p < in_end && ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')))
							p++;
					}
					else if (*p == 'b' || *p == 'B')
					{
						// binary formatted integer of the type 0b01011101

						isBinary = true;

						p++;

						while (p < in_end && (*p >= '0' && *p <= '1'))
							p++;
					}
				}

				if (isHex == false && isBinary == false)
				{
					// floating point exponent
					if (p < in_end && (*p == 'e' || *p == 'E'))
					{
						isFloat = true;

						p++;

						if (p < in_end && (*p == '+' || *p == '-'))
							p++;

						bool hasDigits = false;

						while (p < in_end && (*p >= '0' && *p <= '9'))
						{
							hasDigits = true;

							p++;
						}

						if (hasDigits == false)
							return false;
					}

					// single precision floating point type
					if (p < in_end && *p == 'f')
						p++;
				}

				if (isFloat == false)
				{
					// integer size type
					while (p < in_end && (*p == 'u' || *p == 'U' || *p == 'l' || *p == 'L'))
						p++;
				}

				out_begin = in_begin;
				out_end = p;
				return true;
			}

			static bool TokenizeCStylePunctuation(const Char* in_begin, const Char* in_end, const Char*& out_begin, const Char*& out_end)
			{
				(void)in_end;

				switch (*in_begin)
				{
				case '[':
				case ']':
				case '{':
				case '}':
				case '!':
				case '%':
				case '^':
				case '&':
				case '*':
				case '(':
				case ')':
				case '-':
				case '+':
				case '=':
				case '~':
				case '|':
				case '<':
				case '>':
				case '?':
				case ':':
				case '/':
				case ';':
				case ',':
				case '.':
					out_begin = in_begin;
					out_end = in_begin + 1;
					return true;
				}
				return false;
			}
		}


		const ColorTextEdit::LanguageDefinition& ColorTextEdit::LanguageDefinition::getLanguage(const std::string& name)
		{
			static LanguageDefinition langDef;
			auto copy = name;
			for (auto& it : copy)
				it = tolower(it);
			if (copy == "c++")
				return CPlusPlus();
			else if (copy == "c")
				return C();
			else if (copy == "glsl")
				return GLSL();
			else if (copy == "lua")
				return Lua();
			else if (copy == "hlsl")
				return HLSL();
			else if (copy == "sql")
				return SQL();
			else if (copy == "angelscript")
				return AngelScript();
			else
				return langDef;
		}


		const ColorTextEdit::LanguageDefinition& ColorTextEdit::LanguageDefinition::CPlusPlus()
		{
			static bool inited = false;
			static LanguageDefinition langDef;
			if (!inited)
			{
				static const char* const cppKeywords[] = {
					"alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept", "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char", "char16_t", "char32_t", "class",
					"compl", "concept", "const", "constexpr", "const_cast", "continue", "decltype", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false", "float",
					"for", "friend", "goto", "if", "import", "inline", "int", "long", "module", "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected", "public",
					"register", "reinterpret_cast", "requires", "return", "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "synchronized", "template", "this", "thread_local",
					"throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"
				};
				for (auto& k : cppKeywords)
					langDef.mKeywords.insert(StringTool::fromUtf8(k));

				static const char* const identifiers[] = {
					"abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh", "ctime", "div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit", "isgraph",
					"ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow", "printf", "sprintf", "snprintf", "putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand", "strcat", "strcmp", "strerror", "time", "tolower", "toupper",
					"std", "string", "vector", "map", "unordered_map", "set", "unordered_set", "min", "max"
				};
				for (auto& k : identifiers)
				{
					Identifier id;
					id.mDeclaration = "Built-in function";
					langDef.mIdentifiers.insert(std::make_pair(StringTool::fromUtf8(k), id));
				}

				langDef.mTokenize = [](const Char* in_begin, const Char* in_end, const Char*& out_begin, const Char*& out_end, PaletteIndex& paletteIndex) -> bool
				{
					paletteIndex = PaletteIndex::Max;

					while (in_begin < in_end && isascii(*in_begin) && isblank(*in_begin))
						in_begin++;

					if (in_begin == in_end)
					{
						out_begin = in_end;
						out_end = in_end;
						paletteIndex = PaletteIndex::Default;
					}
					else if (_Detail::TokenizeCStyleString(in_begin, in_end, out_begin, out_end))
						paletteIndex = PaletteIndex::String;
					else if (_Detail::TokenizeCStyleCharacterLiteral(in_begin, in_end, out_begin, out_end))
						paletteIndex = PaletteIndex::CharLiteral;
					else if (_Detail::TokenizeCStyleIdentifier(in_begin, in_end, out_begin, out_end))
						paletteIndex = PaletteIndex::Identifier;
					else if (_Detail::TokenizeCStyleNumber(in_begin, in_end, out_begin, out_end))
						paletteIndex = PaletteIndex::Number;
					else if (_Detail::TokenizeCStylePunctuation(in_begin, in_end, out_begin, out_end))
						paletteIndex = PaletteIndex::Punctuation;

					return paletteIndex != PaletteIndex::Max;
				};

				langDef.mCommentStart = L"/*";
				langDef.mCommentEnd = L"*/";
				langDef.mSingleLineComment = L"//";

				langDef.mCaseSensitive = true;
				langDef.mAutoIndentation = true;

				langDef.mName = L"C++";

				inited = true;
			}
			return langDef;
		}

		const ColorTextEdit::LanguageDefinition& ColorTextEdit::LanguageDefinition::HLSL()
		{
			static bool inited = false;
			static LanguageDefinition langDef;
			if (!inited)
			{
				static const char* const keywords[] = {
					"AppendStructuredBuffer", "asm", "asm_fragment", "BlendState", "bool", "break", "Buffer", "ByteAddressBuffer", "case", "cbuffer", "centroid", "class", "column_major", "compile", "compile_fragment",
					"CompileShader", "const", "continue", "ComputeShader", "ConsumeStructuredBuffer", "default", "DepthStencilState", "DepthStencilView", "discard", "do", "double", "DomainShader", "dword", "else",
					"export", "extern", "false", "float", "for", "fxgroup", "GeometryShader", "groupshared", "half", "Hullshader", "if", "in", "inline", "inout", "InputPatch", "int", "interface", "line", "lineadj",
					"linear", "LineStream", "matrix", "min16float", "min10float", "min16int", "min12int", "min16uint", "namespace", "nointerpolation", "noperspective", "NULL", "out", "OutputPatch", "packoffset",
					"pass", "pixelfragment", "PixelShader", "point", "PointStream", "precise", "RasterizerState", "RenderTargetView", "return", "register", "row_major", "RWBuffer", "RWByteAddressBuffer", "RWStructuredBuffer",
					"RWTexture1D", "RWTexture1DArray", "RWTexture2D", "RWTexture2DArray", "RWTexture3D", "sample", "sampler", "SamplerState", "SamplerComparisonState", "shared", "snorm", "stateblock", "stateblock_state",
					"static", "string", "struct", "switch", "StructuredBuffer", "tbuffer", "technique", "technique10", "technique11", "texture", "Texture1D", "Texture1DArray", "Texture2D", "Texture2DArray", "Texture2DMS",
					"Texture2DMSArray", "Texture3D", "TextureCube", "TextureCubeArray", "true", "typedef", "triangle", "triangleadj", "TriangleStream", "uint", "uniform", "unorm", "unsigned", "vector", "vertexfragment",
					"VertexShader", "void", "volatile", "while",
					"bool1","bool2","bool3","bool4","double1","double2","double3","double4", "float1", "float2", "float3", "float4", "int1", "int2", "int3", "int4", "in", "out", "inout",
					"uint1", "uint2", "uint3", "uint4", "dword1", "dword2", "dword3", "dword4", "half1", "half2", "half3", "half4",
					"float1x1","float2x1","float3x1","float4x1","float1x2","float2x2","float3x2","float4x2",
					"float1x3","float2x3","float3x3","float4x3","float1x4","float2x4","float3x4","float4x4",
					"half1x1","half2x1","half3x1","half4x1","half1x2","half2x2","half3x2","half4x2",
					"half1x3","half2x3","half3x3","half4x3","half1x4","half2x4","half3x4","half4x4",
				};
				for (auto& k : keywords)
					langDef.mKeywords.insert(StringTool::fromUtf8(k));

				static const char* const identifiers[] = {
					"abort", "abs", "acos", "all", "AllMemoryBarrier", "AllMemoryBarrierWithGroupSync", "any", "asdouble", "asfloat", "asin", "asint", "asint", "asuint",
					"asuint", "atan", "atan2", "ceil", "CheckAccessFullyMapped", "clamp", "clip", "cos", "cosh", "countbits", "cross", "D3DCOLORtoUBYTE4", "ddx",
					"ddx_coarse", "ddx_fine", "ddy", "ddy_coarse", "ddy_fine", "degrees", "determinant", "DeviceMemoryBarrier", "DeviceMemoryBarrierWithGroupSync",
					"distance", "dot", "dst", "errorf", "EvaluateAttributeAtCentroid", "EvaluateAttributeAtSample", "EvaluateAttributeSnapped", "exp", "exp2",
					"f16tof32", "f32tof16", "faceforward", "firstbithigh", "firstbitlow", "floor", "fma", "fmod", "frac", "frexp", "fwidth", "GetRenderTargetSampleCount",
					"GetRenderTargetSamplePosition", "GroupMemoryBarrier", "GroupMemoryBarrierWithGroupSync", "InterlockedAdd", "InterlockedAnd", "InterlockedCompareExchange",
					"InterlockedCompareStore", "InterlockedExchange", "InterlockedMax", "InterlockedMin", "InterlockedOr", "InterlockedXor", "isfinite", "isinf", "isnan",
					"ldexp", "length", "lerp", "lit", "log", "log10", "log2", "mad", "max", "min", "modf", "msad4", "mul", "noise", "normalize", "pow", "printf",
					"Process2DQuadTessFactorsAvg", "Process2DQuadTessFactorsMax", "Process2DQuadTessFactorsMin", "ProcessIsolineTessFactors", "ProcessQuadTessFactorsAvg",
					"ProcessQuadTessFactorsMax", "ProcessQuadTessFactorsMin", "ProcessTriTessFactorsAvg", "ProcessTriTessFactorsMax", "ProcessTriTessFactorsMin",
					"radians", "rcp", "reflect", "refract", "reversebits", "round", "rsqrt", "saturate", "sign", "sin", "sincos", "sinh", "smoothstep", "sqrt", "step",
					"tan", "tanh", "tex1D", "tex1D", "tex1Dbias", "tex1Dgrad", "tex1Dlod", "tex1Dproj", "tex2D", "tex2D", "tex2Dbias", "tex2Dgrad", "tex2Dlod", "tex2Dproj",
					"tex3D", "tex3D", "tex3Dbias", "tex3Dgrad", "tex3Dlod", "tex3Dproj", "texCUBE", "texCUBE", "texCUBEbias", "texCUBEgrad", "texCUBElod", "texCUBEproj", "transpose", "trunc"
				};
				for (auto& k : identifiers)
				{
					Identifier id;
					id.mDeclaration = "Built-in function";
					langDef.mIdentifiers.insert(std::make_pair(StringTool::fromUtf8(k), id));
				}

				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[ \\t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

				langDef.mCommentStart = L"/*";
				langDef.mCommentEnd = L"*/";
				langDef.mSingleLineComment = L"//";

				langDef.mCaseSensitive = true;
				langDef.mAutoIndentation = true;

				langDef.mName = L"HLSL";

				inited = true;
			}
			return langDef;
		}

		const ColorTextEdit::LanguageDefinition& ColorTextEdit::LanguageDefinition::GLSL()
		{
			static bool inited = false;
			static LanguageDefinition langDef;
			if (!inited)
			{
				static const char* const keywords[] = {
					"break", "case", "char", "const", "continue", "default", "do", "else", "for", "if", "return",
					"struct", "switch", "while", "true", "false", "const",
					"in","out","attribute","uniform","varying","buffer","shared","centroid","sample","patch",
					"layout","packed","row_major","column_major","binding","offset","align","set","push_constant","input_attachment_index","location","component","index",
					"local_size_x","local_size_y","local_size_z","constant_id",
				};
				for (auto& k : keywords)
					langDef.mKeywords.insert(StringTool::fromUtf8(k));

				static const char* const base_types[] = {
					"int","uint","float","double","bool","void",
					"vec2 ","vec3","vec4", "mat2","mat3","mat4",
					"dvec2 ","dvec3","dvec4", "dmat2","dmat3","dmat4",
					"bvec2 ","bvec3","bvec4", "mat2x2","mat3x3","mat4x4",
					"ivec2 ","ivec3","ivec4", "dmat2x2","dmat3x3","dmat4x4",
					"uvec2 ","uvec3","uvec4",
					"mat2x3","mat3x2","mat2x4","mat4x2","mat3x4","mat4x3",
					"dmat2x3","dmat3x2","dmat2x4","dmat4x2","dmat3x4","dmat4x3",
					"atomic_uint",
					"sampler", "samplerShadow",
					"sampler1D","texture1D","image1D","sampler1DShadow","sampler1DArray","texture1DArray","image1DArray","sampler1DArrayShadow",
					"sampler2D","texture2D","image2D","sampler2DShadow","sampler2DArray","texture2DArray","image2DArray","sampler2DArrayShadow",
					"sampler3D","texture3D","image3D","samplerCube","textureCube","imageCube","samplerCubeShadow","samplerCubeArray","textureCubeArray","imageCubeArray","samplerCubeArrayShadow",
					"isampler1D","itexture1D","iimage1D","isampler1DArray","itexture1DArray","iimage1DArray",
					"isampler2D","itexture2D","iimage2D","isampler2DArray","itexture2DArray","iimage2DArray",
					"isampler3D","itexture3D","iimage3D","isamplerCube","itextureCube","iimageCube","isamplerCubeArray","itextureCubeArray","iimageCubeArray",
					"usampler1D","utexture1D","uimage1D","usampler1DArray","utexture1DArray","uimage1DArray",
					"usampler2D","utexture2D","uimage2D","usampler2DArray","utexture2DArray","uimage2DArray",
					"usampler3D","utexture3D","uimage3D","usamplerCube","utextureCube","uimageCube","usamplerCubeArray","utextureCubeArray","iimageCubeArray",

				};
				for (auto& t : base_types)
					langDef.mKeywords.insert(StringTool::fromUtf8(t));


				static const char* const identifiers[] = {
					"abs", "acos", "asin", "atan", "ceil", "clock", "cosh", "ctime", "div", "exit", "fabs", "floor", "fmod",
					"log10", "log2", "log", "modf", "pow", "sinh", "sqrt", "cos", "sin", "tan"

				};
				for (auto& k : identifiers)
				{
					Identifier id;
					id.mDeclaration = "Built-in function";
					langDef.mIdentifiers.insert(std::make_pair(StringTool::fromUtf8(k), id));
				}

				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[ \\t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));//
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

				langDef.mCommentStart = L"/*";
				langDef.mCommentEnd = L"*/";
				langDef.mSingleLineComment = L"//";

				langDef.mCaseSensitive = true;
				langDef.mAutoIndentation = true;

				langDef.mName = L"GLSL";

				inited = true;
			}
			return langDef;
		}

		const ColorTextEdit::LanguageDefinition& ColorTextEdit::LanguageDefinition::C()
		{
			static bool inited = false;
			static LanguageDefinition langDef;
			if (!inited)
			{
				static const char* const keywords[] = {
					"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict", "return", "short",
					"signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary",
					"_Noreturn", "_Static_assert", "_Thread_local"
				};
				for (auto& k : keywords)
					langDef.mKeywords.insert(StringTool::fromUtf8(k));

				static const char* const identifiers[] = {
					"abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh", "ctime", "div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit", "isgraph",
					"ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow", "putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand", "strcat", "strcmp", "strerror", "time", "tolower", "toupper"
				};
				for (auto& k : identifiers)
				{
					Identifier id;
					id.mDeclaration = "Built-in function";
					langDef.mIdentifiers.insert(std::make_pair(StringTool::fromUtf8(k), id));
				}

				langDef.mTokenize = [](const Char* in_begin, const Char* in_end, const Char*& out_begin, const Char*& out_end, PaletteIndex& paletteIndex) -> bool
				{
					paletteIndex = PaletteIndex::Max;

					while (in_begin < in_end && isascii(*in_begin) && isblank(*in_begin))
						in_begin++;

					if (in_begin == in_end)
					{
						out_begin = in_end;
						out_end = in_end;
						paletteIndex = PaletteIndex::Default;
					}
					else if (_Detail::TokenizeCStyleString(in_begin, in_end, out_begin, out_end))
						paletteIndex = PaletteIndex::String;
					else if (_Detail::TokenizeCStyleCharacterLiteral(in_begin, in_end, out_begin, out_end))
						paletteIndex = PaletteIndex::CharLiteral;
					else if (_Detail::TokenizeCStyleIdentifier(in_begin, in_end, out_begin, out_end))
						paletteIndex = PaletteIndex::Identifier;
					else if (_Detail::TokenizeCStyleNumber(in_begin, in_end, out_begin, out_end))
						paletteIndex = PaletteIndex::Number;
					else if (_Detail::TokenizeCStylePunctuation(in_begin, in_end, out_begin, out_end))
						paletteIndex = PaletteIndex::Punctuation;

					return paletteIndex != PaletteIndex::Max;
				};

				langDef.mCommentStart = L"/*";
				langDef.mCommentEnd = L"*/";
				langDef.mSingleLineComment = L"//";

				langDef.mCaseSensitive = true;
				langDef.mAutoIndentation = true;

				langDef.mName = L"C";

				inited = true;
			}
			return langDef;
		}

		const ColorTextEdit::LanguageDefinition& ColorTextEdit::LanguageDefinition::SQL()
		{
			static bool inited = false;
			static LanguageDefinition langDef;
			if (!inited)
			{
				static const char* const keywords[] = {
					"ADD", "EXCEPT", "PERCENT", "ALL", "EXEC", "PLAN", "ALTER", "EXECUTE", "PRECISION", "AND", "EXISTS", "PRIMARY", "ANY", "EXIT", "PRINT", "AS", "FETCH", "PROC", "ASC", "FILE", "PROCEDURE",
					"AUTHORIZATION", "FILLFACTOR", "PUBLIC", "BACKUP", "FOR", "RAISERROR", "BEGIN", "FOREIGN", "READ", "BETWEEN", "FREETEXT", "READTEXT", "BREAK", "FREETEXTTABLE", "RECONFIGURE",
					"BROWSE", "FROM", "REFERENCES", "BULK", "FULL", "REPLICATION", "BY", "FUNCTION", "RESTORE", "CASCADE", "GOTO", "RESTRICT", "CASE", "GRANT", "RETURN", "CHECK", "GROUP", "REVOKE",
					"CHECKPOINT", "HAVING", "RIGHT", "CLOSE", "HOLDLOCK", "ROLLBACK", "CLUSTERED", "IDENTITY", "ROWCOUNT", "COALESCE", "IDENTITY_INSERT", "ROWGUIDCOL", "COLLATE", "IDENTITYCOL", "RULE",
					"COLUMN", "IF", "SAVE", "COMMIT", "IN", "SCHEMA", "COMPUTE", "INDEX", "SELECT", "CONSTRAINT", "INNER", "SESSION_USER", "CONTAINS", "INSERT", "SET", "CONTAINSTABLE", "INTERSECT", "SETUSER",
					"CONTINUE", "INTO", "SHUTDOWN", "CONVERT", "IS", "SOME", "CREATE", "JOIN", "STATISTICS", "CROSS", "KEY", "SYSTEM_USER", "CURRENT", "KILL", "TABLE", "CURRENT_DATE", "LEFT", "TEXTSIZE",
					"CURRENT_TIME", "LIKE", "THEN", "CURRENT_TIMESTAMP", "LINENO", "TO", "CURRENT_USER", "LOAD", "TOP", "CURSOR", "NATIONAL", "TRAN", "DATABASE", "NOCHECK", "TRANSACTION",
					"DBCC", "NONCLUSTERED", "TRIGGER", "DEALLOCATE", "NOT", "TRUNCATE", "DECLARE", "NULL", "TSEQUAL", "DEFAULT", "NULLIF", "UNION", "DELETE", "OF", "UNIQUE", "DENY", "OFF", "UPDATE",
					"DESC", "OFFSETS", "UPDATETEXT", "DISK", "ON", "USE", "DISTINCT", "OPEN", "USER", "DISTRIBUTED", "OPENDATASOURCE", "VALUES", "DOUBLE", "OPENQUERY", "VARYING","DROP", "OPENROWSET", "VIEW",
					"DUMMY", "OPENXML", "WAITFOR", "DUMP", "OPTION", "WHEN", "ELSE", "OR", "WHERE", "END", "ORDER", "WHILE", "ERRLVL", "OUTER", "WITH", "ESCAPE", "OVER", "WRITETEXT"
				};

				for (auto& k : keywords)
					langDef.mKeywords.insert(StringTool::fromUtf8(k));

				static const char* const identifiers[] = {
					"ABS",  "ACOS",  "ADD_MONTHS",  "ASCII",  "ASCIISTR",  "ASIN",  "ATAN",  "ATAN2",  "AVG",  "BFILENAME",  "BIN_TO_NUM",  "BITAND",  "CARDINALITY",  "CASE",  "CAST",  "CEIL",
					"CHARTOROWID",  "CHR",  "COALESCE",  "COMPOSE",  "CONCAT",  "CONVERT",  "CORR",  "COS",  "COSH",  "COUNT",  "COVAR_POP",  "COVAR_SAMP",  "CUME_DIST",  "CURRENT_DATE",
					"CURRENT_TIMESTAMP",  "DBTIMEZONE",  "DECODE",  "DECOMPOSE",  "DENSE_RANK",  "DUMP",  "EMPTY_BLOB",  "EMPTY_CLOB",  "EXP",  "EXTRACT",  "FIRST_VALUE",  "FLOOR",  "FROM_TZ",  "GREATEST",
					"GROUP_ID",  "HEXTORAW",  "INITCAP",  "INSTR",  "INSTR2",  "INSTR4",  "INSTRB",  "INSTRC",  "LAG",  "LAST_DAY",  "LAST_VALUE",  "LEAD",  "LEAST",  "LENGTH",  "LENGTH2",  "LENGTH4",
					"LENGTHB",  "LENGTHC",  "LISTAGG",  "LN",  "LNNVL",  "LOCALTIMESTAMP",  "LOG",  "LOWER",  "LPAD",  "LTRIM",  "MAX",  "MEDIAN",  "MIN",  "MOD",  "MONTHS_BETWEEN",  "NANVL",  "NCHR",
					"NEW_TIME",  "NEXT_DAY",  "NTH_VALUE",  "NULLIF",  "NUMTODSINTERVAL",  "NUMTOYMINTERVAL",  "NVL",  "NVL2",  "POWER",  "RANK",  "RAWTOHEX",  "REGEXP_COUNT",  "REGEXP_INSTR",
					"REGEXP_REPLACE",  "REGEXP_SUBSTR",  "REMAINDER",  "REPLACE",  "ROUND",  "ROWNUM",  "RPAD",  "RTRIM",  "SESSIONTIMEZONE",  "SIGN",  "SIN",  "SINH",
					"SOUNDEX",  "SQRT",  "STDDEV",  "SUBSTR",  "SUM",  "SYS_CONTEXT",  "SYSDATE",  "SYSTIMESTAMP",  "TAN",  "TANH",  "TO_CHAR",  "TO_CLOB",  "TO_DATE",  "TO_DSINTERVAL",  "TO_LOB",
					"TO_MULTI_BYTE",  "TO_NCLOB",  "TO_NUMBER",  "TO_SINGLE_BYTE",  "TO_TIMESTAMP",  "TO_TIMESTAMP_TZ",  "TO_YMINTERVAL",  "TRANSLATE",  "TRIM",  "TRUNC", "TZ_OFFSET",  "UID",  "UPPER",
					"USER",  "USERENV",  "VAR_POP",  "VAR_SAMP",  "VARIANCE",  "VSIZE "
				};
				for (auto& k : identifiers)
				{
					Identifier id;
					id.mDeclaration = "Built-in function";
					langDef.mIdentifiers.insert(std::make_pair(StringTool::fromUtf8(k), id));
				}

				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"\\\'[^\\\']*\\\'", PaletteIndex::String));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

				langDef.mCommentStart = L"/*";
				langDef.mCommentEnd = L"*/";
				langDef.mSingleLineComment = L"//";

				langDef.mCaseSensitive = false;
				langDef.mAutoIndentation = false;

				langDef.mName = L"SQL";

				inited = true;
			}
			return langDef;
		}

		const ColorTextEdit::LanguageDefinition& ColorTextEdit::LanguageDefinition::AngelScript()
		{
			static bool inited = false;
			static LanguageDefinition langDef;
			if (!inited)
			{
				static const char* const keywords[] = {
					"and", "abstract", "auto", "bool", "break", "case", "cast", "class", "const", "continue", "default", "do", "double", "else", "enum", "false", "final", "float", "for",
					"from", "funcdef", "function", "get", "if", "import", "in", "inout", "int", "interface", "int8", "int16", "int32", "int64", "is", "mixin", "namespace", "not",
					"null", "or", "out", "override", "private", "protected", "return", "set", "shared", "super", "switch", "this ", "true", "typedef", "uint", "uint8", "uint16", "uint32",
					"uint64", "void", "while", "xor"
				};

				for (auto& k : keywords)
					langDef.mKeywords.insert(StringTool::fromUtf8(k));

				static const char* const identifiers[] = {
					"cos", "sin", "tab", "acos", "asin", "atan", "atan2", "cosh", "sinh", "tanh", "log", "log10", "pow", "sqrt", "abs", "ceil", "floor", "fraction", "closeTo", "fpFromIEEE", "fpToIEEE",
					"complex", "opEquals", "opAddAssign", "opSubAssign", "opMulAssign", "opDivAssign", "opAdd", "opSub", "opMul", "opDiv"
				};
				for (auto& k : identifiers)
				{
					Identifier id;
					id.mDeclaration = "Built-in function";
					langDef.mIdentifiers.insert(std::make_pair(StringTool::fromUtf8(k), id));
				}

				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"\\'\\\\?[^\\']\\'", PaletteIndex::String));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

				langDef.mCommentStart = L"/*";
				langDef.mCommentEnd = L"*/";
				langDef.mSingleLineComment = L"//";

				langDef.mCaseSensitive = true;
				langDef.mAutoIndentation = true;

				langDef.mName = L"AngelScript";

				inited = true;
			}
			return langDef;
		}

		const ColorTextEdit::LanguageDefinition& ColorTextEdit::LanguageDefinition::Lua()
		{
			static bool inited = false;
			static LanguageDefinition langDef;
			if (!inited)
			{
				static const char* const keywords[] = {
					"and", "break", "do", "", "else", "elseif", "end", "false", "for", "function", "if", "in", "", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while"
				};

				for (auto& k : keywords)
					langDef.mKeywords.insert(StringTool::fromUtf8(k));

				static const char* const identifiers[] = {
					"assert", "collectgarbage", "dofile", "error", "getmetatable", "ipairs", "loadfile", "load", "loadstring",  "next",  "pairs",  "pcall",  "print",  "rawequal",  "rawlen",  "rawget",  "rawset",
					"select",  "setmetatable",  "tonumber",  "tostring",  "type",  "xpcall",  "_G",  "_VERSION","arshift", "band", "bnot", "bor", "bxor", "btest", "extract", "lrotate", "lshift", "replace",
					"rrotate", "rshift", "create", "resume", "running", "status", "wrap", "yield", "isyieldable", "debug","getuservalue", "gethook", "getinfo", "getlocal", "getregistry", "getmetatable",
					"getupvalue", "upvaluejoin", "upvalueid", "setuservalue", "sethook", "setlocal", "setmetatable", "setupvalue", "traceback", "close", "flush", "input", "lines", "open", "output", "popen",
					"read", "tmpfile", "type", "write", "close", "flush", "lines", "read", "seek", "setvbuf", "write", "__gc", "__tostring", "abs", "acos", "asin", "atan", "ceil", "cos", "deg", "exp", "tointeger",
					"floor", "fmod", "ult", "log", "max", "min", "modf", "rad", "random", "randomseed", "sin", "sqrt", "string", "tan", "type", "atan2", "cosh", "sinh", "tanh",
					"pow", "frexp", "ldexp", "log10", "pi", "huge", "maxinteger", "mininteger", "loadlib", "searchpath", "seeall", "preload", "cpath", "path", "searchers", "loaded", "module", "require", "clock",
					"date", "difftime", "execute", "exit", "getenv", "remove", "rename", "setlocale", "time", "tmpname", "byte", "char", "dump", "find", "format", "gmatch", "gsub", "len", "lower", "match", "rep",
					"reverse", "sub", "upper", "pack", "packsize", "unpack", "concat", "maxn", "insert", "pack", "unpack", "remove", "move", "sort", "offset", "codepoint", "char", "len", "codes", "charpattern",
					"coroutine", "table", "io", "os", "string", "utf8", "bit32", "math", "debug", "package"
				};
				for (auto& k : identifiers)
				{
					Identifier id;
					id.mDeclaration = "Built-in function";
					langDef.mIdentifiers.insert(std::make_pair(StringTool::fromUtf8(k), id));
				}

				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"\\\'[^\\\']*\\\'", PaletteIndex::String));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
				langDef.mTokenRegexStrings.push_back(std::make_pair<String, PaletteIndex>(L"[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

				langDef.mCommentStart = L"--[[";
				langDef.mCommentEnd = L"]]";
				langDef.mSingleLineComment = L"--";

				langDef.mCaseSensitive = true;
				langDef.mAutoIndentation = false;

				langDef.mName = L"Lua";

				inited = true;
			}
			return langDef;
		}




		void ColorTextEdit::setTextPaletteStyle(int style)
		{
			const static uint32_t dark_palette[] = {
					0xff7f7f7f,	// Default
					0xffd69c56,	// Keyword	
					0xff00ff00,	// Number
					0xff7070e0,	// String
					0xff70a0e0, // Char literal
					0xffffffff, // Punctuation
					0xff408080,	// Preprocessor
					0xffaaaaaa, // Identifier
					0xff9bc64d, // Known identifier
					0xffc040a0, // Preproc identifier
					0xff206020, // Comment (single line)
					0xff406020, // Comment (multi line)
					0xff101010, // Background
					0xffe0e0e0, // Cursor
					0x80a06020, // Selection
					0x800020ff, // ErrorMarker
					0x40f08000, // Breakpoint
					0xff707000, // Line number
					0x40000000, // Current line fill
					0x40808080, // Current line fill (inactive)
					0x40a0a0a0, // Current line edge
			};
			const static uint32_t light_palette[] = {
					0xff7f7f7f,	// None
					0xffff0c06,	// Keyword	
					0xff008000,	// Number
					0xff2020a0,	// String
					0xff304070, // Char literal
					0xff000000, // Punctuation
					0xff406060,	// Preprocessor
					0xff404040, // Identifier
					0xff606010, // Known identifier
					0xffc040a0, // Preproc identifier
					0xff205020, // Comment (single line)
					0xff405020, // Comment (multi line)
					0xffffffff, // Background
					0xff000000, // Cursor
					0x80600000, // Selection
					0xa00010ff, // ErrorMarker
					0x80f08000, // Breakpoint
					0xff505000, // Line number
					0x40000000, // Current line fill
					0x40808080, // Current line fill (inactive)
					0x40000000, // Current line edge
			};
			const static uint32_t retroBlue_palette[] = {
					0xff00ffff,	// None
					0xffffff00,	// Keyword	
					0xff00ff00,	// Number
					0xff808000,	// String
					0xff808000, // Char literal
					0xffffffff, // Punctuation
					0xff008000,	// Preprocessor
					0xff00ffff, // Identifier
					0xffffffff, // Known identifier
					0xffff00ff, // Preproc identifier
					0xff808080, // Comment (single line)
					0xff404040, // Comment (multi line)
					0xff800000, // Background
					0xff0080ff, // Cursor
					0x80ffff00, // Selection
					0xa00000ff, // ErrorMarker
					0x80ff8000, // Breakpoint
					0xff808000, // Line number
					0x40000000, // Current line fill
					0x40808080, // Current line fill (inactive)
					0x40000000, // Current line edge
			};
			uint32_t const* palette = dark_palette;
			switch (style)
			{
			case GuiColorStyle::ColorStyle::eDark:palette = dark_palette; break;
			case GuiColorStyle::ColorStyle::eLight:palette = light_palette; break;
			case GuiColorStyle::ColorStyle::eRed:palette = retroBlue_palette; break;
			}
			for (int i = 0; i < (int)PaletteIndex::Max; i++)
				m_textPalette[i] = *(Color*)(&palette[i]);
		}




	}
}