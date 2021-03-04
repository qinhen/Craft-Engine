#pragma once

#include "./PushButton.h"
#include "./TextLine.h"

namespace CraftEngine
{
	namespace gui
	{

		class AbstractValueInput : public Widget
		{
		protected:
			String m_valueName;
			TextLine* m_valueTextLine;
			PushButton* m_showTextView;
			bool m_proxyMode = false;

		protected:

			virtual bool fromString(const String& str) = 0;
			virtual String toString() = 0;

			String _Get_Text()
			{
				if (m_valueName.size() == 0)
					return toString();
				return m_valueName + L":" + toString();
			}

			void _Set_Value_String()
			{
				m_valueTextLine->setText(toString());
				m_showTextView->setText(_Get_Text());
			}

			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				m_valueTextLine->setRect(Rect(Size(0), getSize()));
				m_showTextView->setRect(Rect(Size(0), getSize()));
			}

			virtual Widget* onDetect(const MouseEvent& mouseEvent) override
			{
				if (isHide() || isTransparent() || (isPopupable() && !isNowPopup()) || !getRect().inside(mouseEvent.local))
					return nullptr;

				Widget* hit;
				MouseEvent subEvent = mouseEvent;
				subEvent.local = mouseEvent.local - getPos();
				if (m_proxyMode)
					hit = m_valueTextLine->onMouseEvent(subEvent);
				else
					hit = m_showTextView->onMouseEvent(subEvent);

				if (hit != nullptr)
					return hit;
				return this;
			}
		public:

			void setValueName(const String& name)
			{
				m_valueName = name;
				m_showTextView->setText(_Get_Text());
				m_valueTextLine->setText(toString());
			}

			AbstractValueInput(Widget* parent) : Widget(Rect(0, 0, 80, 22), parent)
			{
				m_showTextView = new PushButton(this);
				m_valueTextLine = new TextLine(this);
				m_valueTextLine->getPalette().mFillMode = Palette::eFillMode_Color;
				m_showTextView->getFont().getAlignment().setHorizonAlignment(Alignment::eAlignmentMode_Center);
				m_valueTextLine->getFont().getAlignment().setHorizonAlignment(Alignment::eAlignmentMode_Center);
				m_valueTextLine->setRect(getRect());
				m_showTextView->setRect(getRect());


				craft_engine_gui_connect_v2(m_showTextView, clicked, [=]() {
					m_proxyMode = true;
					m_valueTextLine->forceInputFocus(true);
					m_valueTextLine->selectAllText();
				});

				craft_engine_gui_connect_v2(m_valueTextLine, editReady, [=]() {
					m_proxyMode = true;
					m_valueTextLine->selectAllText();
				});

				craft_engine_gui_connect_v2(m_valueTextLine, editFinished, [=](const String& str) {
					if (fromString(str))
					{
						m_showTextView->setText(_Get_Text());
						m_valueTextLine->setText(toString());
						m_proxyMode = false;
						m_valueTextLine->forceInputFocus(false);
					}
					else
					{
						m_proxyMode = false;
						_Set_Value_String();//
					}
				});

				craft_engine_gui_connect_v2(m_valueTextLine, enterPressed, [=](const String& str) {
					if (fromString(str))
					{
						m_showTextView->setText(_Get_Text());
						m_valueTextLine->setText(toString());
						m_proxyMode = false;
						m_valueTextLine->forceInputFocus(false);
					}
					else
					{
						m_proxyMode = false;
						_Set_Value_String();//
					}
				});
			}

			virtual void onPaintEvent() override
			{
				if (m_proxyMode)
					drawChild2(m_valueTextLine);
				else
					drawChild2(m_showTextView);
			}

		};


		template<typename Type>
		class TemplateNumericInput: public AbstractValueInput
		{
		public:
			typedef Type ValueType;
		private:
			Type   m_curValue;
			Type   m_upperBound = std::numeric_limits<Type>::max();
			Type   m_lowerBound = std::numeric_limits<Type>::lowest();
			String m_format;
		protected:
			virtual bool fromString(const String& str)
			{
				Type val;
				bool ok = StringTool::toValue<Type>(str, &val);
				if (ok)
				{
					m_curValue = val;
					_Apply_Bound();
					craft_engine_gui_emit(valueChanged, m_curValue);
				}
				return ok;
			}
			virtual String toString()
			{
				if (m_format.size() > 0)
					return StringTool::fromValue<Type>(m_curValue, m_format);
				return StringTool::fromValue<Type>(m_curValue);
			}
		public:
			craft_engine_gui_signal(valueChanged, void(const Type& value));

			TemplateNumericInput(Widget* parent) : AbstractValueInput(parent)
			{
				m_showTextView->setText(_Get_Text());
				m_valueTextLine->setText(toString());
			}

			void setFormat(const String& format)
			{
				m_format = format;
				m_showTextView->setText(_Get_Text());
				m_valueTextLine->setText(toString());
			}

			Type getValue()const { return m_curValue; }

			void setValue(const Type& val)
			{
				m_curValue = val;
				_Apply_Bound();
				m_showTextView->setText(_Get_Text());
				m_valueTextLine->setText(toString());
			}

			void setBound(Type min, Type max)
			{
				m_upperBound = max;
				m_lowerBound = min;
				_Apply_Bound();
				m_showTextView->setText(_Get_Text());
				m_valueTextLine->setText(toString());
			}
		private:
			void _Apply_Bound()
			{
				m_curValue = math::clamp(m_curValue, m_lowerBound, m_upperBound);
			}
		};


		template<size_t Length, typename Type>
		class TemplateVectorValueInput: public Widget
		{
		public:
			enum ValueMode
			{
				eValueMode_RGBA,
				eValueMode_XYZW, 
			};
			typedef Type ValueType;
			typedef math::Vector<Length, Type> VectorType;
		private:
			typedef TemplateNumericInput<Type> ValueInputWdiget;
			ValueInputWdiget* m_valueInputs[Length];
			VectorType        m_value;
			ValueMode         m_valueMode = ValueMode::eValueMode_XYZW;
			VectorType        m_upperBound = VectorType(std::numeric_limits<Type>::max());
			VectorType        m_lowerBound = VectorType(std::numeric_limits<Type>::lowest());
			String            m_format;
		public:
			craft_engine_gui_signal(valueChanged, void(const VectorType& value));

			TemplateVectorValueInput(Widget* parent) : Widget(Rect(0, 0, 200, 22), parent)
			{
				static const Char* val_names_xyzw[] = {
					L"X",L"Y",L"Z",L"W",
				};
				static const Char* val_names_rgba[] = {
					L"R",L"G",L"B",L"A",
				};
				auto val_names = m_valueMode == ValueMode::eValueMode_XYZW ? val_names_xyzw : val_names_rgba;
				for (int i = 0; i < Length; i++)
				{
					m_valueInputs[i] = new ValueInputWdiget(this);
					craft_engine_gui_connect_v2(m_valueInputs[i], valueChanged, [=](const Type& val) {
						m_value[i] = val;
						craft_engine_gui_emit(valueChanged, m_value);
					});
					m_valueInputs[i]->setValueName(val_names[i]);
					m_value[i] = m_valueInputs[i]->getValue();
				}
				updateValueInputLayout();
			}

			void setFormat(const String& format)
			{
				for (int i = 0; i < Length; i++)
					m_valueInputs[i]->setFormat(format);
			}

			VectorType getValue()const { return m_value; }

			void setValue(const VectorType& val)
			{
				m_value = val;
				for (int i = 0; i < Length; i++)
					m_valueInputs[i]->setValue(m_value[i]);
			}

			void setBound(VectorType min, VectorType max)
			{
				m_upperBound = max;
				m_lowerBound = min;
				for (int i = 0; i < Length; i++)
					m_valueInputs[i]->setBound(m_lowerBound[i], m_upperBound[i]);
			}
		private:
			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				updateValueInputLayout();
			}

			void updateValueInputLayout()
			{
				int x = 0;
				int w = getWidth() / Length;
				for (int i = 0; i < Length - 1; i++)
					m_valueInputs[i]->setRect(Rect(w * i, 0, w, getHeight()));
				m_valueInputs[Length - 1]->setRect(Rect(w * (Length - 1), 0, getWidth() - w * (Length - 1), getHeight()));
			}
		};

		typedef TemplateNumericInput<int32_t> ValueInputInt;
		typedef TemplateNumericInput<float_t> ValueInputFloat;
		typedef TemplateNumericInput<double_t> ValueInputDouble;
		typedef TemplateVectorValueInput<2, int32_t> ValueInputInt2;
		typedef TemplateVectorValueInput<3, int32_t> ValueInputInt3;
		typedef TemplateVectorValueInput<4, int32_t> ValueInputInt4;
		typedef TemplateVectorValueInput<2, float_t> ValueInputFloat2;
		typedef TemplateVectorValueInput<3, float_t> ValueInputFloat3;
		typedef TemplateVectorValueInput<4, float_t> ValueInputFloat4;
		typedef TemplateVectorValueInput<2, double_t> ValueInputDouble2;
		typedef TemplateVectorValueInput<3, double_t> ValueInputDouble3;
		typedef TemplateVectorValueInput<4, double_t> ValueInputDouble4;

	}
}