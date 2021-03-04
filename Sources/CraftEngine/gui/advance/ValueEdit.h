#pragma once

#include "../widgets/ValueInput.h"
#include "../widgets/ColorInput.h"

namespace CraftEngine
{
	namespace gui
	{

		namespace value_edit
		{
			typedef core::Variant<String, bool, int, float, Color> ValueData;

			struct ValueEditDescription
			{
				//enum ValueItemType
				//{
				//	eValueItemType_String,
				//	eValueItemType_Bool,
				//	eValueItemType_Int,
				//	eValueItemType_Float,
				//	eValueItemType_Color,
				//};

				enum ValueType
				{
					eValueContainerType_String_TextLine,
					eValueContainerType_String_ComboxBox,
					eValueContainerType_Bool_CheckBox,
					eValueContainerType_Int_ValueInput,
					eValueContainerType_Float_ValueInput,
					eValueContainerType_Color_ColorInput,
				};

				struct ValueData
				{
					String     name;
					StringList candidateStrings;
				};

				struct ValueItem
				{
					//ValueItemType valueType;
					ValueType valueType;
					ValueData valueData;
				};

				struct LineItems
				{
					String lineName;
					core::ArrayList<ValueItem> items;
				};
				core::ArrayList<LineItems> lines;
			};


			class ValueEditWidget : public Widget
			{
			private:
				ValueEditDescription m_descripton;
				core::ArrayList<core::ArrayList<Widget*>> m_widgets;
				core::ArrayList<Label*> m_labels;
			public:

				craft_engine_gui_signal(valueChanged, void(const String& name, const ValueData& data));

				ValueEditWidget(Widget* parent) : Widget(parent)
				{

				}

				void setDescription(const ValueEditDescription& description)
				{
					m_descripton = description;
					_Clear_All_Widget();
					_Update_Container_Widgets();
					_Adjust_Lines_Layout();
				}

				const ValueEditDescription& getDescription()
				{
					return m_descripton;
				}

			private:
				virtual void onResizeEvent(const ResizeEvent& resizeEvent)
				{
					_Adjust_Lines_Layout();
				}

				void _Clear_All_Widget()
				{
					for (auto line : m_widgets)
						for (auto i : line)
							delete i;
					m_widgets.clear();
					for (auto label : m_labels)
						delete label;
					m_labels.clear();
				}

				void _Update_Container_Widgets()
				{
					m_widgets.resize(m_descripton.lines.size());
					m_labels.resize(m_descripton.lines.size());
					for (int i = 0; i < m_descripton.lines.size(); i++)
					{
						m_labels[i] = new Label(m_descripton.lines[i].lineName, this);
						m_widgets[i].resize(m_descripton.lines[i].items.size());
						for (int j = 0; j < m_descripton.lines[i].items.size(); j++)
						{
							auto& item = m_descripton.lines[i].items[j];
							switch (item.valueType)
							{
							case ValueEditDescription::eValueContainerType_String_TextLine:
							{
								auto widget = new TextLine(this);
								widget->setPlaceholderText(item.valueData.name);
								craft_engine_gui_connect_v2(widget, editFinished, [=](const String& str) {
									ValueData data = str;
									const String& name = item.valueData.name;
									craft_engine_gui_emit(valueChanged, name, data);
								});
								m_widgets[i][j] = widget;
								break;
							}
							case ValueEditDescription::eValueContainerType_String_ComboxBox:
							{
								auto widget = new ComboBox(this);
								widget->setItems(item.valueData.candidateStrings);
								craft_engine_gui_connect_v2(widget, selectionChanged, [=](int idx) {
									auto it = item.valueData.candidateStrings.begin();
									for (int p = 0; p < idx; p++)
										++it;
									ValueData data = String((*it));
									const String& name = item.valueData.name;
									craft_engine_gui_emit(valueChanged, name, data);
								});
								m_widgets[i][j] = widget;
								break;
							}
							case ValueEditDescription::eValueContainerType_Bool_CheckBox:
							{
								auto widget = new CheckBox(this);
								widget->setText(item.valueData.name);
								craft_engine_gui_connect_v2(widget, stateChanged, [=](int state) {
									ValueData data = (state == CheckButton::eCheckState_Selected);
									const String& name = item.valueData.name;
									craft_engine_gui_emit(valueChanged, name, data);
								});
								m_widgets[i][j] = widget;
								break;
							}
							case ValueEditDescription::eValueContainerType_Int_ValueInput:
							{
								auto widget = new ValueInputInt(this);
								widget->setValueName(item.valueData.name);
								craft_engine_gui_connect_v2(widget, valueChanged, [=](int val) {
									const ValueData data = val;
									const String& name = item.valueData.name;
									craft_engine_gui_emit(valueChanged, name, data);
								});
								m_widgets[i][j] = widget;
								break;
							}
							case ValueEditDescription::eValueContainerType_Float_ValueInput:
							{
								auto widget = new ValueInputFloat(this);
								widget->setValueName(item.valueData.name);
								craft_engine_gui_connect_v2(widget, valueChanged, [=](float val) {
									const ValueData data = val;
									const String& name = item.valueData.name;
									craft_engine_gui_emit(valueChanged, name, data);
								});
								m_widgets[i][j] = widget;
								break;
							}
							case ValueEditDescription::eValueContainerType_Color_ColorInput:
							{
								auto widget = new ColorInput(this);
								craft_engine_gui_connect_v2(widget, colorChanged, [=](const Color& val) {
									const ValueData data = val;
									const String& name = item.valueData.name;
									craft_engine_gui_emit(valueChanged, name, data);
								});
								m_widgets[i][j] = widget;
								break;
							}
							default:
								throw std::exception("error");
								break;
							}

						}

					}
				}

				void _Adjust_Lines_Layout()
				{
					int label_width = 60;
					int x = 0, y = 0;
					for (int i = 0; i < m_descripton.lines.size(); i++)
					{
						int h = m_widgets[i][0]->getHeight();
						m_labels[i]->setRect(Rect(0, y, label_width, h));
						x = label_width;
						int w = (getWidth() - label_width) / m_descripton.lines[i].items.size();
						int last_w = (getWidth() - label_width) - w * (m_descripton.lines[i].items.size() - 1);
						for (int j = 0; j < m_descripton.lines[i].items.size() - 1; j++)
						{
							m_widgets[i][j]->setRect(Rect(x, y, w, h));
							x += w;
						}
						m_widgets[i][m_descripton.lines[i].items.size() - 1]->setRect(Rect(x, y, last_w, h));
						y += h;
					}
				}








			};




		}
	}
}