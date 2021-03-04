#pragma once
#include "./Image.h"

namespace CraftEngine
{
	namespace soft3d
	{

		namespace detail
		{
			struct ImageViewData
			{
				Image       mImage;
				ImageFormat mFormat;
				ImageType   mType;
				uint32_t    mBaseMipLevel;
				uint32_t    mMipLevelCount;
				uint32_t    mBaseLayer;
				uint32_t    mLayerCount;
			};
		}

		class ImageView
		{
		private:
			detail::ImageViewData* m_imageViewData;
		public:
			explicit ImageView(void* handle) : m_imageViewData((detail::ImageViewData*)handle) {}
			ImageView() : m_imageViewData(nullptr) {}

			ImageFormat format() const { return m_imageViewData->mFormat; }
			ImageType   type() const { return m_imageViewData->mType; }

			uint32_t baseMipLevel() const { return m_imageViewData->mBaseMipLevel; }
			uint32_t mipLevelCount() const { return m_imageViewData->mMipLevelCount; }

			uint32_t baseLayer() const { return m_imageViewData->mBaseLayer; }
			uint32_t layerCount() const { return m_imageViewData->mLayerCount; }

			void* handle() const { return m_imageViewData; }
			bool  valid() const { return handle() != nullptr; }
		};



		ImageView createImageView(
			const Image& image, 
			ImageType type, 
			ImageFormat format, 
			uint32_t baseMipLevel, 
			uint32_t mipLevelCount, 
			uint32_t baseLayer,
			uint32_t layerCount,
			const MemAllocator& allocator = MemAllocator()
		)
		{
			auto view_data = (detail::ImageViewData*)allocator.alloc(sizeof(detail::ImageViewData));
			view_data->mImage = image;
			view_data->mType = type;
			view_data->mFormat = format;
			view_data->mBaseMipLevel = baseMipLevel;
			view_data->mMipLevelCount = mipLevelCount;
			view_data->mBaseLayer = baseLayer;
			view_data->mLayerCount = layerCount;
			return ImageView(view_data);
		}
		void destroyImageView(const ImageView& imageView, const MemAllocator& allocator = MemAllocator())
		{
			allocator.free(imageView.handle());
		}


	}
}