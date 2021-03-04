#pragma once

#include <Windows.h>


#include "./Image.h"

namespace CraftEngine
{
	namespace soft3d
	{

		namespace detail
		{
			constexpr int MaxSwapChainImageBufferCount = 16;

			struct SwapChainData
			{
				Image mImageBuffers[MaxSwapChainImageBufferCount];
				Image mDepthStencilBuffer;
				Memory mImageMemorys[MaxSwapChainImageBufferCount];
				Memory mDepthStencilMemory;
				int mImageCount;
				int mCurrentIndex;

				int mWidth;
				int mHeight;

				//主窗口的HDC和handle
				HWND mScreenHwnd;
				HDC mScreenHdc;
				HDC mHCompatibleDC; //兼容HDC
				HBITMAP mHCompatibleBitmap; //兼容BITMAP
				HBITMAP mHOldBitmap; //旧的BITMAP				  
				BITMAPINFO mBitmapInfo; //BITMAPINFO结构体
			};

		}


		class SwapChain
		{
		private:
			detail::SwapChainData* m_swapChainData;

		public:
			SwapChain(void* handle) : m_swapChainData((detail::SwapChainData*)handle) {}
			SwapChain() : m_swapChainData(nullptr) {}

			int imageCount() const { return m_swapChainData->mImageCount; }
			int currentImageIndex() const { return m_swapChainData->mCurrentIndex; }
			int acquireNextImage()
			{
				if (m_swapChainData->mImageCount <= 0)
				{
					m_swapChainData->mCurrentIndex = -1;
				}
				else if (m_swapChainData->mCurrentIndex < 0)
				{
					m_swapChainData->mCurrentIndex = 0;
				}
				else
				{
					m_swapChainData->mCurrentIndex++;
					if (m_swapChainData->mCurrentIndex >= m_swapChainData->mImageCount)
						m_swapChainData->mCurrentIndex = 0;
				}
				return m_swapChainData->mCurrentIndex;
			}

			Image getImage(int index) const
			{ 
				return m_swapChainData->mImageBuffers[index];
			}
			core::ArrayList<Image> getImages() const
			{
				core::ArrayList<Image> imgs(imageCount());
				for (int i = 0; i < imageCount(); i++)
					imgs[i] = getImage(i);
				return imgs;
			}
			Image getDepthStencilImage() const
			{
				return m_swapChainData->mDepthStencilBuffer;
			}

			int width() const { return m_swapChainData->mWidth; }
			int height() const { return m_swapChainData->mHeight; }

			void create(HWND hwnd, int width, int height, int imageCount, bool depth = false)
			{	
				clear();
				m_swapChainData->mWidth = width;
				m_swapChainData->mHeight = height;

				m_swapChainData->mScreenHwnd = hwnd;
				m_swapChainData->mScreenHdc = GetDC(m_swapChainData->mScreenHwnd);

				//获取兼容HDC和兼容Bitmap,兼容Bitmap选入兼容HDC(每个HDC内存每时刻仅能选入一个GDI资源,GDI资源要选入HDC才能进行绘制)
				m_swapChainData->mHCompatibleDC = CreateCompatibleDC(m_swapChainData->mScreenHdc);
				m_swapChainData->mHCompatibleBitmap = CreateCompatibleBitmap(m_swapChainData->mScreenHdc, m_swapChainData->mWidth, m_swapChainData->mHeight);
				m_swapChainData->mHOldBitmap = (HBITMAP)SelectObject(m_swapChainData->mHCompatibleDC, m_swapChainData->mHCompatibleBitmap);

				//填充结构体
				m_swapChainData->mBitmapInfo = {};
				m_swapChainData->mBitmapInfo.bmiHeader.biBitCount = 32;      //每个像素多少位，也可直接写24(RGB)或者32(RGBA)
				m_swapChainData->mBitmapInfo.bmiHeader.biCompression = BI_RGB;
				m_swapChainData->mBitmapInfo.bmiHeader.biHeight = -m_swapChainData->mHeight; // ?
				m_swapChainData->mBitmapInfo.bmiHeader.biPlanes = 1;
				m_swapChainData->mBitmapInfo.bmiHeader.biSizeImage = 0;
				m_swapChainData->mBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				m_swapChainData->mBitmapInfo.bmiHeader.biWidth = m_swapChainData->mWidth;


				m_swapChainData->mImageCount = imageCount;
				for (int i = 0; i < m_swapChainData->mImageCount; i++)
				{
					auto img = soft3d::createImage(this->width(), this->height(), 1, 1, soft3d::ImageType::eImage2D, soft3d::ImageFormat::eR8G8B8A8_UNORM, 1, 1);
					m_swapChainData->mImageBuffers[i] = img;
					auto mem = soft3d::createMemory(img.size());
					m_swapChainData->mImageMemorys[i] = mem;
					img.bindMemory(mem, 0);
				}

				if (depth)
				{
					auto img = soft3d::createImage(this->width(), this->height(), 1, 1, soft3d::ImageType::eImage2D, soft3d::ImageFormat::eD32_SFLOAT, 1, 1);
					m_swapChainData->mDepthStencilBuffer = img;
					auto mem = soft3d::createMemory(img.size());
					m_swapChainData->mDepthStencilMemory = mem;
					img.bindMemory(mem, 0);
				}
			}

			void clear()
			{
				for (int i = 0; i < imageCount(); i++)
				{
					Image img = Image(m_swapChainData->mImageBuffers[i]);
					soft3d::destroyImage(img);
					Memory mem = Memory(m_swapChainData->mImageMemorys[i]);
					soft3d::destroyMemory(mem);
				}
				m_swapChainData->mImageCount = 0;
				m_swapChainData->mCurrentIndex = -1;
				if (m_swapChainData->mDepthStencilBuffer.valid())
				{
					Image img = Image(m_swapChainData->mDepthStencilBuffer);
					soft3d::destroyImage(img);
					Memory mem = Memory(m_swapChainData->mDepthStencilMemory);
					soft3d::destroyMemory(mem);
				}
			}

			bool presentImage(int index)
			{
				if (imageCount() < 0)
					return false;
				auto clamp_index = index % imageCount();
				clamp_index += imageCount();
				clamp_index %= imageCount();
				SetDIBits(m_swapChainData->mScreenHdc, m_swapChainData->mHCompatibleBitmap, 0, this->height(), 
					m_swapChainData->mImageMemorys[clamp_index].data(), &m_swapChainData->mBitmapInfo, DIB_RGB_COLORS);
				BitBlt(m_swapChainData->mScreenHdc, 0, 0, this->width(), this->height(), m_swapChainData->mHCompatibleDC, 0, 0, SRCCOPY);
				return true;
			}

			void* handle() const { return m_swapChainData; }
			bool  valid() const { return handle() != nullptr; }
		};



		SwapChain createSwapChain(const MemAllocator& allocator = MemAllocator())
		{
			auto swap_chain = (detail::SwapChainData*)allocator.alloc(sizeof(detail::SwapChainData));
			swap_chain->mImageCount = 0;
			swap_chain->mCurrentIndex = -1;
			swap_chain->mDepthStencilBuffer = {};
			swap_chain->mDepthStencilMemory = {};
			return SwapChain(swap_chain);
		}

		void destroySwapChain(SwapChain& swapChain, const MemAllocator& allocator = MemAllocator())
		{
			swapChain.clear();
			allocator.free(swapChain.handle());
		}




	}
}