#pragma once
#include "./Widget.h"

namespace CraftEngine
{
	namespace gui
	{

		class MenuWidget :public Widget
		{
		private:

			enum ElemTypeEnum
			{
				eAction,
				eCheckAction,
				eSeparator,
				eMenu,
			};
			struct ElemRenderData
			{
				bool mEnable = true;
				Rect mAreaRect;
				ElemTypeEnum mType;
				String mText;
				Point mBasepoint;
				// eAction
				Action* mAction;
				Rect mIconRect;
				// eCheckAction
				Rect mCheckboxRect;
				bool mCheck = false;
				// eSeparator
				Rect mSeparatorRect;
				// eMenu
				MenuWidget* mSubMenu;
				Rect mSubMenuTipRect;
			};
			Rect mTextAreaRect;
			HandleImage m_checkboxImage;
			HandleImage m_submenuTipImage;
			int m_curIndex = -1;
			int m_tempIndex = -2;
			bool m_isSubMenu = false;
			Size m_minSize;
			bool m_isMenuShow = false;
			core::ArrayList<ElemRenderData> m_elemList;

			MenuWidget(bool unused, MenuWidget* p) :MenuWidget(p)
			{
				m_isSubMenu = true;
				_Auto_Adapt_Size();
			}

		public:

			MenuWidget(Widget* p) :Widget(p)
			{
				m_checkboxImage = GuiAssetsManager::loadImage("Tick");
				m_submenuTipImage = GuiAssetsManager::loadImage("Triangle_Right");
				setClickable(true);
				setPalette(GuiColorStyle::getSytle(GuiColorStyle::WidgetType::eMenu));
				_Auto_Adapt_Size();
			}

			void unpopupMenu()
			{
				auto root = getRootWidget();
				if (!this->isNowPopup())
					return;
				root->unpopupWidget(this);
			}

			void popupMenu(const Point globalPoint)
			{
				auto root = getRootWidget();
				auto base_pos = getParentWidget()->getLocalPos(globalPoint);
				auto base_offset = Offset(0);
				auto root_rect = root->getRect();
				auto child_rect = Rect(globalPoint, getSize());
				if (child_rect.bottom() > root_rect.bottom())
					base_offset.y = root_rect.bottom() - child_rect.bottom();
				else if (child_rect.top() < root_rect.top())
					base_offset.y = root_rect.top() - child_rect.top();
				if (child_rect.right() > root_rect.right())
					base_offset.x = -child_rect.mWidth;
				setPos(base_pos + base_offset);
				this->setPopupable(true);
				if (this->isNowPopup())
					return;
				root->popupWidget(this);
			}

			void setElemEnable(int index, bool enable)
			{
				m_elemList[index].mEnable = enable;
			}
			void setElemCheck(int index, bool check)
			{
				m_elemList[index].mCheck = check;
			}
			void setElemAction(int index, Action* action)
			{
				m_elemList[index].mAction = action;
			}
			void setElemName(int index, const String& name)
			{
				m_elemList[index].mText = name;
			}

			void addAction(Action* action, bool enable = true)
			{
				ElemRenderData adata;
				adata.mType = eAction;
				adata.mAction = action;
				adata.mText = action->getText();
				adata.mEnable = enable;
				m_elemList.emplace_back(std::move(adata));
				_Auto_Adapt_Size();
			}
			void addCheckAction(Action* action, bool check = false, bool enable = true)
			{
				ElemRenderData adata;
				adata.mType = eCheckAction;
				adata.mAction = action;
				adata.mText = action->getText();
				adata.mCheck = check;
				adata.mEnable = enable;
				m_elemList.emplace_back(std::move(adata));
				_Auto_Adapt_Size();
			}
			MenuWidget* addMenu(const String& name, bool enable = true)
			{
				ElemRenderData adata;
				adata.mType = eMenu;
				adata.mText = name;
				adata.mSubMenu = new MenuWidget(true, this);
				adata.mEnable = enable;
				auto menu = adata.mSubMenu;
				m_elemList.emplace_back(std::move(adata));		
				_Auto_Adapt_Size();
				return menu;
			}
			void addSeparator()
			{
				ElemRenderData adata;
				adata.mType = eSeparator;
				m_elemList.emplace_back(std::move(adata));
				_Auto_Adapt_Size();
			}

			virtual Size getMinSizeLimit()const override
			{
				return m_minSize;
			}

			virtual void onPaintEvent() override
			{
				auto painter = getPainter();
				drawBackground();
				painter.drawRect(Rect(mTextAreaRect.mOffset, mTextAreaRect.mSize),
					getPalette().mBackgroundHighLightColor);
				
				if (m_curIndex >= 0)
					painter.drawRect(Rect(
						m_elemList[m_curIndex].mAreaRect.mOffset, 
						m_elemList[m_curIndex].mAreaRect.mSize),
						getPalette().mBackgroundFocusColor);

				for (auto it: m_elemList)
				{
					if (it.mEnable)
					{
						switch (it.mType)
						{
						case eSeparator:
						{
							painter.drawRect(it.mSeparatorRect, getPalette().mFrameColor);
							break;
						}
						case eAction:
						{
							painter.drawTextLine(it.mText, it.mBasepoint, getFont(), getPalette().mForegroundColor);
							if (it.mAction->getIcon() != nullptr)
								painter.drawRect(it.mIconRect, it.mAction->getIcon());
							break;
						}
						case eCheckAction:
						{
							painter.drawTextLine(it.mText, it.mBasepoint, getFont(), getPalette().mForegroundColor);
							if (it.mCheck)
							{
								painter.drawSDF(it.mCheckboxRect, getPalette().mForegroundColor, m_checkboxImage);
								painter.drawRectFrame(it.mCheckboxRect, getPalette().mForegroundColor);
							}
							break;
						}
						case eMenu:
						{
							painter.drawTextLine(it.mText, it.mBasepoint, getFont(), getPalette().mForegroundColor);
							if(it.mSubMenu->m_isMenuShow)
								painter.drawSDF(Rect(it.mSubMenuTipRect.mOffset, it.mSubMenuTipRect.mSize),
									getPalette().mForegroundColor, m_submenuTipImage);
							else
								painter.drawSDF(Rect(it.mSubMenuTipRect.mOffset, it.mSubMenuTipRect.mSize),
									getPalette().mForegroundFocusColor, m_submenuTipImage);
							break;
						}
						}
					}
					else
					{
						switch (it.mType)
						{
						case eSeparator:
						{
							painter.drawRect(it.mSeparatorRect, getPalette().mBackgroundDisableColor);
							break;
						}
						case eAction:
						{
							painter.drawTextLine(it.mText, it.mBasepoint, getFont(), getPalette().mForegroundDisableColor);
							if (it.mAction->getIcon() != nullptr)
								painter.drawRect(it.mIconRect, it.mAction->getIcon());
							break;
						}
						case eCheckAction:
						{
							painter.drawTextLine(it.mText, it.mBasepoint, getFont(), getPalette().mForegroundDisableColor);
							if (it.mCheck)
							{
								painter.drawSDF(it.mCheckboxRect, getPalette().mForegroundDisableColor, m_checkboxImage);
								painter.drawRectFrame(it.mCheckboxRect, getPalette().mFrameColor);
							}
							break;
						}
						case eMenu:
						{
							painter.drawTextLine(it.mText, it.mBasepoint, getFont(), getPalette().mForegroundDisableColor);
							painter.drawSDF(Rect(it.mSubMenuTipRect.mOffset, it.mSubMenuTipRect.mSize),
								getPalette().mForegroundDisableColor, m_submenuTipImage);
							break;
						}
						}
					}
				}

				if (m_curIndex >= 0 && m_elemList[m_curIndex].mType == eMenu && m_elemList[m_curIndex].mSubMenu->m_isMenuShow)
					drawChild(m_elemList[m_curIndex].mSubMenu);

				drawFrame();
			}

		protected:
			virtual Widget* onDetect(const MouseEvent& mouseEvent)
			{
				if (isHide() || isTransparent())
					return nullptr;
				if ((isPopupable() && !isNowPopup()))
					return nullptr;
				if (m_isSubMenu && !m_isMenuShow)
					return nullptr;
				MouseEvent subEvent = mouseEvent;
				Widget* widget = nullptr;
				if (m_curIndex >= 0 && m_elemList[m_curIndex].mType == eMenu)
				{
					subEvent.local = getLocalPos(subEvent.global, m_elemList[m_curIndex].mSubMenu->getGroupid());
					widget = m_elemList[m_curIndex].mSubMenu->onMouseEvent(subEvent);
					if (widget != nullptr)
						return widget;
				}
				if (getRect().inside(mouseEvent.local))
					return this;
				return nullptr;
			}

		protected:
			virtual void onCallMenu(const MouseEvent& mouseEvent) override final {}

			virtual void onMoveIn(const MouseEvent& mouseEvent) override
			{
				auto local_pos = getLocalPos(mouseEvent.global);
				if (m_curIndex >= 0 && m_elemList[m_curIndex].mType == eMenu)
				{
					m_elemList[m_curIndex].mSubMenu->m_isMenuShow = false;
				}
				m_curIndex = _Find_Index(local_pos.y);
				// _Clear_State
				_Clear_State();
				if (m_curIndex >= 0 && m_elemList[m_curIndex].mType == eMenu)
				{
					auto base_pos = Point(getWidth(), m_elemList[m_curIndex].mAreaRect.top());
					auto base_offset = Offset(0);
					auto root = getRootWidget();
					auto root_rect = root->getRect();
					auto pos = getGlobalPos(Point(0) + base_pos);
					auto child_rect = Rect(pos, m_elemList[m_curIndex].mSubMenu->getSize());
					if (child_rect.bottom() > root_rect.bottom())
						base_offset.y = root_rect.bottom() - child_rect.bottom();
					else if (child_rect.top() < root_rect.top())
						base_offset.y = root_rect.top() - child_rect.top();
					if (child_rect.right() > root_rect.right())
						base_offset.x = -getWidth() - child_rect.mWidth;
					//else if (child_rect.top() < root_rect.top())
					//	base_offset.y = root_rect.top() - child_rect.top();
					m_elemList[m_curIndex].mSubMenu->setPos(base_pos + base_offset);
					m_elemList[m_curIndex].mSubMenu->m_isMenuShow = true;
				}
				MenuWidget* menu = this;
				while (menu->m_isSubMenu)
				{
					menu->m_isMenuShow = true;
					menu = (MenuWidget*)menu->getParentWidget();
				}
			}
			virtual void onMoveOut(const MouseEvent& mouseEvent) override
			{ 
				if (m_curIndex >= 0 && m_elemList[m_curIndex].mType == eMenu)
				{

				}
				else
				{
					m_curIndex = -1;
				}
				//m_curIndex = -1;
			}
			virtual void onMoving(const MouseEvent& mouseEvent) override
			{
				auto local_pos = getLocalPos(mouseEvent.global);
				if (m_curIndex >= 0 && m_elemList[m_curIndex].mType == eMenu)
				{
					m_elemList[m_curIndex].mSubMenu->m_isMenuShow = false;
				}
				m_curIndex = _Find_Index(local_pos.y);
				if (m_curIndex >= 0 && m_elemList[m_curIndex].mType == eMenu)
				{
					auto base_pos = Point(getWidth(), m_elemList[m_curIndex].mAreaRect.top());
					auto base_offset = Offset(0);
					auto root = getRootWidget();
					auto root_rect = root->getRect();
					auto pos = getGlobalPos(Point(0) + base_pos);
					auto child_rect = Rect(pos, m_elemList[m_curIndex].mSubMenu->getSize());
					if (child_rect.bottom() > root_rect.bottom())
						base_offset.y = root_rect.bottom() - child_rect.bottom();
					else if (child_rect.top() < root_rect.top())
						base_offset.y = root_rect.top() - child_rect.top();
					if (child_rect.right() > root_rect.right())
						base_offset.x = -getWidth() - child_rect.mWidth;
					//else if (child_rect.top() < root_rect.top())
					//	base_offset.y = root_rect.top() - child_rect.top();
					m_elemList[m_curIndex].mSubMenu->setPos(base_pos + base_offset);
					m_elemList[m_curIndex].mSubMenu->m_isMenuShow = true;
				}
				MenuWidget* menu = this;
				while (menu->m_isSubMenu)
				{
					menu->m_isMenuShow = true;
					menu = (MenuWidget*)menu->getParentWidget();
				}
			}
			virtual void onPressed(const MouseEvent& mouseEvent) override
			{
				m_tempIndex = m_curIndex;
			}

			virtual void onClicked(const MouseEvent& mouseEvent) override
			{
				auto local_pos = getLocalPos(mouseEvent.global);
				m_curIndex = _Find_Index(local_pos.y);
				if (m_curIndex >= 0 && m_tempIndex == m_curIndex)
				{
					switch (m_elemList[m_curIndex].mType)
					{
					case eAction:
					{
						m_elemList[m_curIndex].mAction->trigger();
						MenuWidget* menu = this;
						while (menu->m_isSubMenu)
						{
							menu->m_curIndex = -1;
							menu->m_isMenuShow = false;
							menu = (MenuWidget*)menu->getParentWidget();
						}
						menu->m_curIndex = -1;
						menu->m_isMenuShow = false;
						auto root = getRootWidget();
						root->unpopupWidget(menu);
						break;
					}
					case eCheckAction:
					{
						m_elemList[m_curIndex].mCheck = !m_elemList[m_curIndex].mCheck;
						m_elemList[m_curIndex].mAction->check(m_elemList[m_curIndex].mCheck);
						break;
					}
					}
				}
			}
			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override { Widget::onResizeEvent(resizeEvent); _Adjust_Layout(); }
		private:
			void _Clear_State();
			int  _Find_Index(int pos);
			void _Auto_Adapt_Size();
			void _Update_Min_Size();
			void _Adjust_Layout();
		};


	}
}


namespace CraftEngine
{
	namespace gui
	{


		inline void MenuWidget::_Clear_State()
		{
			if (m_curIndex >= 0)
			{
				for (int i = 0; i < m_curIndex; i++)
					if (m_elemList[i].mType == eMenu)
					{
						m_elemList[i].mSubMenu->m_curIndex = -1;
						m_elemList[i].mSubMenu->m_isMenuShow = false;
					}
				for (int i = m_curIndex + 1; i < m_elemList.size(); i++)
					if (m_elemList[i].mType == eMenu)
					{
						m_elemList[i].mSubMenu->m_curIndex = -1;
						m_elemList[i].mSubMenu->m_isMenuShow = false;
					}
			}
		}

		inline int MenuWidget::_Find_Index(int pos)
		{
			if (pos < 0)
				return -1;
			for (int i = 0; i < m_elemList.size(); i++)
				if (pos < m_elemList[i].mAreaRect.bottom())
					if (m_elemList[i].mType != eSeparator && m_elemList[i].mEnable)
						return i;
					else
						return -1;
			return -1;
		}

		inline void MenuWidget::_Auto_Adapt_Size()
		{
			_Update_Min_Size();
			setSize(m_minSize);
		}

		inline void MenuWidget::_Update_Min_Size()
		{
			int x = 0, y = 0;
			int text_x_offset = 30;
			int line_height = 22;
			int text_width = 0;
			int separator_height = 5;
			for (int i = 0; i < m_elemList.size(); i++)
			{
				switch (m_elemList[i].mType)
				{
				case eSeparator:
					y += separator_height;
					break;
				case eAction:
				case eCheckAction:
				case eMenu:
					y += line_height;
					auto t = GuiFontSystem::calcFontLineWidth(m_elemList[i].mText, getFont()) + 20;
					if (t > text_width)
						text_width = t;
					break;
				}
			}
			if (text_width < 120)
				text_width = 120;
			x = text_x_offset + text_width;
			m_minSize = Size(x, y);
		}

		void MenuWidget::_Adjust_Layout()
		{
			int icon_width = 25;
			int text_x_offset = 30;
			int icon_x_offset = (text_x_offset - icon_width) / 2;
			int line_height = 22;
			int separator_height = 5;
			int text_width = getWidth() - text_x_offset;
			int x = 0, y = 0;
			mTextAreaRect = Rect(icon_width, 0, getWidth() - icon_width, getHeight());
			for (int i = 0; i < m_elemList.size(); i++)
			{
				switch (m_elemList[i].mType)
				{
				case eSeparator:
					m_elemList[i].mSeparatorRect = Rect(text_x_offset, 2 + y, text_width, 1);
					m_elemList[i].mAreaRect = Rect(0, y, getWidth(), separator_height);
					y += separator_height;
					break;
				case eAction:
					m_elemList[i].mBasepoint = GuiFontSystem::calcFontBasePoint(m_elemList[i].mText, Rect(text_x_offset, y, text_width, line_height), getFont());
					m_elemList[i].mIconRect = Rect(icon_x_offset, y + 1, 20, 20);
					m_elemList[i].mAreaRect = Rect(0, y, getWidth(), line_height);
					y += line_height;
					break;
				case eCheckAction:
					m_elemList[i].mBasepoint = GuiFontSystem::calcFontBasePoint(m_elemList[i].mText, Rect(text_x_offset, y, text_width, line_height), getFont());
					m_elemList[i].mCheckboxRect = Rect(icon_x_offset, y + 1, 20, 20);
					m_elemList[i].mAreaRect = Rect(0, y, getWidth(), line_height);
					y += line_height;
					break;
				case eMenu:
					m_elemList[i].mBasepoint = GuiFontSystem::calcFontBasePoint(m_elemList[i].mText, Rect(text_x_offset, y, text_width, line_height), getFont());
					m_elemList[i].mSubMenuTipRect = Rect(getWidth() - 20, y + 1, 15, 20);
					m_elemList[i].mAreaRect = Rect(0, y, getWidth(), line_height);
					y += line_height;
					break;
				}
			}
		}

	}
}