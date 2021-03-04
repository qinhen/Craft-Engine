#pragma once
#include "Widget.h"
#include "FrameWidget.h"

namespace CraftEngine
{
	namespace gui
	{


		class TabWidget :public Widget
		{
		public:
			enum TabPos
			{
				eTabPos_Top,
				eTabPos_Bottom,
			};

		private:
			class TabButton : public Widget
			{
			public:
				TabWidget* m_tabWidget;
				Point m_basepoint;
				String m_text;
				Size m_minSize;
				int  m_pageIndex;

				TabButton(const String& text, TabWidget* p) :Widget(p)
				{
					m_tabWidget = p;
					getFont().getAlignment().setHorizonAlignment(Alignment::eAlignmentMode_Center);
					setText(text);
					setClickable(true);
				}

				virtual void onPaintEvent() override
				{
					auto painter = getPainter();
					drawBackground();
					if (m_tabWidget->m_currentPageIndex == m_pageIndex)
						painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundFocusColor);
					else if (isHighlight())
						painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundHighLightColor);
					if (m_tabWidget->m_currentPageIndex == m_pageIndex)
						painter.drawTextLine(m_text, m_basepoint, getFont(), getPalette().mForegroundFocusColor);
					else
						painter.drawTextLine(m_text, m_basepoint, getFont(), getPalette().mForegroundColor);
				}

				virtual void onResizeEvent(const ResizeEvent& resizeEvent) override { Widget::onResizeEvent(resizeEvent); _Adjust_Label_Layout(); }

				void setText(const String& str)
				{
					m_text = str;
					m_minSize = Size(GuiFontSystem::calcFontLineWidth(str, getFont()) + getFont().getAlignment().mHorizonOffset, getFont().getSize()) + Size(10, 0);
					_Adjust_Label_Layout();
				}

				virtual Size getMinSizeLimit() const override
				{
					return m_minSize;
				}

				virtual void onClicked(const MouseEvent& mouseEvent) override final
				{
					m_tabWidget->_On_Tab_Button_Clicked(this);
				}
			private:

				void _Adjust_Label_Layout()
				{
					m_basepoint = GuiFontSystem::calcFontBasePoint(m_text, Rect(Point(0), getSize()), getFont());
				}
			};

			core::ArrayList<TabButton*> m_tabButtons;
			core::ArrayList<Widget*> m_tabs;
			int  m_currentPageIndex = -1;
			bool m_menuOpenMode = false;
			Rect m_areaRect;
			TabPos m_tabPos = eTabPos_Top;
		public:

			TabWidget(Widget* p) :Widget(Rect(0, 0, 400, 400), p)
			{
				
			}

			~TabWidget() 
			{

			}

			void setTabPos(TabPos pos)
			{
				m_tabPos = pos;
				_Update_Layout();
			}

			int getCurrentIndex() const
			{
				return m_currentPageIndex;
			}

			void setCurrentIndex(int index)
			{
				m_currentPageIndex = index;
			}

			int addPage(Widget* page, const String& name)
			{
				if (page->getGroupid() != GroupID::gDefaultWidget)
					throw std::exception("error"); 
				if (page->getParentWidget() != nullptr) 
				{
					page->getParentWidget()->unbindChildWidget(page);
					page->unbindParentWidget();
				}
				page->bindParentWidget(this);
				this->bindChildWidget(page);
				int index = m_tabs.size();
				m_tabButtons.push_back(new TabButton(name, this));
				m_tabButtons[index]->m_pageIndex = index;
				m_tabs.push_back(page);
				_Update_Layout();
				return m_tabs.size() - 1;
			}

			void removePage(int index)
			{
				delete m_tabButtons[index];
				m_tabButtons.erase(index);
				for (int i = 0; i < m_tabButtons.size(); i++)
					m_tabButtons[i]->m_pageIndex = i;
				m_tabs[index]->unbindParentWidget();
				unbindChildWidget(m_tabs[index]);
				delete m_tabs[index];
				m_tabs.erase(index);
				if (m_currentPageIndex == index)
					m_currentPageIndex--;
				_Update_Layout();
			}

			Widget* takePage(int index)
			{
				delete m_tabButtons[index];
				auto page = m_tabs[index];
				m_tabButtons.erase(index);
				m_tabs.erase(index);
				for (int i = 0; i < m_tabButtons.size(); i++)
					m_tabButtons[i]->m_pageIndex = i;
				if (m_currentPageIndex == index)
					m_currentPageIndex--;
				_Update_Layout();
				page->unbindParentWidget();
				unbindChildWidget(page);
				return page;
			}

			void swapPage(int iA, int iB)
			{
				m_tabButtons.swap(iA, iB);
				m_tabs.swap(iA, iB);
				for (int i = 0; i < m_tabButtons.size(); i++)
				{
					m_tabButtons[i]->m_pageIndex = i;
				}
				if (m_currentPageIndex == iA)
					m_currentPageIndex = iB;
				else if(m_currentPageIndex == iB)
					m_currentPageIndex = iA;
				_Update_Layout();
			}

			Widget* swapPage(int index, Widget* new_page)
			{
				auto old_page = m_tabs[index];
				old_page->unbindParentWidget();
				unbindChildWidget(old_page);
				if (new_page->getParentWidget() != nullptr)
				{
					new_page->getParentWidget()->unbindChildWidget(new_page);
					new_page->unbindParentWidget();
				}
				new_page->bindParentWidget(this);
				this->bindChildWidget(new_page);
				m_tabs[index] = new_page;
				_Update_Layout();
				return old_page;
			}

			virtual Widget* onDetect(const MouseEvent& mouseEvent) override
			{
				if (isHide() || isTransparent() || (isPopupable() && !isNowPopup()) || !getRect().inside(mouseEvent.local))
					return nullptr;

				Widget* hit;
				MouseEvent subEvent = mouseEvent;
				subEvent.local = mouseEvent.local - getPos() - getChildOriginal(GroupID::gDefaultWidget);
				for (int i = m_tabButtons.size() - 1; i >= 0; i--)
				{
					hit = m_tabButtons[i]->onMouseEvent(subEvent);
					if (hit != nullptr)
						return hit;
				}
				if (m_currentPageIndex >= 0)
				{
					hit = m_tabs[m_currentPageIndex]->onMouseEvent(subEvent);
					if (hit != nullptr)
						return hit;
				}
				return this;
			}

			virtual void onPaintEvent() override
			{
				auto painter = getPainter();
				drawBackground();
				for (int i = 0; i < m_tabButtons.size(); i++)
					drawChild(m_tabButtons[i]);
				if (m_currentPageIndex >= 0)
				{
					painter.drawRectFrame(m_areaRect, m_tabButtons[m_currentPageIndex]->getPalette().mBackgroundFocusColor);
					drawChild(m_tabs[m_currentPageIndex]);
				}
				drawFrame();
			}
		private:
			void _Update_Layout()
			{
				const int tab_height = 22;
				int x = 0;
				int y = 0;

				if (m_tabPos == eTabPos_Top)
				{
					y = 0;
					m_areaRect = Rect(0, tab_height, getWidth(), getHeight() - tab_height);
					auto page_rect = m_areaRect;
					page_rect.mOffset += Offset(4);
					page_rect.mSize -= Offset(8);
					for (int i = 0; i < m_tabButtons.size(); i++)
					{
						m_tabButtons[i]->setRect(Rect(Point(x, y), Size(m_tabButtons[i]->getMinSizeLimit().x, tab_height)));
						x += m_tabButtons[i]->getWidth();
					}
					for (int i = 0; i < m_tabs.size(); i++)
					{
						m_tabs[i]->setRect(page_rect);
					}
				}
				else
				{
					y = getHeight() - tab_height;
					m_areaRect = Rect(0, 0, getWidth(), getHeight() - tab_height);
					auto page_rect = m_areaRect;
					page_rect.mOffset += Offset(4);
					page_rect.mSize -= Offset(8);
					for (int i = 0; i < m_tabButtons.size(); i++)
					{
						m_tabButtons[i]->setRect(Rect(Point(x, y), Size(m_tabButtons[i]->getMinSizeLimit().x, tab_height)));
						x += m_tabButtons[i]->getWidth();
					}
					for (int i = 0; i < m_tabs.size(); i++)
					{
						m_tabs[i]->setRect(page_rect);
					}
				}
			}

			void _On_Tab_Button_Clicked(TabButton* button)
			{
				m_currentPageIndex = button->m_pageIndex;
			}

		protected:
			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				_Update_Layout();
			}
		};


	}
}

