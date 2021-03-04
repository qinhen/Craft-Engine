#pragma once
#ifndef CRAFT_ENGINE_GUI_SPIN_BOX_H_
#define CRAFT_ENGINE_GUI_SPIN_BOX_H_
#include "./Label.h"
#include "./TextLine.h"
#include "./PushButton.h"

namespace CraftEngine
{
	namespace gui
	{
		
		class AbstractSpinBox : public Widget
		{
		private:

			TextLine* m_editableText;
			PushButton* m_upButton;
			PushButton* m_downButton;
		public:

			~AbstractSpinBox()
			{

			}

			AbstractSpinBox(const Rect& rect, Widget* parent) :Widget(rect, parent)
			{
				m_editableText = new TextLine(this);
				m_upButton = new PushButton(this);
				m_downButton = new PushButton(this);
				//m_activateButton->getPalette().mFrameMode = Palette::eFrameMode_Surround;
				m_upButton->setIcon(GuiAssetsManager::loadImage("Triangle_Up"), getPalette().mForegroundColor);
				m_downButton->setIcon(GuiAssetsManager::loadImage("Triangle_Down"), getPalette().mForegroundColor);
				_Adjust_Spin_Box_Layout();
			
				craft_engine_gui_connect(m_upButton, clicked, this, onUpButtonClicked);
				craft_engine_gui_connect(m_downButton, clicked, this, onDownButtonClicked);

				craft_engine_gui_connect(m_editableText, editReady, this, onTextLineEditBegin);
				craft_engine_gui_connect(m_editableText, editFinished, this, onTextLineEditFinished);
				craft_engine_gui_connect(m_editableText, enterPressed, this, onTextLineEditFinished);
			}

			AbstractSpinBox(Widget* parent) :AbstractSpinBox(Rect(0, 2, 80, 22), parent)
			{

			}

			TextLine* getTextEdit()
			{
				return m_editableText;
			}

		protected:

			virtual void onTextLineEditBegin() = 0;
			virtual void onTextLineEditFinished(const String&) = 0;
			virtual void onUpButtonClicked() = 0;
			virtual void onDownButtonClicked() = 0;


			void _Adjust_Spin_Box_Layout()
			{
				const Size& size = getSize();
				m_editableText->setRect(Rect(0, 0, size.x, size.y));
				int h = (size.y - 3) / 2;
				m_upButton->setRect(Rect(size.x - size.y + 2, 2, size.y - 3, h));
				int h2 = size.y - h - 3;
				if(h2 > h)
					m_downButton->setRect(Rect(size.x - size.y + 2, 3 + h, size.y - 3, h));
				else
					m_downButton->setRect(Rect(size.x - size.y + 2, 2 + h, size.y - 3, size.y - h - 3));
			}

	
			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				_Adjust_Spin_Box_Layout();
			}


			virtual void onPaintEvent() override
			{
				drawChild2(m_editableText);
				drawChild(m_upButton);
				drawChild(m_downButton);
			}

		};



		template<class Type>
		class TemplateNumericSpinBox: public AbstractSpinBox
		{
		private:
			Type m_upperBound = std::numeric_limits<Type>::max();
			Type m_lowerBound = std::numeric_limits<Type>::lowest();
			Type m_curValue = Type(0);
			Type m_step = Type(1);


			void _Update_String()
			{
				getTextEdit()->setText(StringTool::fromStdWString(std::to_wstring(m_curValue)));
			}
			void _Apply_Bound()
			{
				m_curValue = math::clamp(m_curValue, m_lowerBound, m_upperBound);
			}
		public:
			craft_engine_gui_signal(valueChanged, void(const Type& value));

			TemplateNumericSpinBox(const Rect& rect, Widget* parent) :AbstractSpinBox(rect, parent)
			{
				//std::cout << "max = " << m_upperBound << ", min = " << m_lowerBound << std::endl;
				_Apply_Bound();
				_Update_String();
			}
			TemplateNumericSpinBox(Widget* parent) :TemplateNumericSpinBox(Rect(0, 2, 80, 22), parent)
			{

			}

			Type getValue()const
			{
				return m_curValue;
			}

			void setStep(Type val)
			{
				m_step = val;
			}

			void setValue(Type val)
			{
				m_curValue = val;
				_Apply_Bound();
				_Update_String();
			}

			void setBound(Type min, Type max)
			{
				m_upperBound = max;
				m_lowerBound = min;
				_Apply_Bound();
				_Update_String();
			}

		protected:
			virtual void onTextLineEditBegin() override
			{ 
				getTextEdit()->selectAllText();
			}

			virtual void onTextLineEditFinished(const String& str) override
			{
				Type tempValue = m_curValue;
				Type val;
				if (StringTool::toValue<Type>(str, &val))
					m_curValue = val;
				_Apply_Bound();
				_Update_String();
				if (tempValue != m_curValue)
					craft_engine_gui_emit(valueChanged, m_curValue);
			}

			virtual void onUpButtonClicked() override
			{
				Type tempValue = m_curValue;
				m_curValue += m_step;
				_Apply_Bound();
				_Update_String();
				if (tempValue != m_curValue)
					craft_engine_gui_emit(valueChanged, m_curValue);
			}
			virtual void onDownButtonClicked() override
			{
				Type tempValue = m_curValue;
				m_curValue -= m_step;
				_Apply_Bound();
				_Update_String();
				if (tempValue != m_curValue)
					craft_engine_gui_emit(valueChanged, m_curValue);
			}

			virtual void onWheel(const MouseEvent& mouseEvent) override
			{
				if (mouseEvent.offset.y < 0)
				{
					Type tempValue = m_curValue;
					m_curValue -= m_step;
					_Apply_Bound();
					_Update_String();
					if (tempValue != m_curValue)
						craft_engine_gui_emit(valueChanged, m_curValue);
				}
				else if (mouseEvent.offset.y > 0)
				{
					Type tempValue = m_curValue;
					m_curValue += m_step;
					_Apply_Bound();
					_Update_String();
					if (tempValue != m_curValue)
						craft_engine_gui_emit(valueChanged, m_curValue);
				}
			}
		};

		typedef TemplateNumericSpinBox<int>   SpinBoxInt;
		typedef TemplateNumericSpinBox<float> SpinBoxFloat;

	}

}





#endif // !CRAFT_ENGINE_GUI_SPIN_BOX_H_


