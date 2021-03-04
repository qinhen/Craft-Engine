#pragma once
#include "../Common.h"
namespace CraftEngine
{
	namespace gui
	{

		namespace model_view
		{

			using ItemData = core::Variant<
				String,
				HandleImage,
				Font,
				Palette,
				Color,
				int,
				float,
				bool,
				void*,
				nullptr_t>;

			enum class ItemDataType: int
			{
				eUnknown,
				eString,
				eImage,
				eFont,
				ePalette,
				eColor,
				eInt,
				eFloat,
				eBool,
				ePointer,
				eNullPointer,
			};

			struct ItemDescription
			{
				ItemDescription() {}
				ItemDescription(const std::initializer_list<ItemDataType>& typeList) : m_typeList(typeList) {}
				ItemDescription(const core::ArrayList<ItemDataType>& typeList) : m_typeList(typeList) {}
				ItemDescription(core::ArrayList<ItemDataType>&& typeList) { m_typeList.swap(typeList); }
				const ItemDataType& operator[](int index)const { return m_typeList[index]; }
				ItemDataType& operator[](int index) { m_typeList[index]; }
				int size() const { return m_typeList.size(); }
			private:
				core::ArrayList<ItemDataType> m_typeList;
			};



		}
	}
}