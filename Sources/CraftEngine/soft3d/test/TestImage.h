#pragma once
#include "../Context.h"



void testImage()
{
	using namespace CraftEngine;
	auto ctx = soft3d::createContext();
	auto img_file = core::Bitmap("img.png");
	auto img = soft3d::createImage(img_file.width(), img_file.height(), 1, 1, soft3d::ImageType::eImage2D, soft3d::ImageFormat::eR8G8B8A8_UNORM, 32, 1);
	auto mem = soft3d::createMemory(img.size());
	img.bind(mem, 0);
	memcpy(mem.data(), img_file.data(), img_file.size());

	for (int i = 0; i < 1; i++)
	{
		auto t1 = std::chrono::system_clock::now();
		soft3d::imgGenMipmap(img);
		auto t2 = std::chrono::system_clock::now();
		std::cout << "duration = " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / double(1000000) << "ms\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	for (int i = 1; i < img.mipLevels(); i++)
	{
		img.writeToFile((std::string("mipmap_level_") + std::to_string(i) + ".png").c_str(), i, 0);
	}
	soft3d::destroyImage(img);
	soft3d::destroyMemory(mem);
}