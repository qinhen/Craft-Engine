#pragma once
#include "../modelview/StandardItemModel.h"
#include "../modelview/StandardItemView.h"

namespace CraftEngine
{
	namespace gui
	{

		class StandradTableWidget : public model_view::StandardItemTableView
		{
		private:
			model_view::StandardItemTableModel* m_tableModel;
		public:
			StandradTableWidget(Widget* parent) : StandardItemTableView(parent)
			{
				m_tableModel = new model_view::StandardItemTableModel();
				setModel(m_tableModel);
			}
			~StandradTableWidget()
			{
				setModel(nullptr);
				delete m_tableModel;
			}
			void setItem(int row, int col, const model_view::StandardItem& item)
			{
				m_tableModel->setItemObject(m_tableModel->getModelIndex(row, col), item);
			}
		};


	}
}
