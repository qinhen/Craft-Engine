#pragma once
#ifndef CRAFT_ENGINE_GUI_STANDARD_ITEM_MODEL_H_
#define CRAFT_ENGINE_GUI_STANDARD_ITEM_MODEL_H_

#include "./AbstractModel.h"
#include "./StandardItem.h"

namespace CraftEngine
{
	namespace gui
	{
		namespace model_view
		{

			class StandardItemListModel : public TemplateListModel<StandardItem>
			{
			private:
				virtual ItemData onGetItemData(const ModelIndex& index, int role = 0) const override final
				{
					const auto& item = getListModelItemData(index.getRow());
					return item.getData(role);
				}
				virtual void onSetItemData(const ModelIndex& index, const ItemData& data, int role) override final
				{
					getListModelItemData(index.getRow()).setData(data, role);
				}
			public:
				virtual ItemDescription getItemDescription()const override final
				{
					return StandardItem::getDescription();
				}
			};

			class StandardItemTableModel : public TemplateTableModel<StandardItem>
			{
			private:
				virtual ItemData onGetItemData(const ModelIndex& index, int role = 0) const override final
				{
					const auto& item = getTableModelItemData(index.getRow(), index.getCol());
					return item.getData(role);
				}
				virtual void onSetItemData(const ModelIndex& index, const ItemData& data, int role) override final
				{
					getTableModelItemData(index.getRow(), index.getCol()).setData(data, role);
				}
			public:
				virtual ItemDescription getItemDescription()const override final
				{
					return StandardItem::getDescription();
				}
			};

			class StandardItemTreeModel : public TemplateTreeModel<StandardItem>
			{
			private:
				virtual ItemData onGetItemData(const ModelIndex& index, int role = 0) const override final
				{
					const auto& item = getTreeModelItemData(index);
					return item.getData(role);
				}
				virtual void onSetItemData(const ModelIndex& index, const ItemData& data, int role) override final
				{
					getTreeModelItemData(index).setData(data, role);
				}
			public:
				virtual ItemDescription getItemDescription()const override final
				{
					return StandardItem::getDescription();
				}
			};



		}
	}
}
#endif