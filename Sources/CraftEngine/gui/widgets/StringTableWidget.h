#pragma once
#ifndef CRAFT_ENGINE_GUI_STRING_TABLE_WIDGET_H_
#define CRAFT_ENGINE_GUI_STRING_TABLE_WIDGET_H_


#include "../modelview/StringsModel.h"
#include "../modelview/StringsView.h"

namespace CraftEngine
{
	namespace gui
	{

		class StringTableWidget : public model_view::StringTableView
		{
		private:
			model_view::StringTableModel* m_tableModel;
		public:
			StringTableWidget(Widget* parent) : StringTableView(parent)
			{
				m_tableModel = new model_view::StringTableModel();
				setModel(m_tableModel);
			}
			~StringTableWidget()
			{
				setModel(nullptr);
				delete m_tableModel;
			}
			void setString(int row, int col, const String& str)
			{
				m_tableModel->setItemData(m_tableModel->getModelIndex(row, col), str, 0);
			}
		};


	}
}

#endif // CRAFT_ENGINE_GUI_STRING_TABLE_WIDGET_H_