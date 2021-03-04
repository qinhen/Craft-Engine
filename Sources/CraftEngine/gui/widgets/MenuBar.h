#pragma once
#include "MenuWidget.h"

namespace CraftEngine
{
	namespace gui
	{


		class MenuBar :public Widget
		{
		private:

			class MenuButton : public Widget
			{
			public:
				MenuWidget* m_menuWidget;
				Point m_basepoint;
				String m_text;
				Size m_minSize;
				MenuButton(const String& text, Widget* p) :Widget(p)
				{
					getFont().getAlignment().setHorizonAlignment(Alignment::eAlignmentMode_Center);
					m_menuWidget = new MenuWidget(this);
					m_menuWidget->setPopupable(true);
					setText(text);
					setClickable(true);
				}

				virtual void onPaintEvent() override
				{
					auto painter = getPainter();
					drawBackground();
					if (isFocus() || m_menuWidget->isNowPopup())
						painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundFocusColor);
					else if (isHighlight())
						painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundHighLightColor);
					painter.drawTextLine(m_text, m_basepoint, getFont(), getPalette().mForegroundColor);
					drawFrame();
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
					if (m_menuWidget->isNowPopup())
					{
						m_menuWidget->unpopupMenu();
					}
					else
					{
						auto pos = getGlobalPos(Point(0, getHeight()));
						m_menuWidget->popupMenu(pos);
					}
				}
			private:

				void _Adjust_Label_Layout()
				{
					m_basepoint = GuiFontSystem::calcFontBasePoint(m_text, Rect(Point(0), getSize()), getFont());
				}
			};


			core::ArrayList<MenuButton*> m_buttons;
			bool m_menuOpenMode = false;
		public:

			MenuBar(Widget* p) :Widget(Rect(0, 0, 200, 25), p)
			{
				
			}

			MenuWidget* addMenu(const String& name)
			{
				auto new_button = new MenuButton(name, this);
				m_buttons.push_back(new_button);
				_Update_Layout();
				return new_button->m_menuWidget;
			}

			void setMenuName(const String& name, int index)
			{
				m_buttons[index]->setText(name);
				_Update_Layout();
			}

			MenuWidget* getMenu(int index)
			{
				return m_buttons[index]->m_menuWidget;
			}
		private:
			void _Update_Layout()
			{
				int x = 0;
				int y = (getHeight() - 22) / 2;
				for (int i = 0; i < m_buttons.size(); i++)
				{
					m_buttons[i]->setRect(Rect(Point(x, y), Size(m_buttons[i]->getMinSizeLimit().x, 22)));
					x += m_buttons[i]->getWidth();
				}
			}
		protected:
			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				_Update_Layout();
			}
		};


	}
}

