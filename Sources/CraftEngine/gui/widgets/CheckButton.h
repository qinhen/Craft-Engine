#pragma once
#ifndef CRAFT_ENGINE_GUI_CHECK_BUTTON_H_
#define CRAFT_ENGINE_GUI_CHECK_BUTTON_H_
#include "./Widget.h"

namespace CraftEngine
{
	namespace gui
	{

		class CheckButtonGroup;


		class CheckButton : public Widget
		{
		private:
			friend class CheckButtonGroup;
		public:

			enum CheckState
			{
				eCheckState_Disable = -1,
				eCheckState_Selected = 0,
				eCheckState_Unselected = 1,
				eCheckState_PartialSelected = 2,
			};
		private:
			HandleImage m_image = HandleImage(nullptr);
			CheckState m_checkState = CheckState::eCheckState_Unselected;
			CheckButtonGroup* m_buttonGroup;
		public:
			CheckButton(Widget* parent) : Widget(parent)
			{
				getPalette().mFillMode = Palette::FillMode::eFillMode_Color;
				getPalette().mFrameMode = Palette::FrameMode::eFrameMode_Surround;
				setClickable(true);
			}

			craft_engine_gui_signal(clicked, void(void));

			void setCheckState(CheckState state)
			{
				m_checkState = state;
				onCheckStateChanged();
			}

			CheckState getCheckState()const
			{
				return m_checkState;
			}

			virtual void onPaintEvent() override
			{
				auto painter = getPainter();
				if (isHighlight())
				{
					switch (getPalette().mFillMode)
					{
					case Palette::FillMode::eFillMode_Color:
						painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundHighLightColor);
						break;
					case Palette::FillMode::eFillMode_Image:
						painter.drawRect(Rect(Point(0), getSize()), getPalette().mBackgroundImage);
						break;
					default:
						break;
					}
				}
				if (m_image != nullptr)
					painter.drawSDF(Rect(Point(0), getSize()), getPalette().mForegroundColor, m_image);

				drawFrame();
			}
		protected:
			void onCheckStateChanged()
			{
				switch (m_checkState)
				{
				case CheckButton::eCheckState_Disable:
					m_image = HandleImage(nullptr);
					break;
				case CheckButton::eCheckState_Selected:
					m_image = GuiAssetsManager::loadImage("Tick");
					break;
				case CheckButton::eCheckState_Unselected:
					m_image = HandleImage(nullptr);
					break;
				case CheckButton::eCheckState_PartialSelected:
					m_image = GuiAssetsManager::loadImage("Block");
					break;
				default:
					break;
				}
			}
			virtual void onClicked(const MouseEvent& mouseEvent) override;
		private:
			void _Add_Into_Button_Group(CheckButtonGroup* buttonGroup);
			void _Remove_From_Button_Group();
		};



		class CheckButtonGroup
		{
		public:


		private:
			core::LinkedList<CheckButton*> m_buttonList;
		public:
			void _On_Any_Check_Button_Clicked(CheckButton* button)
			{
				if (button->getCheckState() == CheckButton::eCheckState_Selected)
				{
					for (auto& it : m_buttonList)
						it->setCheckState(CheckButton::eCheckState_Unselected);
					button->setCheckState(CheckButton::eCheckState_Selected);
				}
				else
				{
					button->setCheckState(CheckButton::eCheckState_Selected);
				}
				craft_engine_gui_emit(stateChanged, m_buttonList);
			}
		public:
			craft_engine_gui_signal(stateChanged, void(const core::LinkedList<CheckButton*>& btnList));

			CheckButtonGroup()
			{

			}

			~CheckButtonGroup()
			{
				for (auto& it : m_buttonList)
					it->_Remove_From_Button_Group();
				m_buttonList.clear();
			}

			void addButton(CheckButton* button)
			{
				button->setCheckState(CheckButton::eCheckState_Unselected);
				button->_Add_Into_Button_Group(this);
				m_buttonList.push_back(button);
			}

			void removeButton(CheckButton* button)
			{
				auto it = m_buttonList.begin();
				for (; it != m_buttonList.end(); it++)
				{
					if (*it == button)
					{
						button->_Remove_From_Button_Group();
						m_buttonList.erase(it);
						break;
					}
				}
			}
		};


		void CheckButton::onClicked(const MouseEvent& mouseEvent)
		{
			if (getCheckState() == CheckButton::eCheckState_Unselected)
				setCheckState(CheckButton::eCheckState_Selected);
			else if (getCheckState() == CheckButton::eCheckState_PartialSelected)
				setCheckState(CheckButton::eCheckState_Selected);
			else if (getCheckState() == CheckButton::eCheckState_Selected)
				setCheckState(CheckButton::eCheckState_Unselected);
			if (m_buttonGroup != nullptr)
				m_buttonGroup->_On_Any_Check_Button_Clicked(this);
			craft_engine_gui_emit(clicked, );
		}

		void CheckButton::_Add_Into_Button_Group(CheckButtonGroup* buttonGroup)
		{
			if (m_buttonGroup != nullptr)
				m_buttonGroup->removeButton(this);
			m_buttonGroup = buttonGroup;
		}

		void CheckButton::_Remove_From_Button_Group()
		{
			m_buttonGroup = nullptr;
		}

	}

}





#endif // !CRAFT_ENGINE_GUI_CHECK_BUTTON_H_ 0


