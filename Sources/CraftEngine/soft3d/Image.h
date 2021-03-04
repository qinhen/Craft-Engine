#pragma once
#include "./Memory.h"

namespace CraftEngine
{
	namespace soft3d
	{


		struct ImageExtent
		{
			uint32_t mWidth;
			uint32_t mHeight;
			uint32_t mDepth;
		};


		enum class ImageType
		{
			eImage1D,
			eImage2D,
			eImage3D,
			eImageCube,
			eImage1DArray,
			eImage2DArray,
			eImage3DArray,
		};

		enum class ImageFormat
		{
			// Unsigned-Normalized-Float
			eImageFormatMin,
			eR8G8B8A8_UNORM = eImageFormatMin,
			eR8_UNORM,
			eR16G16B16A16_UNORM,
			eR16_UNORM,
			// Signed-Normalized-Float
			eR8G8B8A8_SNORM,
			eR8_SNORM,
			eR16G16B16A16_SNORM,
			eR16_SNORM,
			// Float
			eR32G32B32A32_SFLOAT,
			eR32_SFLOAT,
			// Mixed
			// Compressed
			eR5G5B5A1_UNORM,
			// Depth-stencil
			eD24_UNORM_S8_UINT,
			eD32_SFLOAT,
			eImageFormatMax = eD32_SFLOAT,
			eImageFormatCount = eImageFormatMax - eImageFormatMin + 1,
		};




		namespace detail
		{

			const int PixelByteSize[(size_t)ImageFormat::eImageFormatCount] =
			{
				// Unsigned-Normalized-Float
				4,
				1,
				8,
				2,
				// Signed-Normalized-Float
				4,
				1,
				8,
				2,
				// Float
				16,
				4,
				// Mixed
				// Compressed
				4,
				// Depth-stencil
				4,
				4,
			};

			constexpr int getPixelByteSize(const ImageFormat& format)
			{
				if (format >= ImageFormat::eImageFormatMin && format <= ImageFormat::eImageFormatMax)
					return PixelByteSize[(size_t)format];
				soft3d_throw_error(ErrorType::eInvalidEnum);
			}

			uint32_t calcMaxMipLevel(const ImageExtent& extent)
			{
				auto min_extent = math::max(extent.mWidth, extent.mHeight, extent.mDepth);
				uint32_t mipLevels = floorf(log2f(min_extent)) + 1;
				return mipLevels;
			}
			ImageExtent calcMipLevelExtent(const ImageExtent& extent, uint32_t level)
			{
				ImageExtent levelExtent;
				levelExtent.mWidth = extent.mWidth >> level;
				levelExtent.mHeight = extent.mHeight >> level;
				levelExtent.mDepth = extent.mDepth >> level;
				levelExtent.mWidth = levelExtent.mWidth > 0 ? levelExtent.mWidth : 1;
				levelExtent.mHeight = levelExtent.mHeight > 0 ? levelExtent.mHeight : 1;
				levelExtent.mDepth = levelExtent.mDepth > 0 ? levelExtent.mDepth : 1;
				return levelExtent;
			}


			struct ImagePixelData
			{
				using uvec4 = math::uvec4;
				using i8vec4 = math::Vector<4, int8_t>;
				using u16vec4 = math::Vector<4, uint16_t>;
				using i16vec4 = math::Vector<4, int16_t>;

				union {
					byte mData[16];
					// Unsigned-Normalized-Float
					u8vec4 mR8G8B8A8_UNORM;
					uint8_t mR8_UNORM;
					u16vec4 mR16G16B16A16_UNORM;
					uint16_t mR16_UNORM;
					// Signed-Normalized-Float
					i8vec4 mR8G8B8A8_SNORM;
					int8_t mR8_SNORM;
					i16vec4 mR16G16B16A16_SNORM;
					int16_t mR16_SNORM;
					// Float
					vec4 mR32G32B32A32_SFLOAT;
					float mR32_SFLOAT;
					// Mixed
					// Compressed
					union R5G5B5A1_UNORM {
						uint8_t R5 : 5;
						uint8_t G5 : 5;
						uint8_t B5 : 5;
						uint8_t A1 : 1;
					}mR5G5B5A1_UNORM;
					// Depth-stencil
					union D24_UNORM_S8_UINT {
						uint32_t D24 : 24;
						uint8_t  S8 : 8;
					}mD24_UNORM_S8_UINT;
					float mD32_SFLOAT;
				};

				ImagePixelData()
				{
					// Do Nothing.
				}
				ImagePixelData(const ImagePixelData& data)
				{
					memcpy(mData, data.mData, sizeof(*this));
				}
				ImagePixelData& operator=(const ImagePixelData& data)
				{
					memcpy(mData, data.mData, sizeof(*this));
					return *this;
				}
			};

			inline vec4 castPixelToVector(const ImagePixelData& srcData, ImageFormat srcFormat)
			{
				switch (srcFormat)
				{
				case ImageFormat::eR8G8B8A8_UNORM:
					return vec4(srcData.mR8G8B8A8_UNORM) * (1.0f / ((1 << 8) - 1));
				case ImageFormat::eR8_UNORM:
					return vec4(srcData.mR8_UNORM) * (1.0f / ((1 << 8) - 1));
				case ImageFormat::eR16G16B16A16_UNORM:
					return vec4(srcData.mR16G16B16A16_UNORM) * (1.0f / ((1 << 16) - 1));
				case ImageFormat::eR16_UNORM:
					return vec4(srcData.mR16_UNORM) * (1.0f / ((1 << 16) - 1));
					// Signed-Normalized-Float
				case ImageFormat::eR8G8B8A8_SNORM:
					return vec4(srcData.mR8G8B8A8_SNORM) * (1.0f / (1 << 7));
				case ImageFormat::eR8_SNORM:
					return vec4(srcData.mR8_UNORM) * (1.0f / (1 << 7));
				case ImageFormat::eR16G16B16A16_SNORM:
					return vec4(srcData.mR16G16B16A16_SNORM) * (1.0f / ((1 << 15)));
				case ImageFormat::eR16_SNORM:
					return vec4(srcData.mR16_SNORM) * (1.0f / ((1 << 15)));
					// Float
				case ImageFormat::eR32G32B32A32_SFLOAT:
					return srcData.mR32G32B32A32_SFLOAT;
				case ImageFormat::eR32_SFLOAT:
					return vec4(srcData.mR32_SFLOAT);
					// Mixed
					// Compressed
				case ImageFormat::eR5G5B5A1_UNORM:
				{
					return vec4(
						srcData.mR5G5B5A1_UNORM.R5 * (1.0f / ((1 << 5) - 1)),
						srcData.mR5G5B5A1_UNORM.G5 * (1.0f / ((1 << 5) - 1)),
						srcData.mR5G5B5A1_UNORM.B5 * (1.0f / ((1 << 5) - 1)),
						srcData.mR5G5B5A1_UNORM.A1
					);
				}
				// Depth-stencil
				case ImageFormat::eD24_UNORM_S8_UINT:
				{
					return vec4(
						srcData.mD24_UNORM_S8_UINT.D24 * (1.0f / ((1 << 24) - 1)),
						srcData.mD24_UNORM_S8_UINT.S8 * (1.0f / ((1 << 8) - 1)),
						srcData.mD24_UNORM_S8_UINT.S8 * (1.0f / ((1 << 8) - 1)),
						srcData.mD24_UNORM_S8_UINT.S8 * (1.0f / ((1 << 8) - 1))
					);
				}
				case ImageFormat::eD32_SFLOAT:
					return vec4(srcData.mD32_SFLOAT);
				default:
					return vec4(0.0f);
				}
			}

			inline ImagePixelData castVectorToPixel(vec4 value, ImageFormat dstFormat)
			{
				using uvec4 = math::uvec4;
				using i8vec4 = math::Vector<4, int8_t>;
				using u16vec4 = math::Vector<4, uint16_t>;
				using i16vec4 = math::Vector<4, int16_t>;
				ImagePixelData pixel_data;
				switch (dstFormat)
				{
				case ImageFormat::eR8G8B8A8_UNORM:
					pixel_data.mR8G8B8A8_UNORM = u8vec4(value * float((1 << 8) - 1));
					break;
				case ImageFormat::eR8_UNORM:
					pixel_data.mR8_UNORM = value[0] * float((1 << 8) - 1);
					break;
				case ImageFormat::eR16G16B16A16_UNORM:
					pixel_data.mR16G16B16A16_UNORM = u16vec4(value * float((1 << 16) - 1));
					break;
				case ImageFormat::eR16_UNORM:
					pixel_data.mR16_UNORM = value[0] * float((1 << 16) - 1);
					break;
					// Signed-Normalized-Float
				case ImageFormat::eR8G8B8A8_SNORM:
					pixel_data.mR8G8B8A8_SNORM = i8vec4(value * float(1 << 7));
					break;
				case ImageFormat::eR8_SNORM:
					pixel_data.mR8_SNORM = value[0] * float(1 << 7);
					break;
				case ImageFormat::eR16G16B16A16_SNORM:
					pixel_data.mR16G16B16A16_SNORM = i16vec4(value * float(1 << 15));
					break;
				case ImageFormat::eR16_SNORM:
					pixel_data.mR16_SNORM = value[0] * float(1 << 15);
					break;
					// Float
				case ImageFormat::eR32G32B32A32_SFLOAT:
					pixel_data.mR32G32B32A32_SFLOAT = value;
					break;
				case ImageFormat::eR32_SFLOAT:
					pixel_data.mR32_SFLOAT = value[0];
					break;
					// Mixed
					// Compressed
				case ImageFormat::eR5G5B5A1_UNORM:
				{
					pixel_data.mR5G5B5A1_UNORM.R5 = value[0] * float((1 << 5) - 1);
					pixel_data.mR5G5B5A1_UNORM.G5 = value[1] * float((1 << 5) - 1);
					pixel_data.mR5G5B5A1_UNORM.B5 = value[2] * float((1 << 5) - 1);
					pixel_data.mR5G5B5A1_UNORM.A1 = value[3] * float((1 << 1) - 1);
					break;
				}
				// Depth-stencil
				case ImageFormat::eD24_UNORM_S8_UINT:
				{
					pixel_data.mD24_UNORM_S8_UINT.D24 = value[0] * float((1 << 24) - 1);
					pixel_data.mD24_UNORM_S8_UINT.S8 = value[1] * float((1 << 8) - 1);
					break;
				}
				case ImageFormat::eD32_SFLOAT:
					pixel_data.mD32_SFLOAT = value[0];
					break;
				default:
					pixel_data.mR32G32B32A32_SFLOAT = vec4(0.0f);
				}
				return pixel_data;
			}

			inline ivec4 castPixelToVectorInt(const ImagePixelData& srcData, ImageFormat srcFormat)
			{
				switch (srcFormat)
				{
				case ImageFormat::eR8G8B8A8_UNORM:
					return ivec4(srcData.mR8G8B8A8_UNORM);
				case ImageFormat::eR8_UNORM:
					return ivec4(srcData.mR8_UNORM);
				case ImageFormat::eR16G16B16A16_UNORM:
					return ivec4(srcData.mR16G16B16A16_UNORM);
				case ImageFormat::eR16_UNORM:
					return ivec4(srcData.mR16_UNORM);
					// Signed-Normalized-Float
				case ImageFormat::eR8G8B8A8_SNORM:
					return ivec4(srcData.mR8G8B8A8_SNORM);
				case ImageFormat::eR8_SNORM:
					return ivec4(srcData.mR8_UNORM);
				case ImageFormat::eR16G16B16A16_SNORM:
					return ivec4(srcData.mR16G16B16A16_SNORM);
				case ImageFormat::eR16_SNORM:
					return ivec4(srcData.mR16_SNORM);
					// Float
				case ImageFormat::eR32G32B32A32_SFLOAT:
					return ivec4(srcData.mR32G32B32A32_SFLOAT);
				case ImageFormat::eR32_SFLOAT:
					return ivec4(srcData.mR32_SFLOAT);
					// Mixed
					// Compressed
				case ImageFormat::eR5G5B5A1_UNORM:
				{
					return ivec4(
						srcData.mR5G5B5A1_UNORM.R5,
						srcData.mR5G5B5A1_UNORM.G5,
						srcData.mR5G5B5A1_UNORM.B5,
						srcData.mR5G5B5A1_UNORM.A1
					);
				}
				// Depth-stencil
				case ImageFormat::eD24_UNORM_S8_UINT:
				{
					return ivec4(
						srcData.mD24_UNORM_S8_UINT.D24,
						srcData.mD24_UNORM_S8_UINT.S8,
						srcData.mD24_UNORM_S8_UINT.S8,
						srcData.mD24_UNORM_S8_UINT.S8
					);
				}
				case ImageFormat::eD32_SFLOAT:
					return ivec4(srcData.mD32_SFLOAT);
				default:
					return ivec4(0);
				}
			}

		}



		namespace detail
		{
			constexpr uint32_t MaxImageMipLevel = 32;
			constexpr uint32_t MaxImageExtent = (1 << (MaxImageMipLevel - 1)) - 1;

			struct ImageData
			{
				Memory mMemory;
				size_t mMemoryOffset;
				size_t mImageSize;
				void* mMappedPtr;
				ImageFormat mFormat;
				ImageType   mType;
				uint32_t    mPixelBytes;
				ImageExtent mLevelExtents[detail::MaxImageMipLevel];
				size_t      mLevelOffsets[detail::MaxImageMipLevel];
				vec3        mBaseDelta;
				uint32_t    mLayers;
				size_t      mLayerSize;
				uint32_t    mMipLevels;
				uint32_t    mMaxMipLevels;
				uint32_t    mSampleCount;
			};
		}


		class Image
		{
		private:

			detail::ImageData* m_imageData;
		public:
			explicit Image(void* handle) : m_imageData((detail::ImageData*)handle) {}
			Image() : m_imageData(nullptr) {}

			size_t   size() const { return m_imageData->mImageSize; }
			uint32_t width() const { return m_imageData->mLevelExtents[0].mWidth; }
			uint32_t height() const { return m_imageData->mLevelExtents[0].mHeight; }
			uint32_t depth() const { return m_imageData->mLevelExtents[0].mDepth; }
			uint32_t layers() const { return m_imageData->mLayers; }
			uint32_t mipLevels() const { return m_imageData->mMipLevels; }
			uint32_t sampleCount() const { return m_imageData->mSampleCount; }
			ImageFormat format() const { return m_imageData->mFormat; }
			ImageType   type() const { return m_imageData->mType; }

			void unbind()
			{
				m_imageData->mMemory = Memory(nullptr);
				m_imageData->mMemoryOffset = 0;
				m_imageData->mMappedPtr = nullptr;
			}
			void bindMemory(const Memory& mem, size_t offset)
			{
				unbind();
				if (mem.valid())
				{
					if (this->size() > mem.size())
						soft3d_throw_error(ErrorType::eImageMemoryNotEnough);
					if (this->size() > mem.size())
						return;
					m_imageData->mMemoryOffset = offset;
					m_imageData->mMemory = mem;
					m_imageData->mMappedPtr = ((byte*)m_imageData->mMemory.data()) + m_imageData->mMemoryOffset;
				}
			}	

			const Memory& memory() const { return m_imageData->mMemory; }
			void* handle() const { return m_imageData; }
			bool  valid() const { return handle() != nullptr; }

			void writeToFile(const char* filename, uint32_t mipmapLevel, uint32_t layer)
			{
				byte const* data = (byte*)m_imageData->mMappedPtr;

				stbi_write_png(filename, m_imageData->mLevelExtents[mipmapLevel].mWidth, m_imageData->mLevelExtents[mipmapLevel].mHeight, 4,
					data + layer * m_imageData->mLayerSize + m_imageData->mLevelOffsets[mipmapLevel],
					m_imageData->mLevelExtents[mipmapLevel].mWidth * 4);

			}

		};




		Image createImage(
			uint32_t width, 
			uint32_t height,
			uint32_t depth,
			uint32_t layers, 
			ImageType type, 
			ImageFormat format, 
			uint32_t mipLevel,
			uint32_t sampleCount, 
			const MemAllocator& allocator = MemAllocator()
		)
		{
			switch (type)
			{
			case ImageType::eImage1D:
			case ImageType::eImage1DArray:
				soft3d_assert_error(height == 1, ErrorType::eInvalidParam);
			case ImageType::eImage2D:
			case ImageType::eImage2DArray:
				soft3d_assert_error(depth == 1, ErrorType::eInvalidParam);
			}
			switch (type)
			{
			case ImageType::eImage1D:
			case ImageType::eImage2D:
			case ImageType::eImage3D:
				soft3d_assert_error(layers == 1, ErrorType::eInvalidParam);
				break;
			case ImageType::eImageCube:
				soft3d_assert_error(layers == 6, ErrorType::eInvalidParam);
				break;
			}

			auto img_data = (detail::ImageData*)allocator.alloc(sizeof(detail::ImageData));
			img_data->mMemory = Memory(nullptr);
			img_data->mMemoryOffset = 0;
			img_data->mMappedPtr = nullptr;

			img_data->mType = type;
			img_data->mFormat = format;
			img_data->mPixelBytes = detail::getPixelByteSize(format);

			img_data->mLevelExtents[0].mWidth = width;
			img_data->mLevelExtents[0].mHeight = height;
			img_data->mLevelExtents[0].mDepth = depth;
			img_data->mBaseDelta = vec3(img_data->mLevelExtents[0].mWidth, img_data->mLevelExtents[0].mHeight, img_data->mLevelExtents[0].mDepth);
			img_data->mBaseDelta = vec3(math::inverse(img_data->mBaseDelta));

			img_data->mMaxMipLevels = detail::calcMaxMipLevel(img_data->mLevelExtents[0]);
			img_data->mMipLevels = math::clamp(mipLevel, 1U, img_data->mMaxMipLevels);

			for (int i = 1; i < detail::MaxImageMipLevel; i++)
			{
				img_data->mLevelExtents[i] = {};
				img_data->mLevelOffsets[i] = 0;
			}

			for (int i = 1; i < img_data->mMipLevels; i++)
				img_data->mLevelExtents[i] = detail::calcMipLevelExtent(img_data->mLevelExtents[0], i);
			size_t offset = 0;
			for (int i = 0; i < img_data->mMipLevels; i++)
			{
				img_data->mLevelOffsets[i] = offset;
				offset += (size_t)img_data->mPixelBytes * img_data->mLevelExtents[i].mWidth *
					img_data->mLevelExtents[i].mHeight * img_data->mLevelExtents[i].mDepth;
			}

			img_data->mLayerSize = offset;
			img_data->mLayers = layers;
			img_data->mSampleCount = sampleCount;
			img_data->mImageSize = img_data->mLayerSize * img_data->mLayers;
			return Image(img_data);
		}



		void destroyImage(const Image& image, const MemAllocator& allocator = MemAllocator())
		{
			allocator.free(image.handle());
		}


	}
}