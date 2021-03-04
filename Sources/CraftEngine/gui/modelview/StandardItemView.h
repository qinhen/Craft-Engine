#pragma once
#ifndef CRAFT_ENGINE_GUI_STANDARD_ITEM_VIEW_H_
#define CRAFT_ENGINE_GUI_STANDARD_ITEM_VIEW_H_

#include "../widgets/CheckButton.h"
#include "./AbstractView.h"
#include "./StandardItem.h"

namespace CraftEngine
{
	namespace gui
	{
		namespace model_view
		{

			class StandardItemWidget :public AbstractItemWidget
			{
			private:
				CheckButton* m_checkButton;

				// render data
				Point m_basePoint;
				Rect  m_iconRect;

				// meta data
				HandleImage m_dataIcon;
				String      m_dataText;
				int         m_checkState;

			public:
				StandardItemWidget(AbstractItemView* view) : AbstractItemWidget(view)
					, m_dataIcon(nullptr), m_dataText(), m_checkState(0)
					, m_basePoint(0)
				{
					m_checkButton = new CheckButton(this);
					craft_engine_gui_connect(m_checkButton, clicked, this, _On_Check_Button_Clicked);
				}

				virtual void onItemChanged() override final
				{
					auto&& index = getIndex();
					if (index.isValid())
					{
						m_dataIcon = index.getData(StandardItem::eIcon).get<HandleImage>();
						m_dataText = index.getData(StandardItem::eText).get<String>();
						setFont(index.getData(StandardItem::eFont).get<Font>());
						setPalette(index.getData(StandardItem::ePalette).get<Palette>());
						m_checkState = index.getData(StandardItem::eCheckState).get<int>();

						const Size& size = getSize();
						int x = 0;
						int check_button_length = 14;
						if (m_checkState != CheckButton::eCheckState_Disable)
						{
							m_checkButton->setHide(false);
							m_checkButton->setCheckState((CheckButton::CheckState)m_checkState);
							m_checkButton->setRect(Rect((size.y - 14) / 2, (size.y - 14) / 2, 14, 14));
							x += size.y;
						}
						else
						{
							m_checkButton->setHide(true);
						}
						if (m_dataIcon != nullptr)
						{
							m_iconRect = Rect((size.y - 14) / 2, x + (size.y - 14) / 2, 14, 14);
							x += size.y;
						}
						if (m_dataText.size() > 0)
							m_basePoint = GuiFontSystem::calcFontBasePoint(m_dataText, Rect(x, 0, getSize().x - x, getSize().y), getFont());
					}
				}

				virtual void onPaint_drawEx() override
				{
					auto painter = getPainter();
					if (m_checkState != CheckButton::eCheckState_Disable)
						drawChild(m_checkButton);
					if (m_dataIcon != nullptr)
						painter.drawSDF(m_iconRect, getPalette().mForegroundColor, m_dataIcon);
					if (m_dataText.size() > 0)
					{
						if (isFocus())
							painter.drawTextLine(m_dataText, m_basePoint, getFont(), getPalette().mForegroundFocusColor);
						else
							painter.drawTextLine(m_dataText, m_basePoint, getFont(), getPalette().mForegroundColor);
					}
				}
	
			private:

				void _On_Check_Button_Clicked() ///
				{
					if (m_checkButton->getCheckState() == CheckButton::eCheckState_Unselected)
						m_checkButton->setCheckState(CheckButton::eCheckState_Selected);
					else if (m_checkButton->getCheckState() == CheckButton::eCheckState_Selected)
						m_checkButton->setCheckState(CheckButton::eCheckState_Unselected);
					getView()->setModelItemData(getIndex(), (int)m_checkButton->getCheckState(), StandardItem::eCheckState);
				}
			};



			class StandardItemListView :public AbstractListView
			{
			private:

			public:
				StandardItemListView(Widget* parent) :AbstractListView(parent) {}

			protected:
				virtual AbstractItemWidget* createItemWidget()override
				{
					return new StandardItemWidget(this);
				}
			};


			class StandardItemTableView :public AbstractTableView
			{
			private:

			public:
				StandardItemTableView(Widget* parent) :AbstractTableView(parent) {}

			protected:
				virtual AbstractItemWidget* createItemWidget()override
				{
					return new StandardItemWidget(this);
				}
			};


			class StandardItemTreeView :public AbstractTreeView
			{
			private:

			public:
				StandardItemTreeView(Widget* parent) :AbstractTreeView(parent) {}

			protected:
				virtual AbstractItemWidget* createItemWidget()override
				{
					return new StandardItemWidget(this);
				}
			};




		}
	}
}
#endif // CRAFT_ENGINE_GUI_STRINGS_VIEW_H_