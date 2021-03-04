#pragma once
#include "./Common.h"
#include "./ImageView.h"
#include "./Sampler.h"
#include "./SamplerExt.h"


namespace CraftEngine
{
	namespace soft3d
	{



		void imgBlitFast(
			const Image& dstImage,
			const Image& srcImage,
			uint32_t dstMipLevel = 0,
			uint32_t srcMipLevel = 0,
			uint32_t dstLayer = 0,
			uint32_t srcLayer = 0
		);



		void imgBlit(
			const Image& dstImage,
			const Image& srcImage,
			uint32_t dstMipLevel,
			float    srcMipLevel,
			uint32_t dstLayer,
			uint32_t srcLayer,
			Sampler  sampler
		);


		void imgBlit(
			const Image& dstImage,
			const Image& srcImage,
			uint32_t dstMipLevel,
			float    srcMipLevel,
			uint32_t dstLayer,
			uint32_t srcLayer
		);

		void imgBlit(
			const Image& dstImage,
			const Image& srcImage,
			uint32_t dstMipLevel = 0,
			uint32_t dstLayer = 0,
			uint32_t srcLayer = 0
		);

		void imgBlit(
			const ImageView& dstImageView,
			const ImageView& srcImageView
		);

		void imgBlit(
			const ImageView& dstImageView,
			const ImageView& srcImageView,
			Sampler sampler
		);

		void imgGenMipmapFast(
			const Image& image,
			uint32_t level, 
			uint32_t count
		);

		void imgGenMipmap(
			const Image& image,
			uint32_t level = 1,
			uint32_t count = detail::MaxImageMipLevel
		);

		void imgClear(
			const Image& image,
			vec4     clearColor = vec4(0.0f),
			uint32_t mipLevel = 0,
			uint32_t layer = 0
		);

		void imgSetZero(
			const Image& image,
			uint32_t mipLevel = 0,
			uint32_t layer = 0
		);

		void imgSetOne(
			const Image& image,
			uint32_t mipLevel = 0,
			uint32_t layer = 0
		);

		void imgTrans(
			const Image& image,
			uint32_t mipLevel = 0,
			uint32_t layer = 0
		) {
			auto image_data = (detail::ImageData*)image.handle();
			auto img_data_begin = ((byte*)image_data->mMappedPtr) + image_data->mLayerSize * layer + image_data->mLevelOffsets[mipLevel];
			byte* cur_data = img_data_begin;
			byte temp;
			auto level_extent = ivec3(image_data->mLevelExtents[mipLevel].mWidth,
				image_data->mLevelExtents[mipLevel].mHeight,
				image_data->mLevelExtents[mipLevel].mDepth);
			constexpr auto pixel_bytes = 4;
			auto total_pixel = level_extent.x * level_extent.y;
			for (int i = 0; i < total_pixel; i++, cur_data += pixel_bytes)
			{
				temp = cur_data[0];
				cur_data[0] = cur_data[2];
				cur_data[2] = temp;
			}
		}



	}
}




namespace CraftEngine
{
	namespace soft3d
	{



		void imgBlitFast(
			const Image& dstImage,
			const Image& srcImage,
			uint32_t dstMipLevel,
			uint32_t srcMipLevel,
			uint32_t dstLayer,
			uint32_t srcLayer
		)
		{
			if (dstImage.format() != ImageFormat::eR8G8B8A8_UNORM && srcImage.format() != ImageFormat::eR8G8B8A8_UNORM)
				return;
			struct ImageBlit
			{
				uint32_t dstMipLevel;
				uint32_t srcMipLevel;
				uint32_t dstLayer;
				uint32_t srcLayer;
			}info;
			info.dstMipLevel = math::clamp(dstMipLevel, 0U, dstImage.mipLevels() - 1);
			info.dstLayer = math::clamp(dstLayer, 0U, dstImage.layers() - 1);
			info.srcMipLevel = math::clamp(srcMipLevel, 0U, srcImage.mipLevels() - 1);
			info.srcLayer = math::clamp(srcLayer, 0U, srcImage.layers() - 1);

			auto image_data = (detail::ImageData*)dstImage.handle();
			auto img_data_begin = ((byte*)image_data->mMappedPtr) + image_data->mLayerSize * info.dstLayer + image_data->mLevelOffsets[info.dstMipLevel];
			byte* cur_data = img_data_begin;
			auto level_extent = ivec3(image_data->mLevelExtents[info.dstMipLevel].mWidth,
				image_data->mLevelExtents[info.dstMipLevel].mHeight,
				image_data->mLevelExtents[info.dstMipLevel].mDepth);

			auto image_data2 = (detail::ImageData*)srcImage.handle();
			auto img_data_begin2 = ((byte*)image_data2->mMappedPtr) + image_data2->mLayerSize * info.srcLayer + image_data2->mLevelOffsets[info.srcMipLevel];
			byte* cur_data2 = img_data_begin2;
			auto level_extent2 = ivec3(image_data2->mLevelExtents[info.srcMipLevel].mWidth,
				image_data2->mLevelExtents[info.srcMipLevel].mHeight,
				image_data2->mLevelExtents[info.srcMipLevel].mDepth);

			constexpr auto pixel_bytes = 4;
			int x, y;

			switch (dstImage.type())
			{
			case ImageType::eImage2D:
			case ImageType::eImage2DArray:
			case ImageType::eImageCube:
			{
				//vec2 tex_coord;
				for (int h = 0; h < level_extent.y; h++)
				{
					y = h * level_extent2.y / level_extent.y;
					for (int w = 0; w < level_extent.x; w++, cur_data += pixel_bytes)
					{
						x = w * level_extent2.x / level_extent.x;
						cur_data2 = img_data_begin2 + (level_extent2.x * y + x) * pixel_bytes;
						*((uint32_t*)cur_data) = *((uint32_t*)cur_data2);
					}
				}
				break;
			}
			}
		}



		void imgBlit(
			const Image& dstImage,
			const Image& srcImage,
			uint32_t dstMipLevel,
			float    srcMipLevel,
			uint32_t dstLayer,
			uint32_t srcLayer,
			Sampler  sampler
		)
		{
			struct ImageBlit
			{
				uint32_t dstMipLevel;
				float    srcMipLevel;
				uint32_t dstLayer;
				uint32_t srcLayer;
			}info;
			info.dstMipLevel = math::clamp(dstMipLevel, 0U, dstImage.mipLevels() - 1);
			info.dstLayer = math::clamp(dstLayer, 0U, dstImage.layers() - 1);
			info.srcMipLevel = math::clamp(srcMipLevel, 0.0f, float(srcImage.mipLevels() - 1));
			info.srcLayer = math::clamp(srcLayer, 0U, srcImage.layers() - 1);

			auto image_data = (detail::ImageData*)dstImage.handle();
			auto img_data_begin = ((byte*)image_data->mMappedPtr) + image_data->mLayerSize * info.dstLayer + image_data->mLevelOffsets[info.dstMipLevel];
			byte* cur_data = img_data_begin;
			auto level_extent = ivec3(image_data->mLevelExtents[info.dstMipLevel].mWidth,
				image_data->mLevelExtents[info.dstMipLevel].mHeight,
				image_data->mLevelExtents[info.dstMipLevel].mDepth);
			auto pixel_bytes = image_data->mPixelBytes;
			detail::ImagePixelData texel_data;
			vec4 texel_val;

			switch (dstImage.type())
			{
			case ImageType::eImage2D:
			case ImageType::eImage2DArray:
			case ImageType::eImageCube:
			{
				vec2 tex_coord;
				for (int h = 0; h < level_extent.y; h++)
				{
					tex_coord.y = (0.5f + float(h)) / float(level_extent.y);
					for (int w = 0; w < level_extent.x; w++)
					{
						tex_coord.x = (0.5f + float(w)) / float(level_extent.x);
						texel_val = sampler.texture2D(srcImage, tex_coord, info.srcMipLevel, info.srcLayer);
						texel_data = detail::castVectorToPixel(texel_val, image_data->mFormat);
						memcpy(cur_data, texel_data.mData, pixel_bytes);
						cur_data += pixel_bytes;
					}
				}
				break;
			}
			case ImageType::eImage3D:
			case ImageType::eImage3DArray:
			{
				vec3 tex_coord;
				for (int d = 0; d < level_extent.z; d++)
				{
					tex_coord.z = (0.5f + float(d)) / float(level_extent.z);
					for (int h = 0; h < level_extent.y; h++)
					{
						tex_coord.y = (0.5f + float(h)) / float(level_extent.y);
						for (int w = 0; w < level_extent.x; w++)
						{
							tex_coord.x = (0.5f + float(w)) / float(level_extent.x);
							texel_val = sampler.texture3D(srcImage, tex_coord, info.srcMipLevel, info.srcLayer);
							texel_data = detail::castVectorToPixel(texel_val, image_data->mFormat);
							memcpy(cur_data, texel_data.mData, pixel_bytes);
							cur_data += pixel_bytes;
						}
					}
				}
				break;
			}
			case ImageType::eImage1D:
			case ImageType::eImage1DArray:
			{
				float tex_coord;
				for (int w = 0; w < level_extent.x; w++)
				{
					tex_coord = (0.5f + float(w)) / float(level_extent.x);
					texel_val = sampler.texture1D(srcImage, tex_coord, (float)info.srcMipLevel, info.srcLayer);
					texel_data = detail::castVectorToPixel(texel_val, image_data->mFormat);
					memcpy(cur_data, texel_data.mData, pixel_bytes);
					cur_data += pixel_bytes;
				}
				break;
			}
			}
		}





		void imgBlit(
			const Image& dstImage,
			const Image& srcImage,
			uint32_t dstMipLevel,
			float    srcMipLevel,
			uint32_t dstLayer,
			uint32_t srcLayer
		) {
			auto sampler = createSampler(
				SamplerFilterType::eLinear, SamplerAddressMode::eRepeat,
				SamplerBorderColor::eWhiteFloat, SamplerMipmapMode::eLinearMipmap,
				0.0f, 32.0f
			);
			imgBlit(dstImage, srcImage, dstMipLevel, srcMipLevel, dstLayer, srcLayer, sampler);
			destroySampler(sampler);
		}



		void imgBlit(
			const Image& dstImage,
			const Image& srcImage,
			uint32_t dstMipLevel,
			uint32_t dstLayer,
			uint32_t srcLayer
		) {
			auto dst_img_data = (detail::ImageData*)dstImage.handle();
			auto src_img_data = (detail::ImageData*)srcImage.handle();
			auto extent = vec3(srcImage.width(), srcImage.height(), srcImage.depth());
			auto levels = math::log2(dst_img_data->mBaseDelta * extent);
			auto max_level = math::max(levels.x, levels.y, levels.z);
			imgBlit(dstImage, srcImage, dstMipLevel, max_level, dstLayer, srcLayer);
		}






		void imgBlit(
			const ImageView& dstImageView,
			const ImageView& srcImageView
		) {
			auto dst_view_data = (detail::ImageViewData*)dstImageView.handle();
			auto src_view_data = (detail::ImageViewData*)srcImageView.handle();
			imgBlit(dst_view_data->mImage, src_view_data->mImage, dstImageView.baseMipLevel(), srcImageView.baseMipLevel(), dstImageView.baseLayer(), srcImageView.baseLayer());
		}

		void imgBlit(
			const ImageView& dstImageView,
			const ImageView& srcImageView,
			Sampler sampler
		) {
			auto dst_view_data = (detail::ImageViewData*)dstImageView.handle();
			auto src_view_data = (detail::ImageViewData*)srcImageView.handle();
			imgBlit(dst_view_data->mImage, src_view_data->mImage, dstImageView.baseMipLevel(), srcImageView.baseMipLevel(), dstImageView.baseLayer(), srcImageView.baseLayer(), sampler);
		}






		void imgGenMipmapFast(const Image& image, uint32_t level, uint32_t count)
		{
			if (image.type() != ImageType::eImage2D && image.type() != ImageType::eImage2DArray)
				soft3d_throw_error(ErrorType::eWrongImageType);
			if (image.format() != ImageFormat::eR8G8B8A8_UNORM)
				soft3d_throw_error(ErrorType::eWrongImageType);

			auto sampler = createSampler(
				SamplerFilterType::eLinear, SamplerAddressMode::eClampToEdge,
				SamplerBorderColor::eWhiteFloat, SamplerMipmapMode::eNearestMipmap,
				0.0f, 32.0f
			);
			auto sampler_fast = SamplerFast(sampler);

			auto image_data = (detail::ImageData*)image.handle();
			if (level >= image.mipLevels())
				return;
			int begin_level = level;
			int level_count = (level + count) >= image.mipLevels() ? image.mipLevels() - begin_level : count;
			for (int L = 0; L < image_data->mLayers; L++)
			{
				for (int cur_level = begin_level; cur_level < begin_level + level_count; cur_level++)
				{
					auto img_data_begin = ((byte*)image_data->mMappedPtr) + image_data->mLayerSize * L + image_data->mLevelOffsets[cur_level];
					auto level_extent = ivec2(image_data->mLevelExtents[cur_level].mWidth, image_data->mLevelExtents[cur_level].mHeight);
					auto inverse_extent = math::inverse(vec2(level_extent));
					u8vec4 texel_val;
					int  last_level = cur_level - 1;
					vec2 tex_coord;
					byte* cur_data = img_data_begin;
					for (int h = 0; h < level_extent.y; h++)
					{
						tex_coord.y = (0.5f + float(h)) * inverse_extent.y;
						for (int w = 0; w < level_extent.x; w++, cur_data += 4)
						{
							tex_coord.x = (0.5f + float(w)) * inverse_extent.x;
							texel_val = sampler_fast.texture2D(image, tex_coord, last_level, L);
							cur_data[0] = texel_val[0];
							cur_data[1] = texel_val[1];
							cur_data[2] = texel_val[2];
							cur_data[3] = texel_val[3];
						}
					}
				}
			}
			destroySampler(sampler);
		}


		void imgGenMipmap(
			const Image& image,
			uint32_t level,
			uint32_t count
		)
		{
			if (level >= image.mipLevels())
				return;
			int begin_level = level == 0 ? 1 : level;
			int level_count = (begin_level + count) > image.mipLevels() ? image.mipLevels() - begin_level : count;

			if (image.format() == ImageFormat::eR8G8B8A8_UNORM)
			{
				imgGenMipmapFast(image, begin_level, level_count);
				return;
			}
			auto sampler = createSampler(
				SamplerFilterType::eLinear, SamplerAddressMode::eRepeat,
				SamplerBorderColor::eWhiteFloat, SamplerMipmapMode::eNearestMipmap,
				0.0f, 32.0f
			);
			for (int i = 0; i < image.layers(); i++)
				for (int j = begin_level; j < begin_level + level_count; j++)
					imgBlit(image, image, j, j - 1, i, i, sampler);
			destroySampler(sampler);
		}



		void imgClear(
			const Image& image,
			vec4     clearColor,
			uint32_t mipLevel,
			uint32_t layer
		)
		{
			if (mipLevel >= image.mipLevels())
				return;
			if (layer >= image.layers())
				return;
			auto image_data = (detail::ImageData*)image.handle();

			auto img_data_begin = ((byte*)image_data->mMappedPtr) + image_data->mLayerSize * layer + image_data->mLevelOffsets[mipLevel];
			auto level_extent = ivec3(image_data->mLevelExtents[mipLevel].mWidth, image_data->mLevelExtents[mipLevel].mHeight, image_data->mLevelExtents[mipLevel].mDepth);
			detail::ImagePixelData pixel_data;
			pixel_data = detail::castVectorToPixel(clearColor, image.format());
			auto pixel_size = image_data->mPixelBytes;
			byte* cur_data = img_data_begin;
			int w = 0, h = 0, d = 0;
			for (d = 0; d < level_extent.z; d++)
				for (h = 0; h < level_extent.y; h++)
					for (w = 0; w < level_extent.x; w++, cur_data += pixel_size)
						memcpy(cur_data, &pixel_data, pixel_size);
		}

		void imgSetZero(
			const Image& image,
			uint32_t mipLevel,
			uint32_t layer
		)
		{
			if (mipLevel >= image.mipLevels())
				return;
			if (layer >= image.layers())
				return;
			auto image_data = (detail::ImageData*)image.handle();

			auto img_data_begin = ((byte*)image_data->mMappedPtr) + image_data->mLayerSize * layer + image_data->mLevelOffsets[mipLevel];
			auto level_extent = ivec3(image_data->mLevelExtents[mipLevel].mWidth, image_data->mLevelExtents[mipLevel].mHeight, image_data->mLevelExtents[mipLevel].mDepth);
			auto pixel_size = image_data->mPixelBytes;
			auto img_data_count = level_extent.x * level_extent.y * level_extent.z * pixel_size;
			memset(img_data_begin, 0, img_data_count);
		}

		void imgSetOne(
			const Image& image,
			uint32_t mipLevel,
			uint32_t layer
		)
		{
			if (mipLevel >= image.mipLevels())
				return;
			if (layer >= image.layers())
				return;
			auto image_data = (detail::ImageData*)image.handle();

			auto img_data_begin = ((byte*)image_data->mMappedPtr) + image_data->mLayerSize * layer + image_data->mLevelOffsets[mipLevel];
			auto level_extent = ivec3(image_data->mLevelExtents[mipLevel].mWidth, image_data->mLevelExtents[mipLevel].mHeight, image_data->mLevelExtents[mipLevel].mDepth);
			auto pixel_size = image_data->mPixelBytes;
			auto img_data_count = level_extent.x * level_extent.y * level_extent.z * pixel_size;
			memset(img_data_begin, 0xFF, img_data_count);
		}



	}
}


