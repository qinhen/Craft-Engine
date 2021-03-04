#pragma once

#include "../widgets/ValueInput.h"
#include "../widgets/ColorInput.h"

namespace CraftEngine
{
	namespace gui
	{


		struct ValueInputDescription
		{
			enum ItemType
			{


			};

			struct Item
			{
				ItemType type;



			};

			struct LineItems
			{
				String lineName;
				core::ArrayList<Item> items;

			};
			core::ArrayList<LineItems> lines;
		};
















	}
}