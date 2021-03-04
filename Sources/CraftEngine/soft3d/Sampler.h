#pragma once
#include "./Common.h"
#include "./Image.h"

namespace CraftEngine
{
	namespace soft3d
	{


		enum class SamplerFilterType
		{
			eNearest,
			eLinear,
		};
		enum class SamplerAddressMode
		{
			eRepeat,
			eMirroredRepeat,
			eClampToEdge,
			eClampToBorder,
		};
		enum class SamplerBorderColor
		{
			eWhiteFloat,
			eBlackFloat,
			eWhiteInt,
			eBlackInt,
		};
		enum class SamplerMipmapMode
		{
			eNearestMipmap,
			eLinearMipmap,
		};


		namespace detail
		{

			struct SamplerData
			{
				SamplerFilterType  mFilterType;
				SamplerAddressMode mAddressMode;
				SamplerBorderColor mBorderColor;
				SamplerMipmapMode  mMipmapMode;
				float              mMipmapMinLod;
				float              mMipmapMaxLod;
			};

		}

		class Sampler
		{
		private:
			detail::SamplerData* m_samplerData;
		public:
			explicit Sampler(void* handle) : m_samplerData((detail::SamplerData*)handle) {}
			Sampler() : m_samplerData(nullptr) {}

			SamplerFilterType filterType() const { return m_samplerData->mFilterType; }
			SamplerAddressMode addressMode() const { return m_samplerData->mAddressMode; }
			SamplerBorderColor borderColor() const { return m_samplerData->mBorderColor; }
			SamplerMipmapMode mipmapMode() const { return m_samplerData->mMipmapMode; }
			float minLod() const { return m_samplerData->mMipmapMinLod; }
			float maxLod() const { return m_samplerData->mMipmapMaxLod; }

			vec4 texture1D(const Image& image, const float& texCoord, float mipLevel, int32_t layer) const;
			vec4 texture1D(const Image& image, const float& texCoord, int32_t layer, float delta) const;
			vec4 texture2D(const Image& image, const vec2& texCoord, float mipLevel, int32_t layer) const;
			vec4 texture2D(const Image& image, const vec2& texCoord, int32_t layer, vec2 delta) const;
			vec4 texture3D(const Image& image, const vec3& texCoord, float mipLevel, int32_t layer) const;
			vec4 texture3D(const Image& image, const vec3& texCoord, int32_t layer, vec3 delta) const;

			vec4 textureCube(const Image& image, const vec3& texCoord, float mipLevel) const;
			vec4 textureCube(const Image& image, const vec3& texCoord, vec2 delta) const;

			void* handle() const { return m_samplerData; }
			bool  valid() const { return handle() != nullptr; }
		private:

			vec4 texelFetchImage1D(const Image& image, const int32_t& texCoord, uint32_t mipLevel, uint32_t layer) const;
			vec4 sampleNeaestImage1D(const Image& image, const float& texCoord, uint32_t mipLevel, uint32_t layer) const;
			vec4 sampleLinearImage1D(const Image& image, const float& texCoord, uint32_t mipLevel, uint32_t layer) const;

			vec4 texelFetchImage2D(const Image& image, const ivec2& texCoord, uint32_t mipLevel, uint32_t layer) const;
			vec4 sampleNeaestImage2D(const Image& image, const vec2& texCoord, uint32_t mipLevel, uint32_t layer) const;
			vec4 sampleLinearImage2D(const Image& image, const vec2& texCoord, uint32_t mipLevel, uint32_t layer) const;

			vec4 texelFetchImage3D(const Image& image, const ivec3& texCoord, uint32_t mipLevel, uint32_t layer) const;
			vec4 sampleNeaestImage3D(const Image& image, const vec3& texCoord, uint32_t mipLevel, uint32_t layer) const;
			vec4 sampleLinearImage3D(const Image& image, const vec3& texCoord, uint32_t mipLevel, uint32_t layer) const;

			vec4 texelFetchImageCube(const Image& image, const ivec2& texCoord, uint32_t mipLevel, uint32_t layer) const;
			vec4 sampleNeaestImageCube(const Image& image, const vec2& texCoord, uint32_t mipLevel, uint32_t layer) const;
			vec4 sampleLinearImageCube(const Image& image, const vec2& texCoord, uint32_t mipLevel, uint32_t layer) const;
		};



		Sampler createSampler(
			SamplerFilterType filterType, 
			SamplerAddressMode addressMode, 
			SamplerBorderColor borderColor, 
			SamplerMipmapMode mipmapMode, 
			float mipmapMinLod,
			float mipmapMaxLod,
			const MemAllocator& allocator = MemAllocator()
		)
		{
			auto sampler_data = (detail::SamplerData*)allocator.alloc(sizeof(detail::SamplerData));
			sampler_data->mFilterType = filterType;
			sampler_data->mAddressMode = addressMode;
			sampler_data->mBorderColor = borderColor;
			sampler_data->mMipmapMode = mipmapMode;
			sampler_data->mMipmapMinLod = mipmapMinLod;
			sampler_data->mMipmapMaxLod = mipmapMaxLod;
			return Sampler(sampler_data);
		}

		void destroySampler(const Sampler& sampler, const MemAllocator& allocator = MemAllocator())
		{
			allocator.free(sampler.handle());
		}


	}
}



namespace CraftEngine
{
	namespace soft3d
	{


		inline vec4 Sampler::texture1D(const Image& image, const float& texCoord, float mipLevel, int32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			int32_t layer_calmp = math::clamp(layer, 0, int32_t(image_data->mLayers - 1));
			float   clamp_level_float = math::clamp(mipLevel, 0.0f, float(image_data->mMipLevels - 1));
			int32_t clamp_level = (int32_t)clamp_level_float;
			switch (filterType())
			{
			case SamplerFilterType::eNearest:
				return sampleNeaestImage1D(image, texCoord, clamp_level, layer_calmp);
			case SamplerFilterType::eLinear:
				switch (mipmapMode())
				{
				case SamplerMipmapMode::eNearestMipmap:
					return sampleLinearImage1D(image, texCoord, clamp_level, layer_calmp);
				case SamplerMipmapMode::eLinearMipmap:
				{
					int32_t l1 = clamp_level;
					int32_t l2 = l1 + 1;
					float factor = clamp_level_float - clamp_level;
					if (factor == 0 || l2 < 0 || l2 > image_data->mMipLevels - 1)
						return sampleLinearImage1D(image, texCoord, clamp_level, layer_calmp);
					return (1.0f - factor) * sampleLinearImage1D(image, texCoord, l1, layer_calmp) +
						factor * sampleLinearImage1D(image, texCoord, l2, layer_calmp);
				}
				}
			}
			return sampleNeaestImage1D(image, texCoord, clamp_level, layer_calmp);
		}

		inline vec4 Sampler::texture1D(const Image& image, const float& texCoord, int32_t layer, float delta) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			auto max_level = math::log2(delta / float(image_data->mBaseDelta.x));
			return texture1D(image, texCoord, max_level, layer);
		}

		inline vec4 Sampler::texture2D(const Image& image, const vec2& texCoord, float mipLevel, int32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			int32_t layer_calmp = math::clamp(layer, 0, int32_t(image_data->mLayers - 1));
			float   clamp_level_float = math::clamp(mipLevel, 0.0f, float(image_data->mMipLevels - 1));
			int32_t clamp_level = (int32_t)clamp_level_float;
			switch (filterType())
			{
			case SamplerFilterType::eNearest:
				return sampleNeaestImage2D(image, texCoord, clamp_level, layer_calmp);
			case SamplerFilterType::eLinear:
				switch (mipmapMode())
				{
				case SamplerMipmapMode::eNearestMipmap:
					return sampleLinearImage2D(image, texCoord, clamp_level, layer_calmp);
				case SamplerMipmapMode::eLinearMipmap:
				{
					int32_t l1 = clamp_level;
					int32_t l2 = l1 + 1;
					float factor = clamp_level_float - clamp_level;
					if (factor == 0 || l2 < 0 || l2 > image_data->mMipLevels - 1)
						return sampleLinearImage2D(image, texCoord, clamp_level, layer_calmp);
					return (1.0f - factor) * sampleLinearImage2D(image, texCoord, l1, layer_calmp) +
						factor * sampleLinearImage2D(image, texCoord, l2, layer_calmp);
				}
				}
			}
			return sampleNeaestImage2D(image, texCoord, clamp_level, layer_calmp);
		}

		inline vec4 Sampler::texture2D(const Image& image, const vec2& texCoord, int32_t layer, vec2 delta) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			auto levels = math::log2(delta / vec2(image_data->mBaseDelta.xy));
			auto max_level = math::max(levels.x, levels.y);
			return texture2D(image, texCoord, max_level, layer);
		}

		inline vec4 Sampler::texture3D(const Image& image, const vec3& texCoord, float mipLevel, int32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			int32_t layer_calmp = math::clamp(layer, 0, int32_t(image_data->mLayers - 1));
			float   clamp_level_float = math::clamp(mipLevel, 0.0f, float(image_data->mMipLevels - 1));
			int32_t clamp_level = (int32_t)clamp_level_float;
			switch (filterType())
			{
			case SamplerFilterType::eNearest:
				return sampleNeaestImage3D(image, texCoord, clamp_level, layer_calmp);
			case SamplerFilterType::eLinear:
				switch (mipmapMode())
				{
				case SamplerMipmapMode::eNearestMipmap:
					return sampleLinearImage3D(image, texCoord, clamp_level, layer_calmp);
				case SamplerMipmapMode::eLinearMipmap:
				{
					int32_t l1 = clamp_level;
					int32_t l2 = l1 + 1;
					float factor = clamp_level_float - clamp_level;
					if (factor == 0 || l2 < 0 || l2 > image_data->mMipLevels - 1)
						return sampleLinearImage3D(image, texCoord, clamp_level, layer_calmp);
					return (1.0f - factor) * sampleLinearImage3D(image, texCoord, l1, layer_calmp) +
						factor * sampleLinearImage3D(image, texCoord, l2, layer_calmp);
				}
				}
			}
			return sampleNeaestImage3D(image, texCoord, clamp_level, layer_calmp);
		}

		inline vec4 Sampler::texture3D(const Image& image, const vec3& texCoord, int32_t layer, vec3 delta) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			auto levels = math::log2(delta / image_data->mBaseDelta);
			auto max_level = math::max(levels.x, levels.y, levels.z);
			return texture3D(image, texCoord, max_level, layer);
		}



		inline vec4 Sampler::textureCube(const Image& image, const vec3& texCoord, float mipLevel) const
		{
			int32_t layer = -1;
			float std_distance = 1.0f;
			vec3 inverse_tex_coord = math::inverse(texCoord) * std_distance;
			vec2 other_axis;
			if (texCoord.x > 0)
			{
				other_axis = math::abs(vec2(texCoord.y, texCoord.z) * inverse_tex_coord.x);
				if (other_axis[0] <= std_distance && other_axis[1] <= std_distance)
				{
					other_axis[0] = -other_axis[0];
					layer = 0;
				}
			}
			else if (texCoord.x < 0)
			{
				other_axis = math::abs(vec2(texCoord.y, texCoord.z) * inverse_tex_coord.x);
				if (other_axis[0] <= std_distance && other_axis[1] <= std_distance)
				{
					other_axis = -other_axis;
					layer = 1;
				}
			}
			if (layer >= 0)
			{
				if (texCoord.y > 0)
				{
					other_axis = math::abs(vec2(texCoord.x, texCoord.z) * inverse_tex_coord.y);
					if (other_axis[0] <= std_distance && other_axis[1] <= std_distance)
					{
						other_axis[1] = -other_axis[1];
						layer = 2;
					}
				}
				else if (texCoord.y < 0)
				{
					other_axis = math::abs(vec2(texCoord.x, texCoord.z) * inverse_tex_coord.y);
					if (other_axis[0] <= std_distance && other_axis[1] <= std_distance)
						layer = 3;
				}
			}
			if (layer >= 0)
			{
				if (texCoord.z > 0)
				{
					other_axis = math::abs(vec2(texCoord.x, texCoord.y) * inverse_tex_coord.z);
					if (other_axis[0] <= std_distance && other_axis[1] <= std_distance)
					{
						other_axis = -other_axis;
						layer = 4;
					}
				}
				else if (texCoord.z < 0)
				{
					other_axis = math::abs(vec2(texCoord.x, texCoord.y) * inverse_tex_coord.z);
					if (other_axis[0] <= std_distance && other_axis[1] <= std_distance)
					{
						other_axis[1] = -other_axis[1];
						layer = 5;
					}
				}
			}
			other_axis = other_axis * 0.5f + vec2(0.5f);
	
			auto image_data = (detail::ImageData*)image.handle();
			float   clamp_level_float = math::clamp(mipLevel, 0.0f, float(image_data->mMipLevels - 1));
			int32_t clamp_level = (int32_t)clamp_level_float;
			switch (filterType())
			{
			case SamplerFilterType::eNearest:
				return sampleNeaestImageCube(image, other_axis, clamp_level, layer);
			case SamplerFilterType::eLinear:
				switch (mipmapMode())
				{
				case SamplerMipmapMode::eNearestMipmap:
					return sampleLinearImageCube(image, other_axis, clamp_level, layer);
				case SamplerMipmapMode::eLinearMipmap:
				{
					int32_t l1 = clamp_level;
					int32_t l2 = l1 + 1;
					float factor = clamp_level_float - clamp_level;
					if (factor == 0 || l2 < 0 || l2 > image_data->mMipLevels - 1)
						return sampleLinearImageCube(image, other_axis, clamp_level, layer);
					return (1.0f - factor) * sampleLinearImageCube(image, other_axis, l1, layer) +
						factor * sampleLinearImageCube(image, other_axis, l2, layer);
				}
				}
			}
			return sampleLinearImageCube(image, other_axis, clamp_level, layer);
		}
		inline vec4 Sampler::textureCube(const Image& image, const vec3& texCoord, vec2 delta) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			auto levels = math::log2(delta / vec2(image_data->mBaseDelta));
			auto max_level = math::max(levels.x, levels.y);
			return textureCube(image, texCoord, max_level);
		}




		inline vec4 Sampler::texelFetchImage1D(const Image& image, const int32_t& texCoord, uint32_t mipLevel, uint32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			auto img_data_begin = ((byte*)image_data->mMappedPtr) + image_data->mLayerSize * layer + image_data->mLevelOffsets[mipLevel];
			auto level_extent = int32_t(image_data->mLevelExtents[mipLevel].mWidth);
			auto pixel_bytes = image_data->mPixelBytes;
			void const* texel_data = nullptr;
			auto address = texCoord;
			switch (addressMode()) {
			case SamplerAddressMode::eRepeat:
			{
				address %= level_extent;
				address += level_extent;
				address %= level_extent;
				texel_data = (img_data_begin + address * pixel_bytes);
				break;
			}
			case SamplerAddressMode::eMirroredRepeat:
			{
				auto repeat = address / level_extent;
				address %= level_extent;
				address += level_extent;
				address %= level_extent;
				if (repeat % 2 != 0)address = level_extent - address - 1;
				texel_data = (img_data_begin + address * pixel_bytes);
				break;
			}
			case SamplerAddressMode::eClampToEdge:
			{
				if (address < 0)
					address = 0;
				else if (address >= level_extent)
					address = level_extent - 1;
				texel_data = (img_data_begin + address * pixel_bytes);
				break;
			}
			case SamplerAddressMode::eClampToBorder:
			{
				if (address < 0 || address >= level_extent)
				{
					switch (borderColor())
					{
					case SamplerBorderColor::eWhiteFloat:
						return vec4(1.0f, 1.0f, 1.0f, 0.0f);
						break;
					case SamplerBorderColor::eBlackFloat:
						return vec4(0.0f);
						break;
					default:
						break;
					}
				}
				else
				{
					texel_data = (img_data_begin + address * pixel_bytes);
				}
				break;
			}
			}
			return detail::castPixelToVector(*(const detail::ImagePixelData*)texel_data, image_data->mFormat);
		}

		inline vec4 Sampler::sampleNeaestImage1D(const Image& image, const float& texCoord, uint32_t mipLevel, uint32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			auto level_extent = int32_t(image_data->mLevelExtents[mipLevel].mWidth);
			auto address = texCoord * level_extent;
			auto address_floor = math::floor(address);
			int32_t address_int = int32_t(address_floor);
			return texelFetchImage1D(image, address_int, mipLevel, layer);
		}

		inline vec4 Sampler::sampleLinearImage1D(const Image& image, const float& texCoord, uint32_t mipLevel, uint32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			auto level_extent = int32_t(image_data->mLevelExtents[mipLevel].mWidth);

			auto address = texCoord * level_extent;
			auto address_floor = math::floor(address);
			int32_t address_int = int32_t(address_floor);
			auto address_offset = address - address_floor;
			int  sample_dir = 0;
			if (address_offset < 0.5f)
				sample_dir |= 0x1;

			float factors[2];
			vec4 values[2];
			constexpr const int32_t sample_offsets[2] = {
				-1, 1
			};

			switch (sample_dir) {
			case 1: // -x
			{
				values[0] = texelFetchImage1D(image, address_int + sample_offsets[0], mipLevel, layer);
				values[1] = texelFetchImage1D(image, address_int, mipLevel, layer);
				address_offset = 0.5 - address_offset;
				break;
			}
			case 0: // +x
			{
				values[0] = texelFetchImage1D(image, address_int, mipLevel, layer);
				values[1] = texelFetchImage1D(image, address_int + sample_offsets[1], mipLevel, layer);
				address_offset = 1.5f - address_offset;
				break;
			}
			}
			factors[0] = address_offset;
			factors[1] = (1.0f - address_offset);
			return factors[0] * values[0] + factors[1] * values[1];
		}



		inline vec4 Sampler::texelFetchImage2D(const Image& image, const ivec2& texCoord, uint32_t mipLevel, uint32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			auto img_data_begin = ((byte*)image_data->mMappedPtr) + image_data->mLayerSize * layer + image_data->mLevelOffsets[mipLevel];
			auto level_extent = ivec2(image_data->mLevelExtents[mipLevel].mWidth, image_data->mLevelExtents[mipLevel].mHeight);
			auto pixel_bytes = image_data->mPixelBytes;
			void const* texel_data = nullptr;
			auto address = texCoord;
			switch (addressMode()) {
			case SamplerAddressMode::eRepeat:
			{
				address %= level_extent;
				address += level_extent;
				address %= level_extent;
				texel_data = (img_data_begin + (address.y * level_extent.x + address.x) * pixel_bytes);
				break;
			}
			case SamplerAddressMode::eMirroredRepeat:
			{
				auto repeat = address / level_extent;
				address %= level_extent;
				address += level_extent;
				address %= level_extent;
				if (repeat.x % 2 != 0)address.x = level_extent.x - address.x - 1;
				if (repeat.y % 2 != 0)address.y = level_extent.y - address.y - 1;
				texel_data = (img_data_begin + (address.y * level_extent.x + address.x) * pixel_bytes);
				break;
			}
			case SamplerAddressMode::eClampToEdge:
			{
				if (address.x < 0)
					address.x = 0;
				else if (address.x >= level_extent.x)
					address.x = level_extent.x - 1;
				if (address.y < 0)
					address.y = 0;
				else if (address.y >= level_extent.y)
					address.y = level_extent.y - 1;
				texel_data = (img_data_begin + (address.y * level_extent.x + address.x) * pixel_bytes);
				break;
			}
			case SamplerAddressMode::eClampToBorder:
			{
				if (address.x < 0 || address.y < 0 || address.x >= level_extent.x || address.y >= level_extent.y)
				{
					switch (borderColor())
					{
					case SamplerBorderColor::eWhiteFloat:
						return vec4(1.0f, 1.0f, 1.0f, 0.0f);
						break;
					case SamplerBorderColor::eBlackFloat:
						return vec4(0.0f);
						break;
					default:
						break;
					}
				}
				else
				{
					texel_data = (img_data_begin + (address.y * level_extent.x + address.x) * pixel_bytes);
				}
				break;
			}
			}
			return detail::castPixelToVector(*(const detail::ImagePixelData*)texel_data, image_data->mFormat);
		}

		inline vec4 Sampler::sampleNeaestImage2D(const Image& image, const vec2& texCoord, uint32_t mipLevel, uint32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			vec2 level_extent = vec2(image_data->mLevelExtents[mipLevel].mWidth, image_data->mLevelExtents[mipLevel].mHeight);
			auto address = texCoord * level_extent;
			auto address_floor = math::floor(address);
			ivec2 address_int = ivec2(address_floor);
			return texelFetchImage2D(image, address_int, mipLevel, layer);
		}

		inline vec4 Sampler::sampleLinearImage2D(const Image& image, const vec2& texCoord, uint32_t mipLevel, uint32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			vec2 level_extent = vec2(image_data->mLevelExtents[mipLevel].mWidth, image_data->mLevelExtents[mipLevel].mHeight);

			auto address = texCoord * level_extent;
			auto address_floor = math::floor(address);
			ivec2 address_int = ivec2(address_floor);
			auto address_offset = address - address_floor;
			int  sample_dir = 0;
			if (address_offset.x < 0.5f)
				sample_dir |= 0x1;
			if (address_offset.y < 0.5f)
				sample_dir |= 0x2;

			float factors[4];
			vec4 values[4];
			constexpr const ivec2 sample_offsets[8] = {
				ivec2(1,0),
				ivec2(-1,0),
				ivec2(0,1),
				ivec2(0,-1),
				ivec2(1,1),
				ivec2(1,-1),
				ivec2(-1,1),
				ivec2(-1,-1),
			};

			switch (sample_dir) {
			case 3: // -x -y
			{
				values[0] = texelFetchImage2D(image, address_int + sample_offsets[7], mipLevel, layer);
				values[1] = texelFetchImage2D(image, address_int + sample_offsets[3], mipLevel, layer);
				values[2] = texelFetchImage2D(image, address_int + sample_offsets[1], mipLevel, layer);
				values[3] = texelFetchImage2D(image, address_int, mipLevel, layer);
				address_offset = vec2(0.5f, 0.5f) - address_offset;
				break;
			}
			case 0: // +x +y
			{
				values[0] = texelFetchImage2D(image, address_int, mipLevel, layer);
				values[1] = texelFetchImage2D(image, address_int + sample_offsets[0], mipLevel, layer);
				values[2] = texelFetchImage2D(image, address_int + sample_offsets[2], mipLevel, layer);
				values[3] = texelFetchImage2D(image, address_int + sample_offsets[4], mipLevel, layer);
				address_offset = vec2(1.5f) - address_offset;
				break;
			}
			case 1: // -x +y
			{
				values[0] = texelFetchImage2D(image, address_int + sample_offsets[1], mipLevel, layer);
				values[1] = texelFetchImage2D(image, address_int, mipLevel, layer);
				values[2] = texelFetchImage2D(image, address_int + sample_offsets[6], mipLevel, layer);
				values[3] = texelFetchImage2D(image, address_int + sample_offsets[2], mipLevel, layer);
				address_offset = vec2(0.5f, 1.5f) - address_offset;
				break;
			}
			case 2: // +x -y
			{
				values[0] = texelFetchImage2D(image, address_int + sample_offsets[3], mipLevel, layer);
				values[1] = texelFetchImage2D(image, address_int + sample_offsets[5], mipLevel, layer);
				values[2] = texelFetchImage2D(image, address_int, mipLevel, layer);
				values[3] = texelFetchImage2D(image, address_int + sample_offsets[0], mipLevel, layer);
				address_offset = vec2(1.5f, 0.5f) - address_offset;
				break;
			}
			}
			factors[0] = address_offset.x * address_offset.y;
			factors[1] = (1.0f - address_offset.x) * address_offset.y;
			factors[2] = address_offset.x * (1.0f - address_offset.y);
			factors[3] = (1.0f - address_offset.x) * (1.0f - address_offset.y);
			return factors[0] * values[0] + factors[1] * values[1] + factors[2] * values[2] + factors[3] * values[3];
		}




		inline vec4 Sampler::texelFetchImage3D(const Image& image, const ivec3& texCoord, uint32_t mipLevel, uint32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			auto img_data_begin = ((byte*)image_data->mMappedPtr) + image_data->mLayerSize * layer + image_data->mLevelOffsets[mipLevel];
			auto level_extent = ivec3(image_data->mLevelExtents[mipLevel].mWidth, image_data->mLevelExtents[mipLevel].mHeight, image_data->mLevelExtents[mipLevel].mDepth);
			auto pixel_bytes = image_data->mPixelBytes;
			const void* texel_data = nullptr;
			auto address = texCoord;
			switch (addressMode()) {
			case SamplerAddressMode::eRepeat:
			{
				address %= level_extent;
				address += level_extent;
				address %= level_extent;
				texel_data = (img_data_begin + (address.z * level_extent.y * level_extent.x + address.y * level_extent.x + address.x) * pixel_bytes);
				break;
			}
			case SamplerAddressMode::eMirroredRepeat:
			{
				auto repeat = address / level_extent;
				address %= level_extent;
				address += level_extent;
				address %= level_extent;
				if (repeat.x % 2 != 0)address.x = level_extent.x - address.x - 1;
				if (repeat.y % 2 != 0)address.y = level_extent.y - address.y - 1;
				if (repeat.z % 2 != 0)address.z = level_extent.z - address.z - 1;
				texel_data = (img_data_begin + (address.z * level_extent.y * level_extent.x + address.y * level_extent.x + address.x) * pixel_bytes);
				break;
			}
			case SamplerAddressMode::eClampToEdge:
			{
				if (address.x < 0)
					address.x = 0;
				else if (address.x >= level_extent.x)
					address.x = level_extent.x - 1;
				if (address.y < 0)
					address.y = 0;
				else if (address.y >= level_extent.y)
					address.y = level_extent.y - 1;
				if (address.z < 0)
					address.z = 0;
				else if (address.z >= level_extent.z)
					address.z = level_extent.z - 1;
				texel_data = (img_data_begin + (address.z * level_extent.y * level_extent.x + address.y * level_extent.x + address.x) * pixel_bytes);
				break;
			}
			case SamplerAddressMode::eClampToBorder:
			{
				if (address.x < 0 || address.y < 0 || address.z < 0 || address.x >= level_extent.x || address.y >= level_extent.y || address.z >= level_extent.z)
				{
					switch (borderColor())
					{
					case SamplerBorderColor::eWhiteFloat:
						return vec4(1.0f, 1.0f, 1.0f, 0.0f);
						break;
					case SamplerBorderColor::eBlackFloat:
						return vec4(0.0f);
						break;
					default:
						break;
					}
				}
				else
				{
					texel_data = (img_data_begin + (address.z * level_extent.y * level_extent.x + address.y * level_extent.x + address.x) * pixel_bytes);
				}
				break;
			}
			}
			return detail::castPixelToVector(*(const detail::ImagePixelData*)texel_data, image_data->mFormat);
		}

		inline vec4 Sampler::sampleNeaestImage3D(const Image& image, const vec3& texCoord, uint32_t mipLevel, uint32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			auto level_extent = vec3(image_data->mLevelExtents[mipLevel].mWidth, image_data->mLevelExtents[mipLevel].mHeight, image_data->mLevelExtents[mipLevel].mDepth);
			auto address = texCoord * level_extent;
			auto address_floor = math::floor(address);
			auto address_int = ivec3(address_floor);
			return texelFetchImage3D(image, address_int, mipLevel, layer);
		}

		inline vec4 Sampler::sampleLinearImage3D(const Image& image, const vec3& texCoord, uint32_t mipLevel, uint32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			vec3 level_extent = vec3(image_data->mLevelExtents[mipLevel].mWidth, image_data->mLevelExtents[mipLevel].mHeight, image_data->mLevelExtents[mipLevel].mDepth);
			auto address = texCoord * level_extent;
			auto address_floor = math::floor(address);
			ivec3 address_int = ivec3(address_floor);
			auto address_offset = address - address_floor;
			int  sample_dir = 0;
			if (address_offset.x < 0.5f)
				sample_dir |= 0x1;
			if (address_offset.y < 0.5f)
				sample_dir |= 0x2;
			if (address_offset.z < 0.5f)
				sample_dir |= 0x4;

			float factors[8];
			vec4 values[8];
			constexpr const ivec3 sample_offsets[27] = {
				ivec3(-1,-1,-1),ivec3(0,-1,-1),ivec3(1,-1,-1),
				ivec3(-1,0,-1),ivec3(0,0,-1),ivec3(1,0,-1),
				ivec3(-1,1,-1),ivec3(0,1,-1),ivec3(1,1,-1),

				ivec3(-1,-1,0),ivec3(0,-1,0),ivec3(1,-1,0),
				ivec3(-1,0,0),ivec3(0,0,0),ivec3(1,0,0),
				ivec3(-1,1,0),ivec3(0,1,0),ivec3(1,1,0),

				ivec3(-1,-1,1),ivec3(0,-1,1),ivec3(1,-1,1),
				ivec3(-1,0,1),ivec3(0,0,1),ivec3(1,0,1),
				ivec3(-1,1,1),ivec3(0,1,1),ivec3(1,1,1),
			};

			switch (sample_dir) {
			case 7: // -x -y -z
			{
				values[0] = texelFetchImage3D(image, address_int + sample_offsets[0], mipLevel, layer);
				values[1] = texelFetchImage3D(image, address_int + sample_offsets[1], mipLevel, layer);
				values[2] = texelFetchImage3D(image, address_int + sample_offsets[3], mipLevel, layer);
				values[3] = texelFetchImage3D(image, address_int + sample_offsets[4], mipLevel, layer);
				values[4] = texelFetchImage3D(image, address_int + sample_offsets[9], mipLevel, layer);
				values[5] = texelFetchImage3D(image, address_int + sample_offsets[10], mipLevel, layer);
				values[6] = texelFetchImage3D(image, address_int + sample_offsets[12], mipLevel, layer);
				values[7] = texelFetchImage3D(image, address_int + sample_offsets[13], mipLevel, layer);
				address_offset = vec3(0.5f, 0.5f, 0.5f) - address_offset;
				break;
			}
			case 0: // +x +y +z
			{
				values[0] = texelFetchImage3D(image, address_int + sample_offsets[13], mipLevel, layer);
				values[1] = texelFetchImage3D(image, address_int + sample_offsets[14], mipLevel, layer);
				values[2] = texelFetchImage3D(image, address_int + sample_offsets[16], mipLevel, layer);
				values[3] = texelFetchImage3D(image, address_int + sample_offsets[17], mipLevel, layer);
				values[4] = texelFetchImage3D(image, address_int + sample_offsets[22], mipLevel, layer);
				values[5] = texelFetchImage3D(image, address_int + sample_offsets[23], mipLevel, layer);
				values[6] = texelFetchImage3D(image, address_int + sample_offsets[25], mipLevel, layer);
				values[7] = texelFetchImage3D(image, address_int + sample_offsets[26], mipLevel, layer);
				address_offset = vec3(1.5f) - address_offset;
				break;
			}
			case 1: // -x +y +z
			{
				values[0] = texelFetchImage3D(image, address_int + sample_offsets[12], mipLevel, layer);
				values[1] = texelFetchImage3D(image, address_int + sample_offsets[13], mipLevel, layer);
				values[2] = texelFetchImage3D(image, address_int + sample_offsets[15], mipLevel, layer);
				values[3] = texelFetchImage3D(image, address_int + sample_offsets[16], mipLevel, layer);
				values[4] = texelFetchImage3D(image, address_int + sample_offsets[21], mipLevel, layer);
				values[5] = texelFetchImage3D(image, address_int + sample_offsets[22], mipLevel, layer);
				values[6] = texelFetchImage3D(image, address_int + sample_offsets[24], mipLevel, layer);
				values[7] = texelFetchImage3D(image, address_int + sample_offsets[25], mipLevel, layer);
				address_offset = vec3(0.5f, 1.5f, 1.5f) - address_offset;
				break;
			}
			case 2: // +x -y +z
			{
				values[0] = texelFetchImage3D(image, address_int + sample_offsets[10], mipLevel, layer);
				values[1] = texelFetchImage3D(image, address_int + sample_offsets[11], mipLevel, layer);
				values[2] = texelFetchImage3D(image, address_int + sample_offsets[13], mipLevel, layer);
				values[3] = texelFetchImage3D(image, address_int + sample_offsets[14], mipLevel, layer);
				values[4] = texelFetchImage3D(image, address_int + sample_offsets[19], mipLevel, layer);
				values[5] = texelFetchImage3D(image, address_int + sample_offsets[20], mipLevel, layer);
				values[6] = texelFetchImage3D(image, address_int + sample_offsets[22], mipLevel, layer);
				values[7] = texelFetchImage3D(image, address_int + sample_offsets[23], mipLevel, layer);
				address_offset = vec3(1.5f, 0.5f, 1.5f) - address_offset;
				break;
			}
			case 3: // -x -y +z
			{
				values[0] = texelFetchImage3D(image, address_int + sample_offsets[9], mipLevel, layer);
				values[1] = texelFetchImage3D(image, address_int + sample_offsets[10], mipLevel, layer);
				values[2] = texelFetchImage3D(image, address_int + sample_offsets[12], mipLevel, layer);
				values[3] = texelFetchImage3D(image, address_int + sample_offsets[13], mipLevel, layer);
				values[4] = texelFetchImage3D(image, address_int + sample_offsets[18], mipLevel, layer);
				values[5] = texelFetchImage3D(image, address_int + sample_offsets[19], mipLevel, layer);
				values[6] = texelFetchImage3D(image, address_int + sample_offsets[21], mipLevel, layer);
				values[7] = texelFetchImage3D(image, address_int + sample_offsets[22], mipLevel, layer);
				address_offset = vec3(0.5f, 0.5f, 1.5f) - address_offset;
				break;
			}
			case 4: // +x +y -z
			{
				values[0] = texelFetchImage3D(image, address_int + sample_offsets[4], mipLevel, layer);
				values[1] = texelFetchImage3D(image, address_int + sample_offsets[5], mipLevel, layer);
				values[2] = texelFetchImage3D(image, address_int + sample_offsets[7], mipLevel, layer);
				values[3] = texelFetchImage3D(image, address_int + sample_offsets[8], mipLevel, layer);
				values[4] = texelFetchImage3D(image, address_int + sample_offsets[13], mipLevel, layer);
				values[5] = texelFetchImage3D(image, address_int + sample_offsets[14], mipLevel, layer);
				values[6] = texelFetchImage3D(image, address_int + sample_offsets[16], mipLevel, layer);
				values[7] = texelFetchImage3D(image, address_int + sample_offsets[17], mipLevel, layer);
				address_offset = vec3(1.5f, 1.5f, 0.5f) - address_offset;
				break;
			}
			case 5: // -x +y -z
			{
				values[0] = texelFetchImage3D(image, address_int + sample_offsets[3], mipLevel, layer);
				values[1] = texelFetchImage3D(image, address_int + sample_offsets[4], mipLevel, layer);
				values[2] = texelFetchImage3D(image, address_int + sample_offsets[6], mipLevel, layer);
				values[3] = texelFetchImage3D(image, address_int + sample_offsets[7], mipLevel, layer);
				values[4] = texelFetchImage3D(image, address_int + sample_offsets[12], mipLevel, layer);
				values[5] = texelFetchImage3D(image, address_int + sample_offsets[13], mipLevel, layer);
				values[6] = texelFetchImage3D(image, address_int + sample_offsets[15], mipLevel, layer);
				values[7] = texelFetchImage3D(image, address_int + sample_offsets[16], mipLevel, layer);
				address_offset = vec3(0.5f, 1.5f, 0.5f) - address_offset;
				break;
			}
			case 6: // +x -y -z
			{
				values[0] = texelFetchImage3D(image, address_int + sample_offsets[1], mipLevel, layer);
				values[1] = texelFetchImage3D(image, address_int + sample_offsets[2], mipLevel, layer);
				values[2] = texelFetchImage3D(image, address_int + sample_offsets[4], mipLevel, layer);
				values[3] = texelFetchImage3D(image, address_int + sample_offsets[5], mipLevel, layer);
				values[4] = texelFetchImage3D(image, address_int + sample_offsets[10], mipLevel, layer);
				values[5] = texelFetchImage3D(image, address_int + sample_offsets[11], mipLevel, layer);
				values[6] = texelFetchImage3D(image, address_int + sample_offsets[13], mipLevel, layer);
				values[7] = texelFetchImage3D(image, address_int + sample_offsets[14], mipLevel, layer);
				address_offset = vec3(1.5f, 0.5f, 0.5f) - address_offset;
				break;
			}
			}
			factors[0] = address_offset.x * address_offset.y * address_offset.z;
			factors[1] = (1.0f - address_offset.x) * address_offset.y * address_offset.z;
			factors[2] = address_offset.x * (1.0f - address_offset.y) * address_offset.z;
			factors[3] = (1.0f - address_offset.x) * (1.0f - address_offset.y) * address_offset.z;
			factors[4] = address_offset.x * address_offset.y * (1.0f - address_offset.z);
			factors[5] = (1.0f - address_offset.x) * address_offset.y * (1.0f - address_offset.z);
			factors[6] = address_offset.x * (1.0f - address_offset.y) * (1.0f - address_offset.z);
			factors[7] = (1.0f - address_offset.x) * (1.0f - address_offset.y) * (1.0f - address_offset.z);

			return factors[0] * values[0] + factors[1] * values[1] + factors[2] * values[2] + factors[3] * values[3] +
				factors[4] * values[4] + factors[5] * values[5] + factors[6] * values[6] + factors[7] * values[7];
		}




		inline vec4 Sampler::texelFetchImageCube(const Image& image, const ivec2& texCoord, uint32_t mipLevel, uint32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			auto img_data_begin = ((byte*)image_data->mMappedPtr) + image_data->mLayerSize * layer + image_data->mLevelOffsets[mipLevel];
			auto level_extent = ivec2(image_data->mLevelExtents[mipLevel].mWidth, image_data->mLevelExtents[mipLevel].mHeight);
			auto pixel_bytes = image_data->mPixelBytes;
			const void* texel_data = nullptr;
			auto address = texCoord;
			switch (addressMode()) {
			case SamplerAddressMode::eRepeat:
			{
				address %= level_extent;
				address += level_extent;
				address %= level_extent;
				texel_data = (img_data_begin + (address.y * level_extent.x + address.x) * pixel_bytes);
				break;
			}
			case SamplerAddressMode::eMirroredRepeat:
			{
				auto repeat = address / level_extent;
				address %= level_extent;
				address += level_extent;
				address %= level_extent;
				if (repeat.x % 2 != 0)address.x = level_extent.x - address.x - 1;
				if (repeat.y % 2 != 0)address.y = level_extent.y - address.y - 1;
				texel_data = (img_data_begin + (address.y * level_extent.x + address.x) * pixel_bytes);
				break;
			}
			case SamplerAddressMode::eClampToEdge:
			{
				if (address.x < 0)
					address.x = 0;
				else if (address.x >= level_extent.x)
					address.x = level_extent.x - 1;
				if (address.y < 0)
					address.y = 0;
				else if (address.y >= level_extent.y)
					address.y = level_extent.y - 1;
				texel_data = (img_data_begin + (address.y * level_extent.x + address.x) * pixel_bytes);
				break;
			}
			case SamplerAddressMode::eClampToBorder:
			{
				if (address.x < 0 || address.y < 0 || address.x >= level_extent.x || address.y >= level_extent.y)
				{
					switch (borderColor())
					{
					case SamplerBorderColor::eWhiteFloat:
						return vec4(1.0f, 1.0f, 1.0f, 0.0f);
						break;
					case SamplerBorderColor::eBlackFloat:
						return vec4(0.0f);
						break;
					default:
						break;
					}
				}
				else
				{
					texel_data = (img_data_begin + (address.y * level_extent.x + address.x) * pixel_bytes);
				}
				break;
			}
			}
			return detail::castPixelToVector(*(const detail::ImagePixelData*)texel_data, image_data->mFormat);
		}
		inline vec4 Sampler::sampleNeaestImageCube(const Image& image, const vec2& texCoord, uint32_t mipLevel, uint32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			auto level_extent = vec2(image_data->mLevelExtents[mipLevel].mWidth, image_data->mLevelExtents[mipLevel].mHeight);
			auto address = texCoord * level_extent;
			auto address_floor = math::floor(address);
			auto address_int = ivec2(address_floor);
			return texelFetchImageCube(image, address_int, mipLevel, layer);
		}
		inline vec4 Sampler::sampleLinearImageCube(const Image& image, const vec2& texCoord, uint32_t mipLevel, uint32_t layer) const
		{
			auto image_data = (detail::ImageData*)image.handle();
			vec2 level_extent = vec2(image_data->mLevelExtents[mipLevel].mWidth, image_data->mLevelExtents[mipLevel].mHeight);

			auto address = texCoord * level_extent;
			auto address_floor = math::floor(address);
			ivec2 address_int = ivec2(address_floor);
			auto address_offset = address - address_floor;
			int  sample_dir = 0;
			if (address_offset.x < 0.5f)
				sample_dir |= 0x1;
			if (address_offset.y < 0.5f)
				sample_dir |= 0x2;

			float factors[4];
			vec4 values[4];
			constexpr const ivec2 sample_offsets[8] = {
				ivec2(1,0),
				ivec2(-1,0),
				ivec2(0,1),
				ivec2(0,-1),
				ivec2(1,1),
				ivec2(1,-1),
				ivec2(-1,1),
				ivec2(-1,-1),
			};

			switch (sample_dir) {
			case 0: // +x +y
			{
				values[0] = texelFetchImage2D(image, address_int, mipLevel, layer);
				values[1] = texelFetchImage2D(image, address_int + sample_offsets[0], mipLevel, layer);
				values[2] = texelFetchImage2D(image, address_int + sample_offsets[2], mipLevel, layer);
				values[3] = texelFetchImage2D(image, address_int + sample_offsets[4], mipLevel, layer);
				address_offset = vec2(1.5f) - address_offset;
				break;
			}
			case 1: // -x +y
			{
				values[0] = texelFetchImage2D(image, address_int + sample_offsets[1], mipLevel, layer);
				values[1] = texelFetchImage2D(image, address_int, mipLevel, layer);
				values[2] = texelFetchImage2D(image, address_int + sample_offsets[6], mipLevel, layer);
				values[3] = texelFetchImage2D(image, address_int + sample_offsets[2], mipLevel, layer);
				address_offset = vec2(0.5f, 1.5f) - address_offset;
				break;
			}
			case 2: // +x -y
			{
				values[0] = texelFetchImage2D(image, address_int + sample_offsets[3], mipLevel, layer);
				values[1] = texelFetchImage2D(image, address_int + sample_offsets[5], mipLevel, layer);
				values[2] = texelFetchImage2D(image, address_int, mipLevel, layer);
				values[3] = texelFetchImage2D(image, address_int + sample_offsets[0], mipLevel, layer);
				address_offset = vec2(1.5f, 0.5f) - address_offset;
				break;
			}
			case 3: // -x -y
			{
				values[0] = texelFetchImage2D(image, address_int + sample_offsets[7], mipLevel, layer);
				values[1] = texelFetchImage2D(image, address_int + sample_offsets[3], mipLevel, layer);
				values[2] = texelFetchImage2D(image, address_int + sample_offsets[1], mipLevel, layer);
				values[3] = texelFetchImage2D(image, address_int, mipLevel, layer);
				address_offset = vec2(0.5f, 0.5f) - address_offset;
				break;
			}
			}
			factors[0] = address_offset.x * address_offset.y;
			factors[1] = (1.0f - address_offset.x) * address_offset.y;
			factors[2] = address_offset.x * (1.0f - address_offset.y);
			factors[3] = (1.0f - address_offset.x) * (1.0f - address_offset.y);
			return factors[0] * values[0] + factors[1] * values[1] + factors[2] * values[2] + factors[3] * values[3];
		}





	}
}