#pragma once

#include "./SpinBox.h"
#include "./Label.h"
#include "./ColorPickerWidget.h"
#include "./ValueInput.h"

namespace CraftEngine
{
	namespace gui
	{
		class ColorInput : public CraftEngine::gui::Widget
		{
		private:
			PushButton* m_colorWidget;
			//Label* m_labels[4];
			ValueInputInt* m_rgbaSpinBox[4];
			Color m_curColor;
			ColorPickerWidget* m_colorPicker;
		public:

			craft_engine_gui_signal(colorChanged, void(const Color& color));


			ColorInput(CraftEngine::gui::Widget* parent) : Widget(Rect(0,0,200,22), parent)
			{
				this->getPalette().mFrameMode = CraftEngine::gui::Palette::eFrameMode_Surround;

				m_colorWidget = new PushButton(this);
				m_colorWidget->getPalette().mFillMode = CraftEngine::gui::Palette::eFillMode_Color;
				m_colorWidget->getPalette().mFrameMode = CraftEngine::gui::Palette::eFrameMode_Surround;

				for (int i = 0; i < 4; i++)
				{
					//m_labels[i]->getFont().getAlignment().setHorizonAlignment(CraftEngine::gui::Alignment::eAlignmentMode_Center);
					m_rgbaSpinBox[i] = new ValueInputInt(this);
					m_rgbaSpinBox[i]->setBound(0, 255);
					craft_engine_gui_connect(m_rgbaSpinBox[i], valueChanged, this, _On_SpinBox_Value_Changed);
				}

				m_rgbaSpinBox[0]->setValueName(L"R");
				m_rgbaSpinBox[1]->setValueName(L"G");
				m_rgbaSpinBox[2]->setValueName(L"B");
				m_rgbaSpinBox[3]->setValueName(L"A");

				m_colorPicker = new ColorPickerWidget(this);
				m_colorPicker->setPopupable(true);
				craft_engine_gui_connect(m_colorWidget, clicked, this, _On_Color_Widget_Clicked);


				craft_engine_gui_connect_v2(m_colorPicker, colorChanged, [=](const Color& color)
				{
					m_curColor = color;
					m_colorWidget->getPalette().mBackgroundColor = color;
					for (int i = 0; i < 4; i++)
						m_rgbaSpinBox[i]->setValue(m_curColor[i]);
					craft_engine_gui_emit(colorChanged, m_curColor);
				});

				setColor(Color(255));
				_Adjust_Layout();
			}


			void setColor(const Color& color)
			{
				m_curColor = color;
				m_colorWidget->getPalette().mBackgroundColor = color;
				for (int i = 0; i < 4; i++)
					m_rgbaSpinBox[i]->setValue(m_curColor[i]);
				m_colorPicker->setColor(color);
			}

		private:

			void _On_Color_Widget_Clicked()
			{
				m_colorPicker->setRect(Rect(0, getHeight(), 250, 200));
				auto root = getRootWidget();
				if (root != nullptr)
				{
					if (m_colorPicker->isNowPopup())
						root->unpopupWidget(m_colorPicker);
					else
						root->popupWidget(m_colorPicker);
				}
			}

			void _On_SpinBox_Value_Changed(const int& val)
			{
				for (int i = 0; i < 4; i++)
					m_curColor[i] = m_rgbaSpinBox[i]->getValue();
				m_colorWidget->getPalette().mBackgroundColor = m_curColor;
				m_colorPicker->setColor(m_curColor);
				craft_engine_gui_emit(colorChanged, m_curColor);
			}

			virtual void onResizeEvent(const ResizeEvent& resizeEvent)override
			{
				_Adjust_Layout();
			}

			void _Adjust_Layout()
			{
				int x = 0;
				int w = getWidth() - getHeight();
				int pw = w / 4;
				int lw = w - pw * 3;
				//int sw = pw - lw;
				m_colorWidget->setRect(Rect(x, 0, getHeight(), getHeight()));
				x += getHeight();

				for (int i = 0; i < 3; i++)
				{
					//m_labels[i]->setRect(Rect(x, 0, lw, getHeight()));
					//x += lw;
					m_rgbaSpinBox[i]->setRect(Rect(x, 0, pw, getHeight()));
					x += pw;
				}

				//m_labels[3]->setRect(Rect(x, 0, lw, getHeight()));
				//x += lw;
				m_rgbaSpinBox[3]->setRect(Rect(x, 0, lw, getHeight()));
				x += lw;

				m_colorPicker->setRect(Rect(0, getHeight(), 250, 200));
			}

		};
	}
}