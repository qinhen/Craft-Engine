#pragma once

#include <array>
#include "../Common.h"
#include "./s3d_gui_canvas.h"


namespace CraftEngine
{
	namespace gui
	{

		namespace _Soft3D_Render_System_Detail
		{
			constexpr const uint32_t MAX_IMAGE_COUNT = 1024;

			class Soft3DGuiRenderSystem :public AbstractGuiRenderSystem
			{
			private:
				using Image = soft3d::Image;

				core::ObjectPool<Image, MAX_IMAGE_COUNT> m_imagePool;
			public:
				Soft3DGuiRenderSystem()
				{

					
				}

				virtual ~Soft3DGuiRenderSystem()
				{

				}

				virtual HandleImage createImage(const void* data, uint32_t size, uint32_t width, uint32_t height) override
				{
					Image image;
					if (size == width * height * 4)
						image = soft3d::createImage(width, height, 1, 1, soft3d::ImageType::eImage2D, soft3d::ImageFormat::eR8G8B8A8_UNORM, 1, 1);
					else if (size == width * height)
						image = soft3d::createImage(width, height, 1, 1, soft3d::ImageType::eImage2D, soft3d::ImageFormat::eR8_UNORM, 1, 1);
					else
						throw std::runtime_error("unrecognized format!");
					auto memory = soft3d::createMemory(image.size());
					image.bindMemory(memory, 0);
					memcpy(memory.data(), data, size);
					auto himage = m_imagePool.store(image);
					return HandleImage(himage);
				}
				virtual void deleteImage(HandleImage himage) override
				{
					if (himage != nullptr)
					{
						Image* pimage = (Image*)himage.operator void* ();
						soft3d::destroyMemory(pimage->memory());
						soft3d::destroyImage(*pimage);
						this->m_imagePool.free(pimage);
					}
					else
						throw std::runtime_error("himage == nullptr!");
				}
				virtual void updateImage(HandleImage image, const void* data, uint32_t size, uint32_t width, uint32_t height) override
				{
					if (image != nullptr)
					{
						Image* pimage = (Image*)image.operator void* ();
						if (pimage->width() == width && pimage->height() == height)
						{
							auto memory = pimage->memory();
							memcpy(memory.data(), data, size);
						}
						else
						{
							soft3d::destroyMemory(pimage->memory());
							soft3d::destroyImage(*pimage);
							*pimage = soft3d::createImage(width, height, 1, 1, soft3d::ImageType::eImage2D, soft3d::ImageFormat::eR8G8B8A8_UNORM, 1, 1);
							auto memory = soft3d::createMemory(pimage->size());
							pimage->bindMemory(memory, 0);
							memcpy(memory.data(), data, size);
						}
					}
					else
						throw std::runtime_error("himage == nullptr!");
				}

				virtual ImageInfo getImageInfo(HandleImage image) override
				{
					ImageInfo info{};
					Image* pimage = (Image*)image.operator void* ();
					if (image == nullptr)
						return info;
					info.mWidth = pimage->width();
					info.mHeight = pimage->height();
					info.mChannels = pimage->format() == soft3d::ImageFormat::eR8G8B8A8_UNORM ? 4 : 1;
					info.mMipLevels = pimage->mipLevels();
					return info;
				}

				virtual AbstractGuiCanvas* createGuiCanvas(const Size& size) override
				{
					Soft3DGuiCanvas* canvas = new Soft3DGuiCanvas;
					canvas->init();
					return canvas;
				}
				virtual void deleteGuiCanvas(AbstractGuiCanvas* canvas) override
				{
					((Soft3DGuiCanvas*)canvas)->clear();
					delete canvas;
				}

			};


		}


	};



}
