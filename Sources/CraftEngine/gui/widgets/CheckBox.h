#pragma once
#ifndef CRAFT_ENGINE_GUI_CHECK_BOX_H_
#define CRAFT_ENGINE_GUI_CHECK_BOX_H_
#include "./Label.h"
#include "./CheckButton.h"

namespace CraftEngine
{
	namespace gui
	{


		/*
		 CheckBox	
		 Signal: stateChanged / Slot: void(int)
		*/
		class CheckBox : public Widget
		{
		private:

			Label* m_label;
			CheckButton* m_button;

			void _On_Button_Clicked()
			{
				craft_engine_gui_emit(stateChanged, m_button->getCheckState());
			}

			void _Adjust_Check_Box_Layout()
			{
				const Size& size = getSize();
				m_button->setRect(Rect((size.y - 14) / 2, (size.y - 14) / 2, 14, 14));
				m_label->setRect(Rect(size.y, 1, size.x - size.y, size.y - 2));
			}
		public:

			craft_engine_gui_signal(stateChanged, void(int));

			~CheckBox()
			{

			}

			CheckBox(const String& text, const Rect& rect, Widget* parent) :Widget(rect, parent)
			{
				m_label = new Label(text, Rect(), this);
				m_label->getFont().getAlignment().mHorizonMode = Alignment::eAlignmentMode_Left;
				m_button = new CheckButton(this);
				craft_engine_gui_connect(m_button, clicked, this, _On_Button_Clicked);
				_Adjust_Check_Box_Layout();
			}

			CheckBox(const String& text, Widget* parent) :CheckBox(text, Rect(0, 2, 80, 22), parent)
			{

			}

			CheckBox(Widget* parent) :CheckBox(L"Check Box", Rect(0, 2, 80, 22), parent)
			{

			}

			void setText(const String& text)
			{
				m_label->setText(text);
			}

			CheckButton* getCheckButton()
			{
				return m_button;
			}

			Label* getLabel()
			{
				return m_label;
			}
		protected:

			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				_Adjust_Check_Box_Layout();
			}

			virtual void onPaintEvent() override
			{
				auto painter = getPainter();
				drawBackground();
				drawChild(m_button);
				drawChild(m_label);
				drawFrame();
			}

		};



	}

}


#endif // !CRAFT_ENGINE_GUI_CHECK_BOX_H_


