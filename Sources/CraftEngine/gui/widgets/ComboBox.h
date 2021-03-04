#pragma once
#ifndef CRAFT_ENGINE_GUI_COMBO_BOX_H_
#define CRAFT_ENGINE_GUI_COMBO_BOX_H_

#include "./Label.h"
#include "./TextLine.h"
#include "./PushButton.h"

namespace CraftEngine
{
	namespace gui
	{

		class ComboBox : public Widget
		{
		private:

			class SeclectionLabel :public Label
			{
				ComboBox* m_combox = nullptr;
				
			public:

				SeclectionLabel(ComboBox* combox, const String& str, const Rect& rect, Widget* parent): Label(str, Rect(), parent)
				{
					m_combox = combox;
					setClickable(true);
					getPalette().mFillMode = Palette::eFillMode_Color;
				}

				virtual void onPaintEvent() override
				{
					auto painter = getPainter();
					drawBackground();
					if (isFocus())
						painter.drawRect(Rect(Offset(0), getSize()), getPalette().mBackgroundFocusColor);
					else if (isHighlight())
						painter.drawRect(Rect(Offset(0), getSize()), getPalette().mBackgroundHighLightColor);
					Label::onPaint_drawIcon();
					Label::onPaint_drawText();
					drawFrame();
				}

				virtual bool shouldPopupCancel()
				{
					if (isHighlight())
						return false;
					else
						return true;
				}

				virtual void onClicked(const MouseEvent& mouseEvent)override
				{
					if (isClickable())
						m_combox->_Selection_Clicked(this);
				}
			};





			int _Get_Selection_Index(SeclectionLabel* label)const
			{
				int i = 0;
				for (auto it = m_itemsList.begin(); i < m_itemsList.size(); i++, it++)
					if ((*it) == label)
						return i;
				return -1;
			}

			void _Selection_Clicked(SeclectionLabel* label)
			{
				auto index = _Get_Selection_Index(label);
				m_editableText->setText(label->getText());
				_On_Activate_Button_Clicked();
				if (m_curSelectionIndex != index)
				{
					m_curSelectionIndex = index;
					craft_engine_gui_emit(selectionChanged, m_curSelectionIndex);
				}
			}


			std::list<SeclectionLabel*> m_itemsList;

			Widget* m_popupWidget;
			TextLine* m_editableText;
			PushButton* m_activateButton;

			int m_curSelectionIndex = -1;
		public:

			craft_engine_gui_signal(selectionChanged, void(int));

			~ComboBox()
			{

			}

			ComboBox(const Rect& rect, Widget* parent) :Widget(rect, parent)
			{
				setMouseWheelable(true);
				m_popupWidget = new Widget(this);
				m_editableText = new TextLine(this);
				m_activateButton = new PushButton(this);
				//m_activateButton->getPalette().mFrameMode = Palette::eFrameMode_Surround;

				auto image = GuiAssetsManager::loadImage("Triangle_Down");
				m_activateButton->setIcon(image, Color(0, 0, 0, 255));

				_Adjust_Combo_Box_Layout();
				_Adjust_Popup_Widget_Layout();
				m_popupWidget->setPopupable(true);
				m_popupWidget->getPalette().mFillMode = Palette::eFillMode_Color;
				m_popupWidget->getPalette().mFrameMode = Palette::eFrameMode_Surround;
				m_popupWidget->getPalette().mBackgroundColor = Color(255, 255, 255, 255);
				getPalette().mFrameMode = Palette::eFrameMode_Surround;
				craft_engine_gui_connect(m_activateButton, clicked, this, _On_Activate_Button_Clicked);
			}

			ComboBox(Widget* parent) :ComboBox(Rect(0, 2, 80, 22), parent)
			{

			}

			void setCurrentIndex(int idx)
			{
				if (idx < 0)
				{
					m_curSelectionIndex = -1;
					m_editableText->setText(String());
					return;
				}
				auto it = m_itemsList.begin();
				for (int i = 0; i < idx; i++)
					it++;
				m_editableText->setText((*it)->getText());
				m_curSelectionIndex = idx;
			}

			TextLine* getTextLine() 
			{ 
				return m_editableText; 
			}

			void setEditable(bool enable)
			{
				m_editableText->setEditable(enable);
			}

			void setSelectionPopable(bool enable)
			{
				m_activateButton->setClickable(enable);
			}

			void addItem(const String& str)
			{
				auto label = new SeclectionLabel(this, str, Rect(), m_popupWidget);
				m_itemsList.push_back(label);
				_Adjust_Popup_Widget_Layout();
			}

			void insertItem(uint32_t index, const String& str)
			{
				auto label = new SeclectionLabel(this, str, Rect(), m_popupWidget);
				auto it = m_itemsList.begin();
				for (int i = 0; i < index; i++)
					it++;
				m_itemsList.insert(it, label);
				_Adjust_Popup_Widget_Layout();
			}

			void removeItem(uint32_t index)
			{
				if (index >= m_itemsList.size())
					throw std::runtime_error("param [index] out of range!");
				auto label = m_itemsList.begin();
				for (int i = 0; i < index; i++)
					label++;
				delete* label;
				m_itemsList.erase(label);
				_Adjust_Popup_Widget_Layout();
			}

			void clearItems()
			{
				for (auto& it : m_itemsList)
					delete it;
				m_itemsList.clear();
			}

			void setItems(const StringList& items)
			{
				clearItems();
				for (auto& it : items)
					addItem(it);
			}

			/*
			 Show candidate widget.
			 Do not call this if parent widget is set to nullptr
			*/
			virtual void showPopup()
			{
				if (!m_popupWidget->isNowPopup())
				{
					RootWidget* frame = getRootWidget();
					frame->popupWidget(m_popupWidget);
				}
			}

			/*
			 Hide candidate widget.
			 Do not call this if parent widget is set to nullptr
			*/
			virtual void hidePopup()
			{
				if (m_popupWidget->isNowPopup())
				{
					RootWidget* frame = getRootWidget();
					frame->unpopupWidget(m_popupWidget);
				}
			}

		protected:
			virtual void onWheel(const MouseEvent& mouseEvent) override
			{
				if (m_curSelectionIndex < 0)
					return;
				if (mouseEvent.offset.y < 0)
				{
					if (m_curSelectionIndex < m_itemsList.size() - 1)
					{
						m_curSelectionIndex++;
						setCurrentIndex(m_curSelectionIndex);
						craft_engine_gui_emit(selectionChanged, m_curSelectionIndex);
					}
				}
				else if (mouseEvent.offset.y > 0)
				{
					if (m_curSelectionIndex > 0)
					{
						m_curSelectionIndex--;
						setCurrentIndex(m_curSelectionIndex);
						craft_engine_gui_emit(selectionChanged, m_curSelectionIndex);
					}
				}
			}


			virtual bool shouldPopupCancel()override
			{
				if (!m_popupWidget->shouldPopupCancel())
					return false;
				else
					return Widget::shouldPopupCancel();
			}

			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				_Adjust_Combo_Box_Layout();
				_Adjust_Popup_Widget_Layout();
			}

			virtual void onPaintEvent() override
			{
				drawBackground();
				drawChild2(m_editableText);
				drawChild(m_activateButton);
			}

			void _Adjust_Combo_Box_Layout()
			{
				const Size& size = getSize();
				m_editableText->setRect(Rect(0, 0, size.x, size.y));
				m_activateButton->setRect(Rect(size.x - size.y + 2, 2, size.y - 3, size.y - 3));
			}

			void _Adjust_Popup_Widget_Layout()
			{
				m_popupWidget->setPos(Point(0, getSize().y));
				Size size = Size(getSize().x, 5);
				for (auto it : m_itemsList)
				{
					if (it->getMinSizeLimit().x > size.x)
						size.x = it->getMinSizeLimit().x;
				}
				if (m_itemsList.size() > 0)
				{
					size.y = 2;
					int i = 0;
					auto it = m_itemsList.begin();
					for (; i < m_itemsList.size(); i++, it++)
					{
						(*it)->setSize(Size(size.x, 19));
						(*it)->setPos(Point(2, size.y));
						size.y += 20;
					}
					size.x += 3;
				}
				m_popupWidget->setSize(size);			
			}


			void _On_Activate_Button_Clicked()
			{
				if (!m_popupWidget->isNowPopup())
				{
					RootWidget* frame = getRootWidget();
					frame->popupWidget(m_popupWidget);
				}
				else
				{
					RootWidget* frame = getRootWidget();
					frame->unpopupWidget(m_popupWidget);
				}
			}
	

		};

	}

}





#endif // !CRAFT_ENGINE_GUI_COMBO_BOX_H_


