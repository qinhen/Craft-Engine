#pragma once
#ifndef CRAFT_ENGINE_CORE_BITMAP_H_
#define CRAFT_ENGINE_CORE_BITMAP_H_

#include "../3rdparty/ThirdPartyImportStbimg.h"
#include "./core/String.h"
#include <iostream>

namespace CraftEngine
{

	namespace core
	{

		class Bitmap
		{
		public:
			enum Format
			{
				Format_Null,
				Format_R8,
				Format_RGBA8,
			};

			struct PixelRGBA8
			{
				uint8_t rgba[4];
			};
			struct PixelR8
			{
				uint8_t r[1];
			};

			static uint32_t CalculateSize(uint32_t width, uint32_t height, Format format)
			{
				uint32_t byte_per_pixel = 0;
				switch (format)
				{
				case Format::Format_R8:byte_per_pixel = 1; break;
				case Format::Format_RGBA8:byte_per_pixel = 4; break;
				default:break;
				}
				return byte_per_pixel * width * height;
			}
			//static uint32_t CalculatePixel(uint32_t width, uint32_t height, Format format)
			//{
			//	uint32_t byte_per_pixel = 0;
			//	switch (format)
			//	{
			//	case Format::Format_R8:byte_per_pixel = 1; break;
			//	case Format::Format_RGBA8:byte_per_pixel = 4; break;
			//	default:break;
			//	}
			//	return byte_per_pixel * width * height;
			//}
		private:
			void* m_data;
			uint32_t m_size;
			uint32_t m_width;
			uint32_t m_height;
			Format m_format;
		public:

			//Bitmap(Bitmap&& bitmap)
			//{
			//	swap(bitmap);
			//	bitmap.clear();
			//}


			Bitmap(const Bitmap& bitmap) :m_data(nullptr), m_size(bitmap.size()), m_width(bitmap.width()), m_height(bitmap.height()), m_format(bitmap.m_format)
			{
				m_data = new uint8_t[bitmap.m_size];
				memcpy(m_data, bitmap.data(), bitmap.m_size);
			}
			Bitmap() :m_data(nullptr), m_size(0), m_width(0), m_height(0), m_format(Format_Null)
			{

			}
			CRAFT_ENGINE_EXPLICIT Bitmap(const char* path) :Bitmap()
			{
				loadFromFile(path);
			}
			CRAFT_ENGINE_EXPLICIT Bitmap(const wchar_t* path) :Bitmap()
			{
				loadFromFile(path);
			}
			~Bitmap()
			{
				clear();
			}
			Bitmap& operator=(const Bitmap& bitmap)
			{
				clear();
				m_size = bitmap.m_size;
				m_width = bitmap.m_width;
				m_height = bitmap.m_height;
				m_format = bitmap.m_format;
				m_data = new uint8_t[bitmap.m_size];
				memcpy(m_data, bitmap.data(), bitmap.m_size);
				return *this;
			}
			//Bitmap& operator=(Bitmap&& bitmap)
			//{
			//	swap(bitmap);
			//	bitmap.clear();
			//	return *this;
			//}

			bool valid()const { return data() != nullptr; }
			uint32_t width()const { return m_width; }
			uint32_t height()const { return m_height; }
			const void* data()const { return m_data; }
			void* data() { return m_data; }

			uint32_t size()const { return m_size; }
			Format format()const { return m_format; }

			void clear()
			{
				if (m_data != nullptr)
					delete[] m_data;
				m_data = nullptr;
				m_size = 0;
				m_width = 0;
				m_height = 0;
				m_format = Format_Null;
			}

			void swap(Bitmap& bitmap)
			{
				auto s = m_size;
				auto w = m_width;
				auto h = m_height;
				auto f = m_format;
				auto d = m_data;
				m_size = bitmap.m_size;
				m_width = bitmap.m_width;
				m_height = bitmap.m_height;
				m_format = bitmap.m_format;
				m_data = bitmap.m_data;
				bitmap.m_data = d;
				bitmap.m_size = s;
				bitmap.m_width = w;
				bitmap.m_height = h;
				bitmap.m_format = f;
			}

			bool loadFromFile(const wchar_t* path, bool red_channal_only = false)
			{
				return loadFromFile(codecvt::utf16le_to_utf8(path).c_str());
			}

			bool loadFromFile(const char* path, bool red_channal_only = false)
			{

				clear();
				int w = 0, h = 0, c = 0;
				m_data = stbi_load(path, &w, &h, &c, red_channal_only ? 1 : 4);
				if (m_data != nullptr)
				{
					m_width = w;
					m_height = h;
					if (red_channal_only) m_format = Format_R8;
					else m_format = Format_RGBA8;
					m_size = CalculateSize(w, h, m_format);
					return true;
				}
				else
				{
					return false;
				}
			}

			bool loadFromMemory(const void* buffer, uint32_t size, bool r8 = false)
			{
				clear();
				int w = 0, h = 0, c = 0;
				m_data = stbi_load_from_memory((unsigned char*)buffer, size, &w, &h, &c, r8 ? 1 : 4);
				if (m_data != nullptr)
				{
					m_width = w;
					m_height = h;
					if (r8) m_format = Format_R8;
					else m_format = Format_RGBA8;
					m_size = CalculateSize(w, h, m_format);
					return true;
				}
				else
				{
					return false;
				}
			}

			Bitmap flipY() const
			{		
				Bitmap bitmap;
				bitmap.m_size = size();
				bitmap.m_data = new uint8_t[bitmap.m_size];
				bitmap.m_width = width();
				bitmap.m_height = height();
				bitmap.m_format = format();
				switch (format())
				{
				case Format_RGBA8:
				{
					auto pSrcData = (const PixelRGBA8*)data();
					auto pDstData = (PixelRGBA8*)bitmap.m_data;
					for (int j = 0, k = height() - 1; j < height(); j++, k--)
						for (int i = 0; i < width(); i++)
							pDstData[i + j * width()] = pSrcData[i + k * width()];
					break;
				}
				case Format_R8:
				{
					auto pSrcData = (const PixelR8*)data();
					auto pDstData = (PixelR8*)bitmap.m_data;
					for (int j = 0, k = height() - 1; j < height(); j++, k--)
						for (int i = 0; i < width(); i++)
							pDstData[i + j * width()] = pSrcData[i + k * width()];
					break;
				}
				default:
					return Bitmap();
				}
				return bitmap;
			}

			Bitmap flipX() const
			{
				Bitmap bitmap;
				bitmap.m_size = size();
				bitmap.m_data = new uint8_t[bitmap.m_size];
				bitmap.m_width = width();
				bitmap.m_height = height();
				bitmap.m_format = format();
				switch (format())
				{
				case Format_RGBA8:
				{
					auto pSrcData = (const PixelRGBA8*)data();
					auto pDstData = (PixelRGBA8*)bitmap.m_data;
					for (int j = 0; j < height(); j++)
						for (int i = 0, k = width() - 1; i < width(); i++, k--)
							pDstData[i + j * width()] = pSrcData[k + j * width()];
					break;
				}
				case Format_R8:
				{
					auto pSrcData = (const PixelR8*)data();
					auto pDstData = (PixelR8*)bitmap.m_data;
					for (int j = 0; j < height(); j++)
						for (int i = 0, k = width() - 1; i < width(); i++, k--)
							pDstData[i + j * width()] = pSrcData[k + j * width()];
					break;
				}
				default:
					return Bitmap();
				}
				return bitmap;
			}




			Bitmap resize(int width, int height) const
			{
				Bitmap bitmap;
				bitmap.m_size = CalculateSize(width, height, format());
				bitmap.m_data = new uint8_t[bitmap.m_size];
				bitmap.m_width = width;
				bitmap.m_height = height;
				bitmap.m_format = format();

				switch (format())
				{
				case Format_RGBA8:
				{
					auto pSrcData = (const PixelRGBA8*)data();
					auto pDstData = (PixelRGBA8*)bitmap.m_data;		
					for (int j = 0; j < height; j++)
						for (int i = 0; i < width; i++)
						{
							int sample_x = i * m_width / width;
							int sample_y = j * m_height / height;
							memcpy(&pDstData[i + j * width], &pSrcData[sample_x + sample_y * m_width], sizeof(PixelRGBA8));
							//pDstData[i + j * width] = pSrcData[sample_x + sample_y * m_width];
						}
					break;
				}
				case Format_R8:
				{
					auto pSrcData = (const PixelR8*)data();
					auto pDstData = (PixelR8*)bitmap.m_data;
					for (int j = 0; j < height; j++)
						for (int i = 0; i < width; i++)
						{
							int sample_x = i * m_width / width;
							int sample_y = j * m_height / height;
							pDstData[i + j * width] = pSrcData[sample_x + sample_y * m_width];
						}
					break;
				}
				default:
				{
					return Bitmap();
				}
				}
				return bitmap;
			}
		};





	}

}


#endif // CRAFT_ENGINE_CORE_BITMAP_H_