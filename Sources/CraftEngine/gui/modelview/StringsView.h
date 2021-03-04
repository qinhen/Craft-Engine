#pragma once
#ifndef CRAFT_ENGINE_GUI_STRINGS_VIEW_H_
#define CRAFT_ENGINE_GUI_STRINGS_VIEW_H_

#include "./AbstractView.h"
namespace CraftEngine
{
	namespace gui
	{
		namespace model_view
		{



			class StringWidget :public AbstractItemWidget
			{
			private:
				String m_str;
				Point m_basePoint;
			public:
				StringWidget(AbstractItemView* view) : AbstractItemWidget(view) {}
				virtual void onItemChanged() override final
				{
					auto&& index = getIndex();
					if (index.isValid())
					{
						m_str = index.getData(0).get<String>();
						m_basePoint = GuiFontSystem::calcFontBasePoint(m_str, Rect(0, 0, getSize().x, getSize().y), getFont());
					}
				}

				virtual void onPaint_drawEx() override
				{
					auto painter = getPainter();
					if (m_str.size() > 0)
					{
						if (isFocus())
							painter.drawTextLine(m_str, m_basePoint, getFont(), getPalette().mForegroundFocusColor);
						else
							painter.drawTextLine(m_str, m_basePoint, getFont(), getPalette().mForegroundColor);
					}
				}

			};



			class StringListView :public AbstractListView
			{
			private:

			public:
				StringListView(Widget* parent) :AbstractListView(parent) {}

			protected:
				virtual AbstractItemWidget* createItemWidget()override
				{
					auto new_widget = new StringWidget(this);
					return new_widget;
				}
			};


			class StringTableView :public AbstractTableView
			{
			private:

			public:
				StringTableView(Widget* parent) :AbstractTableView(parent) {}

			protected:
				virtual AbstractItemWidget* createItemWidget()override
				{
					auto new_widget = new StringWidget(this);
					return new_widget;
				}
			};


			class StringTreeView :public AbstractTreeView
			{
			private:

			public:
				StringTreeView(Widget* parent) :AbstractTreeView(parent) {}

			protected:
				virtual AbstractItemWidget* createItemWidget()override
				{
					auto new_widget = new StringWidget(this);
					return new_widget;
				}
			};




		}
	}
}
#endif // CRAFT_ENGINE_GUI_STRINGS_VIEW_H_