#pragma once
#include "./Widget.h"
#include "./ValueInput.h"
#include "./CheckButton.h"
#include "./ColorInput.h"
#include "./TextLine.h"
#include "./ComboBox.h"
#include "./CheckBox.h"
namespace CraftEngine
{
	namespace gui
	{

		class DataBinderWidget : public Widget
		{
		private:

			enum DataBindingType
			{
				eNothing_Nullptr,
				eBool_CheckButton,
				eBools_CheckBoxs, // bools
				eInt_ComboBox, // int
				eInt_RadioButtons, // int
				eInt_ValueInputInt,
				eFloat_ValueInputFloat,
				eDouble_ValueInputDouble,
				eInt2_ValueInputInt2,
				eInt3_ValueInputInt3,
				eInt4_ValueInputInt4,
				eFloat2_ValueInputFloat2,
				eFloat3_ValueInputFloat3,
				eFloat4_ValueInputFloat4,
				eDouble2_ValueInputDouble2,
				eDouble3_ValueInputDouble3,
				eDouble4_ValueInputDouble4,
				eColor_ColorInput,
				eString_TextLine,
			};

			DataBindingType m_dataType = eNothing_Nullptr;

			union DataPointer
			{
				bool*   pBool;
				int*    pInt;
				float*  pFloat;
				double* pDouble;
				Color*  pColor;
				String* pString;
				void*   pAny;
			}m_dataPointer;

			union BinderWidget
			{
				CheckButton* mCheckButton;
				CheckBox** mCheckBoxs;
				ValueInputInt* mValueInputInt;
				ValueInputFloat* mValueInputFloat;
				ValueInputDouble* mValueInputDouble;
				ValueInputInt2* mValueInputInt2;
				ValueInputInt3* mValueInputInt3;
				ValueInputInt4* mValueInputInt4;
				ValueInputFloat2* mValueInputFloat2;
				ValueInputFloat3* mValueInputFloat3;
				ValueInputFloat4* mValueInputFloat4;
				ValueInputDouble2* mValueInputDouble2;
				ValueInputDouble3* mValueInputDouble3;
				ValueInputDouble4* mValueInputDouble4;
				ColorInput* mColorInput;
				TextLine* mTextLine;
				ComboBox* mComboBox;
			}m_binderWidget;
			//int m_binderDataCount;
			int m_binderWidgetCount;
			String m_dataName;
		public:
			DataBinderWidget(Widget* parent) : Widget(parent)
			{
				sizeof(BinderWidget);
			}

			void setDataName(const String& name)
			{
				m_dataName = name;
			}

			const String& getDataName() const
			{
				return m_dataName;
			}

			void bindBools(bool ptr[], const StringList& strings, const String& name);
			void bindInt(int* ptr, const StringList& strings, const String& name);

			void bindBool(bool* ptr, const String& name = String());
			void bindColor(Color* ptr, const String& name = String());
			void bindString(String* ptr, const String& name = String());

			void bindFloat(float* ptr, const String& name = String());
			void bindFloat2(float* ptr, const String& name = String());
			void bindFloat3(float* ptr, const String& name = String());
			void bindFloat4(float* ptr, const String& name = String());
			void bindDouble(double* ptr, const String& name = String());
			void bindDouble2(double* ptr, const String& name = String());
			void bindDouble3(double* ptr, const String& name = String());
			void bindDouble4(double* ptr, const String& name = String());
			void bindInt(int* ptr, const String& name = String());
			void bindInt2(int* ptr, const String& name = String());
			void bindInt3(int* ptr, const String& name = String());
			void bindInt4(int* ptr, const String& name = String());

			void setDataFormat(const String& format);

			void rebindData(void* ptr)
			{
				m_dataPointer.pAny = ptr;
				updateData();
			}
			void unbindData();
			void updateData();
		protected:

			void updateBinderLayout();

			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override 
			{
				updateBinderLayout();
			}
		};



		class DataBinderListWidget : public Widget
		{
		private:
			core::ArrayList<DataBinderWidget*> m_binderList;
			core::ArrayList<Label*> m_binderLabelList;
		public:

			DataBinderListWidget(Widget* parent) : Widget(parent)
			{

			}
			void updateData(int i)
			{
				m_binderList[i]->updateData();
			}
			void setDataFormat(int i, const String& format)
			{
				m_binderList[i]->setDataFormat(format);
			}
			void clearBindings() 
			{
				for (auto it : m_binderLabelList)
					delete it;
				m_binderLabelList.clear();
				for (auto it : m_binderList)
					delete it;
				m_binderList.clear();
				updateDataBinderWidgetsLayout();
			}
			void bindBools(bool ptr[], const StringList& strings, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindBools(ptr, strings, name);
				updateDataBinderWidgetsLayout();
			}
			void bindInt(int* ptr, const StringList& strings, const String& name) 
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindInt(ptr, strings, name);
				updateDataBinderWidgetsLayout();
			}
			void bindBool(bool* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindBool(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindColor(Color* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindColor(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindString(String* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindString(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindFloat(float* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindFloat(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindFloat2(float* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindFloat2(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindFloat3(float* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindFloat3(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindFloat4(float* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindFloat4(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindDouble(double* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindDouble(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindDouble2(double* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindDouble2(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindDouble3(double* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindDouble3(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindDouble4(double* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindDouble4(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindInt(int* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindInt(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindInt2(int* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindInt2(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindInt3(int* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindInt3(ptr, name);
				updateDataBinderWidgetsLayout();
			}
			void bindInt4(int* ptr, const String& name)
			{
				auto widget = new DataBinderWidget(this);
				m_binderList.push_back(widget);
				widget->bindInt4(ptr, name);
				updateDataBinderWidgetsLayout();
			}
		protected:

			void updateDataBinderWidgetsLayout()
			{
				if (m_binderLabelList.size() < m_binderList.size())
				{
					m_binderLabelList.push_back(new Label(this));
				}
				else if (m_binderLabelList.size() > m_binderList.size())
				{
					delete m_binderLabelList.back();
					m_binderLabelList.pop_back();
				}
				int y = 0, h = 0;
				for (int i = 0; i< m_binderList.size(); i++)
				{
					h = m_binderList[i]->getMinSizeLimit().y;
					if (i < m_binderLabelList.size())
					{
						m_binderLabelList[i]->setRect(Rect(0, y, getWidth(), h));
						m_binderLabelList[i]->setText(m_binderList[i]->getDataName());
					}
					y += h;
					m_binderList[i]->setRect(Rect(0, y, getWidth(), h));
					y += h;
				}
				setMinSizeLimit(Size(0, y));
			}

			virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
			{
				updateDataBinderWidgetsLayout();
			}
		};



	}
}





namespace CraftEngine
{
	namespace gui
	{
		inline void DataBinderWidget::bindBools(bool ptr[], const StringList& strings, const String& name)
		{
			unbindData();
			setDataName(name);
			m_dataType = eBools_CheckBoxs;
			m_dataPointer.pBool = ptr;
			m_binderWidget.mCheckBoxs = new CheckBox * [strings.size()];
			m_binderWidgetCount = strings.size();
			auto it = strings.begin();
			for (int i = 0; i < strings.size(); i++, it++)
			{
				auto widget = new CheckBox(this);
				m_binderWidget.mCheckBoxs[i] = widget;
				widget->setText(*it);
				widget->getCheckButton()->setCheckState(m_dataPointer.pBool[i] ? CheckButton::eCheckState_Selected : CheckButton::eCheckState_Unselected);
				craft_engine_gui_connect_v2(widget->getCheckButton(), clicked, [=]() {
					m_dataPointer.pBool[i] = m_binderWidget.mCheckBoxs[i]->getCheckButton()->getCheckState() == CheckButton::eCheckState_Selected ? true : false;
				});
			}
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindInt(int* ptr, const StringList& strings, const String& name)
		{
			unbindData();
			setDataName(name);
			m_dataType = eInt_ComboBox;
			m_dataPointer.pInt = ptr;
			m_binderWidget.mComboBox = new ComboBox(this);
			m_binderWidgetCount = 1;

			m_binderWidget.mComboBox->setItems(strings);
			m_binderWidget.mComboBox->setEditable(false); // 
			m_binderWidget.mComboBox->setCurrentIndex(m_dataPointer.pInt[0]);
			craft_engine_gui_connect_v2(m_binderWidget.mComboBox, selectionChanged, [=](int index) { m_dataPointer.pInt[0] = index; });

			updateBinderLayout();
		}
		inline void DataBinderWidget::bindBool(bool* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			m_dataType = eBool_CheckButton;
			m_dataPointer.pBool = ptr;
			m_binderWidget.mCheckButton = new CheckButton(this);
			m_binderWidgetCount = 1;

			m_binderWidget.mCheckButton->setCheckState(m_dataPointer.pBool[0] ? CheckButton::eCheckState_Selected : CheckButton::eCheckState_Unselected);
			craft_engine_gui_connect_v2(m_binderWidget.mCheckButton, clicked, [=]() {
				m_dataPointer.pBool[0] = m_binderWidget.mCheckButton->getCheckState() == CheckButton::eCheckState_Selected ? true : false;
			});

			updateBinderLayout();
		}
		inline void DataBinderWidget::bindColor(Color* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new ColorInput(this);
			m_dataType = eColor_ColorInput;
			m_dataPointer.pColor = ptr;
			m_binderWidget.mColorInput = widget;
			m_binderWidgetCount = 1;
			widget->setColor(ptr[0]);
			craft_engine_gui_connect_v2(widget, colorChanged, [=](const Color& color) {
				m_dataPointer.pColor[0] = color;
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindString(String* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new TextLine(this);
			m_dataType = eString_TextLine;
			m_dataPointer.pString = ptr;
			m_binderWidget.mTextLine = widget;
			m_binderWidgetCount = 1;
			widget->setText(ptr[0]);
			craft_engine_gui_connect_v2(widget, textChanged, [=](const String& text) {
				m_dataPointer.pString[0] = text;
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindFloat(float* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new ValueInputFloat(this);
			m_dataType = eFloat_ValueInputFloat;
			m_dataPointer.pFloat = ptr;
			m_binderWidget.mValueInputFloat = widget;
			m_binderWidgetCount = 1;
			widget->setValue(ptr[0]);
			craft_engine_gui_connect_v2(widget, valueChanged, [=](float val) {
				m_dataPointer.pFloat[0] = val;
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindFloat2(float* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new ValueInputFloat2(this);
			m_dataType = eFloat2_ValueInputFloat2;
			m_dataPointer.pFloat = ptr;
			m_binderWidget.mValueInputFloat2 = widget;
			m_binderWidgetCount = 1;
			using ValueType = std::decay_t<decltype(*widget)>::VectorType;
			widget->setValue(((ValueType*)ptr)[0]);
			craft_engine_gui_connect_v2(widget, valueChanged, [=](const ValueType& val) {
				m_dataPointer.pFloat[0] = val[0];
				m_dataPointer.pFloat[1] = val[1];
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindFloat3(float* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new ValueInputFloat3(this);
			m_dataType = eFloat3_ValueInputFloat3;
			m_dataPointer.pFloat = ptr;
			m_binderWidget.mValueInputFloat3 = widget;
			m_binderWidgetCount = 1;
			using ValueType = std::decay_t<decltype(*widget)>::VectorType;
			widget->setValue(((ValueType*)ptr)[0]);
			craft_engine_gui_connect_v2(widget, valueChanged, [=](const ValueType& val) {
				m_dataPointer.pFloat[0] = val[0];
				m_dataPointer.pFloat[1] = val[1];
				m_dataPointer.pFloat[2] = val[2];
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindFloat4(float* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new ValueInputFloat4(this);
			m_dataType = eFloat4_ValueInputFloat4;
			m_dataPointer.pFloat = ptr;
			m_binderWidget.mValueInputFloat4 = widget;
			m_binderWidgetCount = 1;
			using ValueType = std::decay_t<decltype(*widget)>::VectorType;
			widget->setValue(((ValueType*)ptr)[0]);
			craft_engine_gui_connect_v2(widget, valueChanged, [=](const ValueType& val) {
				m_dataPointer.pFloat[0] = val[0];
				m_dataPointer.pFloat[1] = val[1];
				m_dataPointer.pFloat[2] = val[2];
				m_dataPointer.pFloat[3] = val[3];
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindDouble(double* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new ValueInputDouble(this);
			m_dataType = eDouble_ValueInputDouble;
			m_dataPointer.pDouble = ptr;
			m_binderWidget.mValueInputDouble = widget;
			m_binderWidgetCount = 1;
			widget->setValue(ptr[0]);
			craft_engine_gui_connect_v2(widget, valueChanged, [=](double val) {
				m_dataPointer.pDouble[0] = val;
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindDouble2(double* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new ValueInputDouble2(this);
			m_dataType = eDouble2_ValueInputDouble2;
			m_dataPointer.pDouble = ptr;
			m_binderWidget.mValueInputDouble2 = widget;
			m_binderWidgetCount = 1;
			using ValueType = std::decay_t<decltype(*widget)>::VectorType;
			widget->setValue(((ValueType*)ptr)[0]);
			craft_engine_gui_connect_v2(widget, valueChanged, [=](const ValueType& val) {
				m_dataPointer.pDouble[0] = val[0];
				m_dataPointer.pDouble[1] = val[1];
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindDouble3(double* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new ValueInputDouble3(this);
			m_dataType = eDouble3_ValueInputDouble3;
			m_dataPointer.pDouble = ptr;
			m_binderWidget.mValueInputDouble3 = widget;
			m_binderWidgetCount = 1;
			using ValueType = std::decay_t<decltype(*widget)>::VectorType;
			widget->setValue(((ValueType*)ptr)[0]);
			craft_engine_gui_connect_v2(widget, valueChanged, [=](const ValueType& val) {
				m_dataPointer.pDouble[0] = val[0];
				m_dataPointer.pDouble[1] = val[1];
				m_dataPointer.pDouble[2] = val[2];
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindDouble4(double* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new ValueInputDouble4(this);
			m_dataType = eDouble4_ValueInputDouble4;
			m_dataPointer.pDouble = ptr;
			m_binderWidget.mValueInputDouble4 = widget;
			m_binderWidgetCount = 1;
			using ValueType = std::decay_t<decltype(*widget)>::VectorType;
			widget->setValue(((ValueType*)ptr)[0]);
			craft_engine_gui_connect_v2(widget, valueChanged, [=](const ValueType& val) {
				m_dataPointer.pDouble[0] = val[0];
				m_dataPointer.pDouble[1] = val[1];
				m_dataPointer.pDouble[2] = val[2];
				m_dataPointer.pDouble[3] = val[3];
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindInt(int* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new ValueInputInt(this);
			m_dataType = eInt_ValueInputInt;
			m_dataPointer.pInt = ptr;
			m_binderWidget.mValueInputInt = widget;
			m_binderWidgetCount = 1;
			widget->setValue(ptr[0]);
			craft_engine_gui_connect_v2(widget, valueChanged, [=](int val) {
				m_dataPointer.pInt[0] = val;
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindInt2(int* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new ValueInputInt2(this);
			m_dataType = eInt2_ValueInputInt2;
			m_dataPointer.pInt = ptr;
			m_binderWidget.mValueInputInt2 = widget;
			m_binderWidgetCount = 1;
			using ValueType = std::decay_t<decltype(*widget)>::VectorType;
			widget->setValue(((ValueType*)ptr)[0]);
			craft_engine_gui_connect_v2(widget, valueChanged, [=](const ValueType& val) {
				m_dataPointer.pInt[0] = val[0];
				m_dataPointer.pInt[1] = val[1];
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindInt3(int* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new ValueInputInt3(this);
			m_dataType = eInt3_ValueInputInt3;
			m_dataPointer.pInt = ptr;
			m_binderWidget.mValueInputInt3 = widget;
			m_binderWidgetCount = 1;
			using ValueType = std::decay_t<decltype(*widget)>::VectorType;
			widget->setValue(((ValueType*)ptr)[0]);
			craft_engine_gui_connect_v2(widget, valueChanged, [=](const ValueType& val) {
				m_dataPointer.pInt[0] = val[0];
				m_dataPointer.pInt[1] = val[1];
				m_dataPointer.pInt[2] = val[2];
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::bindInt4(int* ptr, const String& name)
		{
			unbindData();
			setDataName(name);
			auto widget = new ValueInputInt4(this);
			m_dataType = eInt4_ValueInputInt4;
			m_dataPointer.pInt = ptr;
			m_binderWidget.mValueInputInt4 = widget;
			m_binderWidgetCount = 1;
			using ValueType = std::decay_t<decltype(*widget)>::VectorType;
			widget->setValue(((ValueType*)ptr)[0]);
			craft_engine_gui_connect_v2(widget, valueChanged, [=](const ValueType& val) {
				m_dataPointer.pInt[0] = val[0];
				m_dataPointer.pInt[1] = val[1];
				m_dataPointer.pInt[2] = val[2];
				m_dataPointer.pInt[3] = val[3];
			});
			updateBinderLayout();
		}
		inline void DataBinderWidget::setDataFormat(const String& format)
		{
			switch (m_dataType)
			{
			case eNothing_Nullptr:
				break;
			case eBool_CheckButton:
				break;
			case eBools_CheckBoxs:
				break;
			case eInt_ValueInputInt:
				m_binderWidget.mValueInputInt->setFormat(format);
				break;
			case eFloat_ValueInputFloat:
				m_binderWidget.mValueInputFloat->setFormat(format);
				break;
			case eDouble_ValueInputDouble:
				m_binderWidget.mValueInputDouble->setFormat(format);
				break;
			case eInt2_ValueInputInt2:
				m_binderWidget.mValueInputInt2->setFormat(format);
				break;
			case eInt3_ValueInputInt3:
				m_binderWidget.mValueInputInt3->setFormat(format);
				break;
			case eInt4_ValueInputInt4:
				m_binderWidget.mValueInputInt4->setFormat(format);
				break;
			case eFloat2_ValueInputFloat2:
				m_binderWidget.mValueInputFloat2->setFormat(format);
				break;
			case eFloat3_ValueInputFloat3:
				m_binderWidget.mValueInputFloat3->setFormat(format);
				break;
			case eFloat4_ValueInputFloat4:
				m_binderWidget.mValueInputFloat4->setFormat(format);
				break;
			case eDouble2_ValueInputDouble2:
				m_binderWidget.mValueInputDouble2->setFormat(format);
				break;
			case eDouble3_ValueInputDouble3:
				m_binderWidget.mValueInputDouble3->setFormat(format);
				break;
			case eDouble4_ValueInputDouble4:
				m_binderWidget.mValueInputDouble4->setFormat(format);
				break;
			case eColor_ColorInput:
				break;
			case eString_TextLine:
				break;
			case eInt_ComboBox:
				break;
			case eInt_RadioButtons:
				// TODO
				break;
			default:
				// error
				break;
			}
		}
		inline void DataBinderWidget::unbindData()
		{
			switch (m_dataType)
			{
			case eNothing_Nullptr:
				break;
			case eBool_CheckButton:
				delete m_binderWidget.mCheckButton;
				break;
			case eBools_CheckBoxs:
				for (int i = 0; i < m_binderWidgetCount; i++)
					delete m_binderWidget.mCheckBoxs[i];
				delete m_binderWidget.mCheckBoxs;
				break;
			case eInt_ValueInputInt:
				delete m_binderWidget.mValueInputInt;
				break;
			case eFloat_ValueInputFloat:
				delete m_binderWidget.mValueInputFloat;
				break;
			case eDouble_ValueInputDouble:
				delete m_binderWidget.mValueInputDouble;
				break;
			case eInt2_ValueInputInt2:
				delete m_binderWidget.mValueInputInt2;
				break;
			case eInt3_ValueInputInt3:
				delete m_binderWidget.mValueInputInt3;
				break;
			case eInt4_ValueInputInt4:
				delete m_binderWidget.mValueInputInt4;
				break;
			case eFloat2_ValueInputFloat2:
				delete m_binderWidget.mValueInputFloat2;
				break;
			case eFloat3_ValueInputFloat3:
				delete m_binderWidget.mValueInputFloat3;
				break;
			case eFloat4_ValueInputFloat4:
				delete m_binderWidget.mValueInputFloat4;
				break;
			case eDouble2_ValueInputDouble2:
				delete m_binderWidget.mValueInputDouble2;
				break;
			case eDouble3_ValueInputDouble3:
				delete m_binderWidget.mValueInputDouble3;
				break;
			case eDouble4_ValueInputDouble4:
				delete m_binderWidget.mValueInputDouble4;
				break;
			case eColor_ColorInput:
				delete m_binderWidget.mColorInput;
				break;
			case eString_TextLine:
				delete m_binderWidget.mTextLine;
				break;
			case eInt_ComboBox:
				delete m_binderWidget.mComboBox;
				break;
			case eInt_RadioButtons:
				// TODO
				break;
			default:
				// error
				break;
			}
			m_binderWidgetCount = 0;
			m_dataType = eNothing_Nullptr;
		}
		inline void DataBinderWidget::updateData()
		{
			switch (m_dataType)
			{
			case eNothing_Nullptr:
				break;
			case eBool_CheckButton:
				m_binderWidget.mCheckButton->setCheckState(m_dataPointer.pBool[0] ? CheckButton::eCheckState_Selected : CheckButton::eCheckState_Unselected);
				break;
			case eBools_CheckBoxs:
				for (int i = 0; i < m_binderWidgetCount; i++)
					m_binderWidget.mCheckBoxs[i]->getCheckButton()->setCheckState(m_dataPointer.pBool[i] ? CheckButton::eCheckState_Selected : CheckButton::eCheckState_Unselected);
				break;
			case eInt_ValueInputInt:
				m_binderWidget.mValueInputInt->setValue(m_dataPointer.pInt[0]);
				break;
			case eFloat_ValueInputFloat:
				m_binderWidget.mValueInputFloat->setValue(m_dataPointer.pFloat[0]);
				break;
			case eDouble_ValueInputDouble:
				m_binderWidget.mValueInputDouble->setValue(m_dataPointer.pDouble[0]);
				break;
			case eInt2_ValueInputInt2:
			{
				auto widget = m_binderWidget.mValueInputInt2;
				widget->setValue(((std::decay_t<decltype(*widget)>::VectorType*)m_dataPointer.pInt)[0]);
				break;
			}
			case eInt3_ValueInputInt3:
			{
				auto widget = m_binderWidget.mValueInputInt3;
				widget->setValue(((std::decay_t<decltype(*widget)>::VectorType*)m_dataPointer.pInt)[0]);
				break;
			}
			case eInt4_ValueInputInt4:
			{
				auto widget = m_binderWidget.mValueInputInt4;
				widget->setValue(((std::decay_t<decltype(*widget)>::VectorType*)m_dataPointer.pInt)[0]);
				break;
			}
			case eFloat2_ValueInputFloat2:
			{
				auto widget = m_binderWidget.mValueInputFloat2;
				widget->setValue(((std::decay_t<decltype(*widget)>::VectorType*)m_dataPointer.pFloat)[0]);
				break;
			}
			case eFloat3_ValueInputFloat3:
			{
				auto widget = m_binderWidget.mValueInputFloat3;
				widget->setValue(((std::decay_t<decltype(*widget)>::VectorType*)m_dataPointer.pFloat)[0]);
				break;
			}
			case eFloat4_ValueInputFloat4:
			{
				auto widget = m_binderWidget.mValueInputFloat4;
				widget->setValue(((std::decay_t<decltype(*widget)>::VectorType*)m_dataPointer.pFloat)[0]);
				break;
			}
			case eDouble2_ValueInputDouble2:
			{
				auto widget = m_binderWidget.mValueInputDouble2;
				widget->setValue(((std::decay_t<decltype(*widget)>::VectorType*)m_dataPointer.pDouble)[0]);
				break;
			}
			case eDouble3_ValueInputDouble3:
			{
				auto widget = m_binderWidget.mValueInputDouble3;
				widget->setValue(((std::decay_t<decltype(*widget)>::VectorType*)m_dataPointer.pDouble)[0]);
				break;
			}
			case eDouble4_ValueInputDouble4:
			{
				auto widget = m_binderWidget.mValueInputDouble4;
				widget->setValue(((std::decay_t<decltype(*widget)>::VectorType*)m_dataPointer.pDouble)[0]);
				break;
			}
			case eColor_ColorInput:
				m_binderWidget.mColorInput->setColor(m_dataPointer.pColor[0]);
				break;
			case eString_TextLine:
				m_binderWidget.mTextLine->setText(m_dataPointer.pString[0]);
				break;
			case eInt_ComboBox:
				m_binderWidget.mComboBox->setCurrentIndex(m_dataPointer.pInt[0]);
				break;
			case eInt_RadioButtons:
				// TODO
				break;
			default:
				// error
				break;
			}
		}
		void DataBinderWidget::updateBinderLayout()
		{
			switch (m_dataType)
			{
			case eNothing_Nullptr:
				break;
			case eBool_CheckButton:
				m_binderWidget.mCheckButton->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eBools_CheckBoxs:
				for (int i = 0; i < m_binderWidgetCount; i++)
					m_binderWidget.mCheckBoxs[i]->setRect(Rect(0, i * 22, getWidth(), 22));
				setMinSizeLimit(Size(0, m_binderWidgetCount * 22));
				break;
			case eInt_ValueInputInt:
				m_binderWidget.mValueInputInt->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eFloat_ValueInputFloat:
				m_binderWidget.mValueInputFloat->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eDouble_ValueInputDouble:
				m_binderWidget.mValueInputDouble->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eInt2_ValueInputInt2:
				m_binderWidget.mValueInputInt2->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eInt3_ValueInputInt3:
				m_binderWidget.mValueInputInt3->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eInt4_ValueInputInt4:
				m_binderWidget.mValueInputInt4->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eFloat2_ValueInputFloat2:
				m_binderWidget.mValueInputFloat2->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eFloat3_ValueInputFloat3:
				m_binderWidget.mValueInputFloat3->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eFloat4_ValueInputFloat4:
				m_binderWidget.mValueInputFloat4->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eDouble2_ValueInputDouble2:
				m_binderWidget.mValueInputDouble2->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eDouble3_ValueInputDouble3:
				m_binderWidget.mValueInputDouble3->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eDouble4_ValueInputDouble4:
				m_binderWidget.mValueInputDouble4->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eColor_ColorInput:
				m_binderWidget.mColorInput->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eString_TextLine:
				m_binderWidget.mTextLine->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eInt_ComboBox:
				m_binderWidget.mComboBox->setRect(Rect(0, 0, getWidth(), getHeight()));
				setMinSizeLimit(Size(0, 22));
				break;
			case eInt_RadioButtons:
				// TODO
				break;
			default:
				// error
				break;
			}
		}

	}
}