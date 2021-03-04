#pragma once
#ifndef CRAFT_ENGINE_GUI_WIDGET_CREATOR_H_
#define CRAFT_ENGINE_GUI_WIDGET_CREATOR_H_

#include "./widgets/Widget.h"
#include "./widgets/CanvasWidget.h"
#include "./widgets/FrameWidget.h"

// present-widgets
#include "./widgets/TextView.h"
#include "./widgets/ImageView.h"
#include "./widgets/Label.h"
#include "./widgets/ProgressBar.h"
#include "./widgets/Slider.h"
#include "./widgets/SeekBar.h"
#include "./widgets/ScrollBar.h"
#include "./widgets/ScrollWidget.h"

// input-widgets
#include "./widgets/CheckButton.h"
#include "./widgets/PushButton.h"
#include "./widgets/TextLine.h"
#include "./widgets/RichTextLine.h"
#include "./widgets/TextBox.h"
#include "./widgets/PasswordBox.h"
#include "./widgets/SpinBox.h"
#include "./widgets/CheckBox.h"
#include "./widgets/ComboBox.h"
#include "./widgets/ColorPickerWidget.h"
#include "./widgets/ValueInput.h"
#include "./widgets/ColorInput.h"
#include "./widgets/FontSelectWidget.h"

// data-binding
#include "./widgets/DataBinderWidget.h"
#include "./widgets/InspectorWidget.h"

// tools
#include "./widgets/MenuWidget.h"
#include "./widgets/MenuBar.h"
#include "./widgets/TabWidget.h"
#include "./widgets/DockArea.h"

// model-view
#include "./modelview/AbstractModel.h"
#include "./modelview/AbstractView.h"
#include "./modelview/StandardItemModel.h"
#include "./modelview/StandardItemView.h"
#include "./modelview/StringsModel.h"
#include "./modelview/StringsView.h"
#include "./modelview/NodeModel.h"
#include "./modelview/NodeView.h"

// container-widgets
#include "./widgets/StringListWidget.h"
#include "./widgets/StringTableWidget.h"
#include "./widgets/StandardListWidget.h"
#include "./widgets/StandardTableWidget.h"

//#include "./widgetsext/ColorTextEdit.h"

// OffScreen Framebuffer
// #include "./widgets/ChartWidget.h"

//#include "./advance/ValueEdit.h"


#include "./MainWindow.h"

namespace CraftEngine
{
	namespace gui
	{

		class WidgetCreator :private core::IocContainer
		{
		public:

			WidgetCreator()
			{
				this->regtype_s<Widget, Widget*>("Widget");
				this->regtype<Widget, FrameWidget, Widget*>("FrameWidget");
				this->regtype<Widget, Label, Widget*>("Label");
				this->regtype<Widget, PushButton, Widget*>("PushButton");
				this->regtype<Widget, ProgressBar, Widget*>("ProgressBar");

				this->regtype<Widget, HSliderInt, Widget*>("HSliderInt");
				this->regtype<Widget, VSliderInt, Widget*>("VSliderInt");
				this->regtype<Widget, HSliderFloat, Widget*>("HSliderFloat");
				this->regtype<Widget, VSliderFloat, Widget*>("VSliderFloat");
				this->regtype<Widget, HSeekBar, Widget*>("HSeekBar");
				this->regtype<Widget, VSeekBar, Widget*>("VSeekBar");
				this->regtype<Widget, HScrollBar, Widget*>("HScrollBar");
				this->regtype<Widget, VScrollBar, Widget*>("VScrollBar");
				this->regtype<Widget, ScrollWidget, Widget*>("ScrollWidget");

				this->regtype<Widget, TextLine, Widget*>("TextLine");
				this->regtype<Widget, PasswordBox, Widget*>("PasswordBox");
				this->regtype<Widget, TextBox, Widget*>("TextBox");

				this->regtype<Widget, ComboBox, Widget*>("ComboBox");
				this->regtype<Widget, SpinBoxInt, Widget*>("SpinBoxInt");
				this->regtype<Widget, SpinBoxFloat, Widget*>("SpinBoxFloat");
				this->regtype<Widget, CheckBox, Widget*>("CheckBox");

			}


			Widget* createWidget(const std::string& type, Widget* parent, std::type_index* idx = nullptr)
			{
				auto widget = create<CraftEngine::gui::Widget, CraftEngine::gui::Widget*>(type, parent, idx);
				if (widget == nullptr)
				{
					throw std::runtime_error("unable to create widget");
				}
				return widget;
			}

			MainWindow* createWindow(
				const std::wstring& title = L"CraftEngine Window",
				uint32_t    width = 1280,
				uint32_t    height = 720,
				const MainWindow* parent = nullptr,
				std::type_index* idx = nullptr
			)
			{
				MainWindow* mainwindow = new MainWindow(title, width, height, parent);
				if (mainwindow == nullptr)
				{
					throw std::runtime_error("unable to create mainwindow");
				}
				return mainwindow;
			}
	
		};


	}
}

#endif // !CRAFT_ENGINE_GUI_WIDGET_CREATOR_H_
