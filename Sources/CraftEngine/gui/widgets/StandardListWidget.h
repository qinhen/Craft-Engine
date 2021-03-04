#pragma once
#include "../modelview/StandardItemModel.h"
#include "../modelview/StandardItemView.h"

namespace CraftEngine
{
	namespace gui
	{

		class StandardListWidget : public model_view::StandardItemListView
		{
		private:
			model_view::StandardItemListModel* m_listModel;
		public:
			StandardListWidget(Widget* parent) : StandardItemListView(parent)
			{
				m_listModel = new model_view::StandardItemListModel();
				setModel(m_listModel);
			}
			StandardListWidget(const StringList& strings, Widget* parent) : StandardListWidget(parent)
			{
				m_listModel = new model_view::StandardItemListModel();
				m_listModel->insertRows(0, strings.size());
				auto it = strings.begin();
				for (int i = 0; i < strings.size(); i++, it++)
					m_listModel->setItemData(m_listModel->getModelIndex(i), *it, 0);
				setModel(m_listModel);
			}
			~StandardListWidget()
			{
				setModel(nullptr);
				delete m_listModel;
			}
			void insertItem(int index, const model_view::StandardItem& item)
			{
				m_listModel->insertRow(index);
				m_listModel->setItemObject(m_listModel->getModelIndex(index), item);
				m_listModel->notifyUpdated(m_listModel->getModelIndex(index));
			}
			void insertItem(const model_view::StandardItem& item)
			{
				auto index = m_listModel->getRowCount();
				insertItem(index, item);
			}
			void removeString(int index)
			{
				m_listModel->removeRow(index);
			}
			void setItem(int index, const model_view::StandardItem& item)
			{
				m_listModel->setItemObject(m_listModel->getModelIndex(index), item);
				m_listModel->notifyUpdated(m_listModel->getModelIndex(index));
			}
			void setItemList(const core::LinkedList<model_view::StandardItem>& items)
			{
				removeAllStrings();
				m_listModel->insertRows(0, items.size());
				auto it = items.begin();
				for (int i = 0; i < items.size(); i++, it++)
					m_listModel->setItemObject(m_listModel->getModelIndex(i), *it);
			}
			void removeAllStrings()
			{
				m_listModel->removeRows(0, m_listModel->getRowCount());
			}
		};


	}
}
