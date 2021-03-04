#pragma once
#include "./Common.h"

#include "./Image.h"
#include "./Buffer.h"
#include "./Sampler.h"

namespace CraftEngine
{
	namespace soft3d
	{


		namespace detail
		{
			constexpr int MaxShaderResourceImageCount = 32;
			constexpr int MaxShaderResourceBufferCount = 16;
			constexpr int MaxShaderResourceSamplerCount = 8;
			constexpr int MaxShaderResourceUserDataCount = 8;
			constexpr int MaxShaderResourcePushContantSize = 512;
			constexpr int MaxShaderAttributeCount = 4;
			constexpr int MaxPipelineTargetCount = 8;
		}
		struct ShaderResources
		{
			Image mImages[detail::MaxShaderResourceImageCount];
			Buffer mBuffers[detail::MaxShaderResourceBufferCount];
			Sampler mSamplers[detail::MaxShaderResourceSamplerCount];
			void const* mUserDatas[detail::MaxShaderResourceUserDataCount];
			uint8_t mPushContants[detail::MaxShaderResourcePushContantSize];
		};

		struct ShaderVertexPhaseInput
		{
			void const* mAttributes;
			uint32_t mVertex;
			uint32_t mIndex;
			uint32_t mInstance;
		};
		struct ShaderVertexPhaseOutput
		{
			vec4 mAttributes[detail::MaxShaderAttributeCount];
			vec4 mPosition;
		};
		struct ShaderFragmentPhaseInput
		{
			vec4 mAttributes[detail::MaxShaderAttributeCount];
			ivec2 mFragPos;
			float mDepth;
			uint32_t mStencil;
		};
		struct ShaderFragmentPhaseOutput
		{
			vec4 mColors[detail::MaxPipelineTargetCount];
			bool mDiscard;
		};

		using ShaderVertexPhaseFunc = void(*)(const ShaderResources&, const ShaderVertexPhaseInput&, ShaderVertexPhaseOutput&);
		using ShaderFragmentPhaseFunc = void(*)(const ShaderResources&, const ShaderFragmentPhaseInput&, ShaderFragmentPhaseOutput&);



		namespace detail
		{
			struct ShaderData
			{
				ShaderVertexPhaseFunc   mVertexShader;
				ShaderFragmentPhaseFunc mFragmentShader;
			};

			void DefaultVertexShader(const ShaderResources& resources, const ShaderVertexPhaseInput& input, ShaderVertexPhaseOutput& output)
			{
				output.mPosition = ((vec4*)input.mAttributes)[0];
			};
			void DefaultFragmentShader(const ShaderResources& resources, const ShaderFragmentPhaseInput& input, ShaderFragmentPhaseOutput& output)
			{
				output.mColors[0] = input.mAttributes[0];
			};
		}


		class Shader
		{
		private:
			detail::ShaderData* m_shaderData;
		public:
			Shader(void* obj) : m_shaderData((detail::ShaderData*)obj) {}
			Shader() : m_shaderData(nullptr) {}
			void* handle() const { return m_shaderData; }
			bool  valid() const { return handle() != nullptr; }
		};


		Shader createShader(ShaderVertexPhaseFunc vertexShader, ShaderFragmentPhaseFunc fragmentShader, const MemAllocator& allocator = MemAllocator())
		{
			auto shader_data = (detail::ShaderData*)allocator.alloc(sizeof(detail::ShaderData));
			shader_data->mVertexShader = vertexShader;
			if (vertexShader == nullptr)
				shader_data->mVertexShader = detail::DefaultVertexShader;
			shader_data->mFragmentShader = fragmentShader;
			if (fragmentShader == nullptr)
				shader_data->mFragmentShader = detail::DefaultFragmentShader;
			return Shader(shader_data);
		}
		void destroyShader(const Shader& shader, const MemAllocator& allocator = MemAllocator())
		{
			allocator.free(shader.handle());
		}



	}
}