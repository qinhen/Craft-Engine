#pragma once
#ifndef CRAFT_ENGINE_GUI_STRINGS_MODEL_H_
#define CRAFT_ENGINE_GUI_STRINGS_MODEL_H_

#include "./AbstractModel.h"
namespace CraftEngine
{
	namespace gui
	{
		namespace model_view
		{


			class StringListModel : public TemplateListModel<String>
			{
			private:
				virtual ItemData onGetItemData(const ModelIndex& index, int role = 0) const override final
				{
					return getListModelItemData(index.getRow());
				}
				virtual void onSetItemData(const ModelIndex& index, const ItemData& data, int role) override final
				{
					getListModelItemData(index.getRow()) = data.get<String>();
				}
			public:
				virtual ItemDescription getItemDescription()const override final
				{
					return ItemDescription({ ItemDataType::eString });
				}
			};

			class StringTableModel : public TemplateTableModel<String>
			{
			private:
				virtual ItemData onGetItemData(const ModelIndex& index, int role = 0) const override final
				{
					return getTableModelItemData(index.getRow(), index.getCol());
				}
				virtual void onSetItemData(const ModelIndex& index, const ItemData& data, int role) override final
				{
					getTableModelItemData(index.getRow(), index.getCol()) = data.get<String>();
				}
			public:
				virtual ItemDescription getItemDescription()const override final
				{
					return ItemDescription({ ItemDataType::eString });
				}
			};

			class StringTreeModel : public TemplateTreeModel<String>
			{
			private:
				virtual ItemData onGetItemData(const ModelIndex& index, int role = 0) const override final
				{
					return getTreeModelItemData(index);
				}
				virtual void onSetItemData(const ModelIndex& index, const ItemData& data, int role) override final
				{
					getTreeModelItemData(index) = data.get<String>();
				}
			public:
				virtual ItemDescription getItemDescription()const override final
				{
					return ItemDescription({ ItemDataType::eString });
				}
			};



		}
	}
}
#endif