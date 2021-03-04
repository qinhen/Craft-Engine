#pragma once
#ifndef CRAFT_ENGINE_GUI_STRING_LIST_WIDGET_H_
#define CRAFT_ENGINE_GUI_STRING_LIST_WIDGET_H_


#include "../modelview/StringsModel.h"
#include "../modelview/StringsView.h"

namespace CraftEngine
{
	namespace gui
	{

		class StringListWidget : public model_view::StringListView
		{
		private:
			model_view::StringListModel* m_listModel;
		public:
			StringListWidget(Widget* parent) : StringListView(parent)
			{
				m_listModel = new model_view::StringListModel();
				setModel(m_listModel);
			}
			StringListWidget(const StringList& strings, Widget* parent) : StringListView(parent)
			{
				m_listModel = new model_view::StringListModel();
				m_listModel->insertRows(0, strings.size());
				auto it = strings.begin();
				for (int i = 0; i < strings.size(); i++, it++)
					m_listModel->setItemData(m_listModel->getModelIndex(i), *it, 0);
				setModel(m_listModel);
			}
			~StringListWidget()
			{
				setModel(nullptr);
				delete m_listModel;
			}
			void insertString(int index, const String& str)
			{
				m_listModel->insertRow(index);
				m_listModel->setItemData(m_listModel->getModelIndex(index), str, 0);
			}
			void insertString(const String& str)
			{
				auto index = m_listModel->getRowCount();
				insertString(index, str);
			}
			void removeString(int index)
			{
				m_listModel->removeRow(index);
			}
			void setString(int index, const String& str)
			{
				m_listModel->setItemData(m_listModel->getModelIndex(index), str, 0);
			}
			void setStringList(const StringList& strings)
			{
				removeAllStrings();
				m_listModel->insertRows(0, strings.size());
				auto it = strings.begin();
				for (int i = 0; i < strings.size(); i++, it++)
					m_listModel->setItemData(m_listModel->getModelIndex(i), *it, 0);
			}
			void removeAllStrings()
			{
				m_listModel->removeRows(0, m_listModel->getRowCount());
			}
		};


	}
}

#endif // CRAFT_ENGINE_GUI_STRING_LIST_WIDGET_H_