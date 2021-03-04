#include "CraftEngine/gui/Gui.h"
using namespace CraftEngine;

int main()
{
	gui::Application app;

	auto main_window = new gui::MainWindow(L"Craft-Engine Example-01: MainWindow");
	main_window->exec();
}
