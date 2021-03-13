#pragma once
#include "./Label.h"
#include "./TextLine.h"
#include "./ComboBox.h"
#include "./RichTextLine.h"
#include "./CheckBox.h"
#include "./PushButton.h"
#include "./SpinBox.h"

#include "./ColorInput.h"

namespace CraftEngine
{
	namespace gui
	{
		class FontSelectWidget : public CraftEngine::gui::Widget
		{
		private:


			Font           m_currentFont;
			RichTextFormat m_currentFormat;

			Label* m_labels[16];

			// line 0
			RichTextLine* m_exampleText;
			// line 1
			ComboBox* m_fontSelectBox;
			// line 2
			TextLine* m_fontSizeInput;
			HSliderInt* m_fontSizeSlider;
			// line 3
			ColorInput* m_fontColorSelector;

			// line 3
			CheckButtonGroup m_fontHAlignmentGroup;
			CheckBox* m_fontHAlignmentCheckBox[3];
			// line 4
			CheckButtonGroup m_fontVAlignmentGroup;
			CheckBox* m_fontVAlignmentCheckBox[3];

			// line 3
			ComboBox* m_fontModeSelectBox;
			// line 4
			ComboBox* m_fontEdgeModeSelectBox;
			// line 5
			TextLine* m_fontEdgeSizeInput;
			HSliderFloat* m_fontEdgeSizeSlider;
			// line 6
			TextLine* m_fontSlopeInput;
			HSliderFloat* m_fontSlopeSlider;
			// 
			ColorInput* m_fontEdgeColorSelector;

		public:

			craft_engine_gui_signal_export(m_exampleText, textChanged, exampleTextChanged, void(const String&));
			craft_engine_gui_signal(fontChanged, void(const Font&));
			craft_engine_gui_signal(formatChanged, void(const RichTextFormat&));
			FontSelectWidget(Widget* parent) : Widget(Rect(0, 0, 300, 110 + 22 * 10), parent)
			{
				this->getPalette().mFrameMode = CraftEngine::gui::Palette::eFrameMode_Surround;

				// line 0
				m_exampleText = new RichTextLine(this);
				m_exampleText->setText(L"This is an example.");
				// line 1
				m_labels[1] = new Label(L"Font Type", this);
				m_fontSelectBox = new ComboBox(this);
				CraftEngine::core::ArrayList<int> fontList = CraftEngine::gui::GuiFontSystem::getGlobalFontIDList();
				for (auto it : fontList)
					m_fontSelectBox->addItem(L"Default Font");
				m_fontSelectBox->setCurrentIndex(0);

				// line 2
				m_labels[2] = new Label(L"Font Size", this);
				m_fontSizeInput = new TextLine(this);
				m_fontSizeSlider = new HSliderInt(this);
				m_fontSizeSlider->setValue(m_currentFont.getSize(), 10, 100);

				wchar_t val_str[256];
				if (swprintf_s(val_str, 256, L"%d", m_fontSizeSlider->getValue()))
					m_fontSizeInput->setText(val_str);
				m_fontSizeInput->setText(val_str);

				// line 3
				m_labels[3] = new Label(L"Font Color", this);
				m_fontColorSelector = new ColorInput(this);
				m_fontColorSelector->setColor(m_currentFormat.getColor());

				// line 4
				m_labels[4] = new Label(L"HAlignment", this);
				m_fontHAlignmentCheckBox[0] = new CheckBox(L"Left", this);
				m_fontHAlignmentCheckBox[1] = new CheckBox(L"Center", this);
				m_fontHAlignmentCheckBox[2] = new CheckBox(L"Right", this);
				m_fontHAlignmentGroup.addButton(m_fontHAlignmentCheckBox[0]->getCheckButton());
				m_fontHAlignmentGroup.addButton(m_fontHAlignmentCheckBox[1]->getCheckButton());
				m_fontHAlignmentGroup.addButton(m_fontHAlignmentCheckBox[2]->getCheckButton());
				m_fontHAlignmentCheckBox[0]->getCheckButton()->setCheckState(CheckButton::eCheckState_Selected);

				// line 5
				m_labels[5] = new Label(L"VAlignment", this);
				m_fontVAlignmentCheckBox[0] = new CheckBox(L"Top", this);
				m_fontVAlignmentCheckBox[1] = new CheckBox(L"Center", this);
				m_fontVAlignmentCheckBox[2] = new CheckBox(L"Bottom", this);
				m_fontVAlignmentGroup.addButton(m_fontVAlignmentCheckBox[0]->getCheckButton());
				m_fontVAlignmentGroup.addButton(m_fontVAlignmentCheckBox[1]->getCheckButton());
				m_fontVAlignmentGroup.addButton(m_fontVAlignmentCheckBox[2]->getCheckButton());
				m_fontVAlignmentCheckBox[1]->getCheckButton()->setCheckState(CheckButton::eCheckState_Selected);

				// line 6
				m_labels[6] = new Label(L"Font Mode", this);
				m_fontModeSelectBox = new ComboBox(this);
				m_fontModeSelectBox->addItem(L"Standard");
				m_fontModeSelectBox->addItem(L"Bold");
				m_fontModeSelectBox->addItem(L"Light");
				m_fontModeSelectBox->setCurrentIndex(0);

				// line 7
				m_labels[7] = new Label(L"Edge Mode", this);
				m_fontEdgeModeSelectBox = new ComboBox(this);
				m_fontEdgeModeSelectBox->addItem(L"Fill");
				m_fontEdgeModeSelectBox->addItem(L"Fade Out");
				m_fontEdgeModeSelectBox->addItem(L"Fade In");
				m_fontEdgeModeSelectBox->setCurrentIndex(0);

				// line 8
				m_labels[8] = new Label(L"Edge Size", this);
				m_fontEdgeSizeInput = new TextLine(this);
				m_fontEdgeSizeSlider = new HSliderFloat(this);
				m_fontEdgeSizeSlider->setValue(m_currentFormat.getEdgeSize(), 0.0f, 1.0f);

				if (swprintf_s(val_str, 256, L"%.2f", m_fontEdgeSizeSlider->getValue()))
					m_fontEdgeSizeInput->setText(val_str);
				m_fontEdgeSizeInput->setText(val_str);

				// line 9
				m_labels[9] = new Label(L"Font Slope", this);
				m_fontSlopeInput = new TextLine(this);
				m_fontSlopeSlider = new HSliderFloat(this);
				m_fontSlopeSlider->setValue(m_currentFormat.getSlope(), -45.0f, 45.0f);

				if (swprintf_s(val_str, 256, L"%.2f", m_fontSlopeSlider->getValue()))
					m_fontSlopeInput->setText(val_str);
				m_fontSlopeInput->setText(val_str);

				// line 10
				m_labels[10] = new Label(L"Edge Color", this);
				m_fontEdgeColorSelector = new ColorInput(this);
				m_fontEdgeColorSelector->setColor(m_currentFormat.getEdgeColor());

				_Adjust_Layout();

				craft_engine_gui_connect(m_fontSizeInput, textChanged, this, _On_Font_Size_Edit_Finished);
				craft_engine_gui_connect(m_fontSizeSlider, slid, this, _On_Font_Size_Sild);
				craft_engine_gui_connect(m_fontSelectBox, selectionChanged, this, _On_Font_Select_Box_Idx_Changed);
				craft_engine_gui_connect(m_fontModeSelectBox, selectionChanged, this, _On_Font_Mode_Select_Box_Idx_Changed);
				craft_engine_gui_connect(m_fontEdgeModeSelectBox, selectionChanged, this, _On_Font_Edge_Mode_Select_Box_Idx_Changed);
				craft_engine_gui_connect(m_fontEdgeSizeInput, textChanged, this, _On_Font_Edge_Size_Edit_Finished);
				craft_engine_gui_connect(m_fontEdgeSizeSlider, slid, this, _On_Font_Edge_Size_Sild);
				craft_engine_gui_connect(m_fontSlopeInput, textChanged, this, _On_Font_Slope_Edit_Finished);
				craft_engine_gui_connect(m_fontSlopeSlider, slid, this, _On_Font_Slope_Sild);
				craft_engine_gui_connect(&m_fontHAlignmentGroup, stateChanged, this, _On_Font_HAlignment_Changed);
				craft_engine_gui_connect(&m_fontVAlignmentGroup, stateChanged, this, _On_Font_VAlignment_Changed);


				craft_engine_gui_connect(m_fontColorSelector, colorChanged, this, _On_Font_Color_Changed);
				craft_engine_gui_connect(m_fontEdgeColorSelector, colorChanged, this, _On_Font_Edge_Color_Changed);
			}
		protected:
			virtual Size getMinSizeLimit()const
			{
				return Size(300, 110 + 22 * 10);
			}

		private:

			void updateFont()
			{
				m_exampleText->setFont(m_currentFont);
				m_exampleText->setText(m_exampleText->getText());
			}
			void updateFormat()
			{
				m_exampleText->setTextFormat(m_currentFormat);
				m_exampleText->setText(m_exampleText->getText());
			}

			void _On_Font_Edge_Color_Changed(const Color& color)
			{
				m_currentFormat.setEdgeColor(color);
				updateFormat();
			}
			void _On_Font_Color_Changed(const Color& color)
			{
				m_currentFormat.setColor(color);
				updateFormat();
			}

			void _On_Font_HAlignment_Changed(const CraftEngine::core::LinkedList<CheckButton*>& btnList)
			{
				auto it = btnList.begin();
				auto i = 0;
				for (; it != btnList.end(); it++, i++)
					if ((*it)->getCheckState() == CheckButton::eCheckState_Selected)
						break;
				switch (i)
				{
				case 0:m_currentFont.getAlignment().setHorizonAlignment(CraftEngine::gui::Alignment::eAlignmentMode_Left); break;
				case 1:m_currentFont.getAlignment().setHorizonAlignment(CraftEngine::gui::Alignment::eAlignmentMode_Center); break;
				case 2:m_currentFont.getAlignment().setHorizonAlignment(CraftEngine::gui::Alignment::eAlignmentMode_Right); break;
				default:
					break;
				}
				updateFont();
			}
			void _On_Font_VAlignment_Changed(const CraftEngine::core::LinkedList<CheckButton*>& btnList)
			{
				auto it = btnList.begin();
				auto i = 0;
				for (; it != btnList.end(); it++, i++)
					if ((*it)->getCheckState() == CheckButton::eCheckState_Selected)
						break;
				switch (i)
				{
				case 0:m_currentFont.getAlignment().setVerticalAlignment(CraftEngine::gui::Alignment::eAlignmentMode_Top); break;
				case 1:m_currentFont.getAlignment().setVerticalAlignment(CraftEngine::gui::Alignment::eAlignmentMode_Center); break;
				case 2:m_currentFont.getAlignment().setVerticalAlignment(CraftEngine::gui::Alignment::eAlignmentMode_Bottom); break;
				default:
					break;
				}
				updateFont();
			}

			virtual void _On_Font_Slope_Edit_Finished(const CraftEngine::gui::String& str)
			{
				float val;
				if (CraftEngine::gui::StringTool::toValue(m_fontSlopeInput->getText(), &val))
				{
					m_fontSlopeSlider->setValue(val, -45.0f, 45.0f);
					m_currentFormat.setSlope(val);
					updateFormat();
				}
			}

			virtual void _On_Font_Slope_Sild(float val)
			{
				wchar_t val_str[256];
				if (swprintf_s(val_str, 256, L"%.2f", m_fontSlopeSlider->getValue()))
					m_fontSlopeInput->setText(val_str);
				m_currentFormat.setSlope(val);
				updateFormat();
			}


			virtual void _On_Font_Edge_Size_Edit_Finished(const CraftEngine::gui::String& str)
			{
				float val;
				if (CraftEngine::gui::StringTool::toValue(m_fontEdgeSizeInput->getText(), &val))
				{
					m_fontEdgeSizeSlider->setValue(val, 0.0f, 1.0f);
					m_currentFormat.setEdgeSize(val);
					updateFormat();
				}
			}

			virtual void _On_Font_Edge_Size_Sild(float val)
			{
				wchar_t val_str[256];
				if (swprintf_s(val_str, 256, L"%.2f", m_fontEdgeSizeSlider->getValue()))
					m_fontEdgeSizeInput->setText(val_str);
				m_currentFormat.setEdgeSize(val);
				updateFormat();
			}


			virtual void _On_Font_Edge_Mode_Select_Box_Idx_Changed(int idx)
			{
				switch (idx)
				{
				case 1:m_currentFormat.setEdgeMode(RichTextFormat::eEdgeMode_FadeOut); break;
				case 2:m_currentFormat.setEdgeMode(RichTextFormat::eEdgeMode_FadeIn); break;
				case 0:default:m_currentFormat.setEdgeMode(RichTextFormat::eEdgeMode_Fill);
				}
				updateFormat();
			}

			virtual void _On_Font_Mode_Select_Box_Idx_Changed(int idx)
			{
				switch (idx)
				{
				case 1:m_currentFormat.setFontMode(RichTextFormat::eFontMode_Bold); break;
				case 2:m_currentFormat.setFontMode(RichTextFormat::eFontMode_Light); break;
				case 0:default:m_currentFormat.setFontMode(RichTextFormat::eFontMode_Standard);
				}
				updateFormat();
			}

			virtual void _On_Font_Select_Box_Idx_Changed(int idx)
			{
				m_fontSelectBox->clearItems();
				CraftEngine::core::ArrayList<int> fontList = CraftEngine::gui::GuiFontSystem::getGlobalFontIDList();
				for (auto& it : fontList)
					m_fontSelectBox->addItem(L"Default Font");
				if (idx < fontList.size())
				{
					m_currentFont.setFontID(fontList[idx]);
					updateFont();
				}
			}

			virtual void _On_Font_Size_Edit_Finished(const CraftEngine::gui::String& str)
			{
				float val;
				if (CraftEngine::gui::StringTool::toValue(m_fontSizeInput->getText(), &val))
				{
					m_fontSizeSlider->setValue(val, 10, 100);
					m_currentFont.setSize(val);
					updateFont();
				}
			}

			virtual void _On_Font_Size_Sild(int val)
			{
				wchar_t val_str[256];
				if (swprintf_s(val_str, 256, L"%d", m_fontSizeSlider->getValue()))
					m_fontSizeInput->setText(val_str);
				m_currentFont.setSize(val);
				updateFont();
			}

			virtual void onResizeEvent(const ResizeEvent& resizeEvent)override
			{
				_Adjust_Layout();
			}

			void _Adjust_Layout()
			{
				int labelWidth = 80;
				if (getWidth() - 80 < 250)
					labelWidth = getWidth() - 250;
				if (labelWidth < 0)
					labelWidth = 0;

				int inputWidth = 60;
				int y = 0;
				int textLineHeight = math::max(110, getHeight() - 22 * 10);
				// line 0
				m_exampleText->setRect(Rect(0, 0, getWidth(), textLineHeight).padding(2, 2));
				y += textLineHeight;
				// line 1
				m_labels[1]->setRect(Rect(0, y, labelWidth, 22).padding(2, 2));
				m_fontSelectBox->setRect(Rect(labelWidth, y, getWidth() - labelWidth, 22).padding(2, 2));
				y += 22;
				// line 2
				m_labels[2]->setRect(Rect(0, y, labelWidth, 22).padding(2, 2));
				m_fontSizeInput->setRect(Rect(labelWidth, y, inputWidth, 22).padding(2, 2));
				m_fontSizeSlider->setRect(Rect(labelWidth + inputWidth, y, getWidth() - (labelWidth + inputWidth), 22).padding(2, 2));
				y += 22;

				// line 3
				m_labels[3]->setRect(Rect(0, y, labelWidth, 22).padding(2, 2));
				m_fontColorSelector->setRect(Rect(labelWidth, y, getWidth() - labelWidth, 22).padding(2, 2));
				y += 22;

				// line 4
				m_labels[4]->setRect(Rect(0, y, labelWidth, 22).padding(2, 2));
				m_fontHAlignmentCheckBox[0]->setRect(Rect(labelWidth, y, (getWidth() - labelWidth) / 3 * 1, 22).padding(2, 2));
				m_fontHAlignmentCheckBox[1]->setRect(Rect(labelWidth + (getWidth() - labelWidth) / 3 * 1, y, (getWidth() - labelWidth) / 3 * 2, 22).padding(2, 2));
				m_fontHAlignmentCheckBox[2]->setRect(Rect(labelWidth + (getWidth() - labelWidth) / 3 * 2, y, getWidth() - (getWidth() - labelWidth) / 3 * 2 - labelWidth, 22).padding(2, 2));
				y += 22;

				// line 5
				m_labels[5]->setRect(Rect(0, y, labelWidth, 22).padding(2, 2));
				m_fontVAlignmentCheckBox[0]->setRect(Rect(labelWidth, y, (getWidth() - labelWidth) / 3 * 1, 22).padding(2, 2));
				m_fontVAlignmentCheckBox[1]->setRect(Rect(labelWidth + (getWidth() - labelWidth) / 3 * 1, y, (getWidth() - labelWidth) / 3 * 2, 22).padding(2, 2));
				m_fontVAlignmentCheckBox[2]->setRect(Rect(labelWidth + (getWidth() - labelWidth) / 3 * 2, y, getWidth() - (getWidth() - labelWidth) / 3 * 2 - labelWidth, 22).padding(2, 2));
				y += 22;

				// line 6
				m_labels[6]->setRect(Rect(0, y, labelWidth, 22).padding(2, 2));
				m_fontModeSelectBox->setRect(Rect(labelWidth, y, getWidth() - labelWidth, 22).padding(2, 2));
				y += 22;
				// line 7
				m_labels[7]->setRect(Rect(0, y, labelWidth, 22).padding(2, 2));
				m_fontEdgeModeSelectBox->setRect(Rect(labelWidth, y, getWidth() - labelWidth, 22).padding(2, 2));
				y += 22;
				// line 8
				m_labels[8]->setRect(Rect(0, y, labelWidth, 22).padding(2, 2));
				m_fontEdgeSizeInput->setRect(Rect(labelWidth, y, inputWidth, 22).padding(2, 2));
				m_fontEdgeSizeSlider->setRect(Rect(labelWidth + inputWidth, y, getWidth() - (labelWidth + inputWidth), 22).padding(2, 2));
				y += 22;
				// line 9
				m_labels[9]->setRect(Rect(0, y, labelWidth, 22).padding(2, 2));
				m_fontSlopeInput->setRect(Rect(labelWidth, y, inputWidth, 22).padding(2, 2));
				m_fontSlopeSlider->setRect(Rect(labelWidth + inputWidth, y, getWidth() - (labelWidth + inputWidth), 22).padding(2, 2));
				y += 22;
				// line 10
				m_labels[10]->setRect(Rect(0, y, labelWidth, 22).padding(2, 2));
				m_fontEdgeColorSelector->setRect(Rect(labelWidth, y, getWidth() - labelWidth, 22).padding(2, 2));
				y += 22;
			}

		};


	}
}
