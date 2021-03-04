#pragma once
#ifndef CRAFT_ENGINE_GUI_STANDARD_ITEM_H_
#define CRAFT_ENGINE_GUI_STANDARD_ITEM_H_

#include "./AbstractModel.h"
namespace CraftEngine
{
	namespace gui
	{
		namespace model_view
		{


			struct StandardItem
			{
			public:
				enum ItemRole
				{
					eIcon,
					eText,
					eFont,
					ePalette,
					eCheckState,
				};
			private:
				HandleImage m_icon;
				String      m_text;
				Font        m_font;
				Palette     m_palette;
				int         m_checkBoxState;
			public:
				StandardItem()
					:m_icon(nullptr), m_text(), m_font(), m_palette(), m_checkBoxState(0) {}

				void setIcon(const HandleImage& icon) { m_icon = icon; }
				const HandleImage& getIcon() { return m_icon; }
				void setText(const String& text) { m_text = text; }
				const String& getText() { return m_text; }
				void setFont(const Font& font) { m_font = font; }
				const Font& getFont() { return m_font; }
				void setPalette(const Palette& palette) { m_palette = palette; }
				const Palette& getPalette() { return m_palette; }
				void setCheckBoxState(int checkBoxState) { m_checkBoxState = checkBoxState; }
				int getCheckBoxState() { return m_checkBoxState; }

				ItemData getData(int role = 0) const
				{
					switch (role)
					{
					case StandardItem::eIcon:
						return m_icon;
					case StandardItem::eText:
						return m_text;
					case StandardItem::eFont:
						return m_font;
					case StandardItem::ePalette:
						return m_palette;
					case StandardItem::eCheckState:
						return m_checkBoxState;
					default:
						return nullptr;
					}
				}

				void setData(const ItemData& data, int role = 0)
				{
					switch (role)
					{
					case StandardItem::eIcon:
						m_icon = data.get<HandleImage>();
						break;
					case StandardItem::eText:
						m_text = data.get<String>();
						break;
					case StandardItem::eFont:
						m_font = data.get<Font>();
						break;
					case StandardItem::ePalette:
						m_palette = data.get<Palette>();
						break;
					case StandardItem::eCheckState:
						m_checkBoxState = data.get<int>();
						break;
					default:
						return;
					}
				}

				static ItemDescription getDescription()
				{
					return ItemDescription({ 
						ItemDataType::eImage,
						ItemDataType::eString,
						ItemDataType::eFont,
						ItemDataType::ePalette,
						ItemDataType::eInt
						});
				}
			};


		}
	}
}
#endif