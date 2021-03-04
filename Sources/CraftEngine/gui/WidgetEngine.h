#pragma once
#ifndef CRAFT_ENGINE_GUI_WIDGET_ENGINE_H_
#define CRAFT_ENGINE_GUI_WIDGET_ENGINE_H_

#include "WidgetCreator.h"


namespace CraftEngine
{
	namespace gui
	{



		class WidgetEngine :private util::IocContainer
		{
		private:

			struct WidgetData
			{
				Widget* ptr = nullptr;
				std::type_index idx = typeid(void);
			};

			std::unordered_map<std::string, WidgetData> m_widgetMap;
			Widget* m_curWidget;
			WidgetCreator m_widgetCreator;

			void _Add_Widget(const std::string name, Widget* widget, std::type_index idx)
			{
				if (m_widgetMap.find(name) == m_widgetMap.end())
				{
					m_widgetMap[name].ptr = widget;
					m_widgetMap[name].idx = idx;
				}
				else
				{
					throw std::exception("name conflict.");
				}
			}

		public:

			Widget* createWidget(const std::string& type, Widget* parent, const std::string& name)
			{
				std::type_index index(typeid(void));
				auto widget = m_widgetCreator.createWidget(type, parent, &index);
				if (widget != nullptr)
				{
					_Add_Widget(name, widget, index);
				}
				return widget;
			}

			MainWindow* createWindow(
				const std::wstring& title = L"CraftEngine Window",
				uint32_t    width = 1280,
				uint32_t    height = 720,
				const MainWindow* parent = nullptr
			)
			{
				std::type_index index(typeid(void));
				MainWindow* mainwindow = m_widgetCreator.createWindow(title, width, height, parent, &index);
				_Add_Widget("MainWindow", mainwindow, index);
				return mainwindow;
			}

			Widget* getByName(const std::string& name)
			{
				auto it = m_widgetMap.find(name);
				if (it != m_widgetMap.end())
					return it->second.ptr;
				else
					return nullptr;
			}

			void execute(const std::string& command)
			{
				std::stringstream strbuf(command);
				std::string cmd;
				strbuf >> cmd;
				for (auto& it : cmd)
					it = tolower(it);
				if (cmd == "select")
				{
					std::string opt;
					strbuf >> opt;
					auto it = m_widgetMap.find(opt);
					if (it != m_widgetMap.end())
						m_curWidget = it->second.ptr;
				}
				else if (cmd == "create")
				{
					std::string opt;
					strbuf >> opt;
					std::string name;
					strbuf >> name;
					createWidget(opt, m_curWidget, name);
				}
				else
				{
					m_curWidget->execute(command);
				}
			}

		};
		

	}
}

#endif // !CRAFT_ENGINE_GUI_WIDGET_ENGINE_H_
