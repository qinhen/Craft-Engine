#pragma once

#ifndef CRAFT_SOFT3D_GUI_VULKAN_GUI_PIPELINE_H_
#define CRAFT_SOFT3D_GUI_VULKAN_GUI_PIPELINE_H_

#include "../Common.h"
#include "../../soft3d/Soft3D.h"

namespace CraftEngine
{
	namespace gui
	{

		namespace _Soft3D_Render_System_Detail
		{






			class Soft3DGuiRendererPipeline
			{
			public:
				using ShaderVertexPhaseInput = soft3d::ShaderVertexPhaseInput;
				using ShaderVertexPhaseOutput = soft3d::ShaderVertexPhaseOutput;
				using ShaderFragmentPhaseInput = soft3d::ShaderFragmentPhaseInput;
				using ShaderFragmentPhaseOutput = soft3d::ShaderFragmentPhaseOutput;
				using ShaderResources = soft3d::ShaderResources;

				struct RenderInfo
				{
					using vec2 = math::vec2;
					vec2 renderArea;
				};

				struct PushContant0
				{
					Rect ltwh;
					uint32_t color;
				};
				struct PushContant1
				{
					Rect ltwh;
				};
				struct PushContant2
				{
					using vec4 = math::vec4;
					vec4 ltwh;
					Rect uvltwh;
					uint32_t color;
				};

				struct PushContant6
				{
					using mat3 = math::mat3;
					mat3 model;
				};
				
				static void VertexShader0(const ShaderResources& resources, const ShaderVertexPhaseInput& input, ShaderVertexPhaseOutput& output)
				{
					using namespace soft3d;
					vec2 inPos = ((vec2*)input.mAttributes)[0];
					auto& irect = *((PushContant0*)resources.mPushContants);
					auto& renderArea = ((RenderInfo*)resources.mBuffers[0].data())->renderArea;
					auto& color = output.mAttributes[0];
					color = math::unpackUnorm4x8(irect.color);
					auto& poscoord = output.mAttributes[1];
					poscoord.xy = vec2((*((ivec4*)(&irect.ltwh))).xy) + inPos * vec2((*((ivec4*)(&irect.ltwh))).zw);
					poscoord.xy = poscoord.xy * 2.0f / renderArea - 1.0f;
					output.mPosition = vec4(poscoord.xy, 1.0f, 1.0f);
				};
				static void FragmentShader0(const ShaderResources& resources, const ShaderFragmentPhaseInput& input, ShaderFragmentPhaseOutput& output)
				{
					auto& incolor = input.mAttributes[0];
					output.mColors[0] = incolor;
				};

				static void VertexShader1(const ShaderResources& resources, const ShaderVertexPhaseInput& input, ShaderVertexPhaseOutput& output)
				{
					using namespace soft3d;
					vec2 inPos = ((vec2*)input.mAttributes)[0];
					auto& irect = *((PushContant1*)resources.mPushContants);
					auto& renderArea = ((RenderInfo*)resources.mBuffers[0].data())->renderArea;
					auto& texcoord = output.mAttributes[0];
					texcoord.xy = inPos;
					auto& poscoord = output.mAttributes[1];
					poscoord.xy = vec2((*((ivec4*)(&irect.ltwh))).xy) + inPos * vec2((*((ivec4*)(&irect.ltwh))).zw);
					poscoord.xy = poscoord.xy * 2.0f / renderArea - 1.0f;
					output.mPosition = vec4(poscoord.xy, 1.0f, 1.0f);
				};
				static void FragmentShader1(const ShaderResources& resources, const ShaderFragmentPhaseInput& input, ShaderFragmentPhaseOutput& output)
				{
					using soft3d::SamplerFast;
					auto& intexcoord = input.mAttributes[0].xy;
					auto& sampler = (resources.mSamplers[0]);
					auto sampler_fast = SamplerFast(sampler);
					auto& image = resources.mImages[0];
					output.mColors[0] = math::vec4(sampler_fast.texture2D(image, intexcoord, 0, 0)) * (1.0f / 255.0f);
				};
			
				static void VertexShader2(const ShaderResources& resources, const ShaderVertexPhaseInput& input, ShaderVertexPhaseOutput& output)
				{
					using namespace soft3d;
					vec2 inPos = ((vec2*)input.mAttributes)[0];
					auto& irect = *((PushContant2*)resources.mPushContants);
					auto& renderArea = ((RenderInfo*)resources.mBuffers[0].data())->renderArea;

					auto& outColor = output.mAttributes[0];
					outColor = math::unpackUnorm4x8(irect.color);

					auto& outTexcoord = output.mAttributes[1].xy;
					auto& image = resources.mImages[0];
					outTexcoord = vec2((*((ivec4*)&irect.uvltwh)).xy) + inPos * vec2((*((ivec4*)&irect.uvltwh)).zw) + vec2(0.5, 0.5);
					outTexcoord /= vec2(image.width(), image.height());

					auto& outPoscoord = output.mAttributes[1].zw;				
					outPoscoord = (*((vec4*)(&irect.ltwh))).xy + inPos * (*((vec4*)(&irect.ltwh))).zw;
					outPoscoord = outPoscoord * 2.0f / renderArea - 1.0f;
					output.mPosition = vec4(outPoscoord, 1.0f, 1.0f);
				};
				static void FragmentShader2(const ShaderResources& resources, const ShaderFragmentPhaseInput& input, ShaderFragmentPhaseOutput& output)
				{
					using namespace soft3d;
					auto& incolor = input.mAttributes[0];
					auto& intexcoord = input.mAttributes[1].xy;
					auto& sampler = (resources.mSamplers[0]);
					auto& image = resources.mImages[0];
					auto power = sampler.texture2D(image, intexcoord, 0.0f, 0).r;
					if(power == 0.0f)
					{
						output.mDiscard = true;
						return;
					}
					output.mColors[0] = vec4(incolor.xyz, incolor.a * power);
				};


				static void VertexShader4(const ShaderResources& resources, const ShaderVertexPhaseInput& input, ShaderVertexPhaseOutput& output)
				{
					using namespace soft3d;
					vec2 inPos = ((vec2*)input.mAttributes)[0];
					auto& irect = *((PushContant0*)resources.mPushContants);
					auto& renderArea = ((RenderInfo*)resources.mBuffers[0].data())->renderArea;

					auto& outColor = output.mAttributes[0];
					outColor = math::unpackUnorm4x8(irect.color);

					auto& outTexcoord = output.mAttributes[1].xy;
					outTexcoord = inPos;
					auto& outPoscoord = output.mAttributes[2].zw;
					outPoscoord = vec2((*((ivec4*)(&irect.ltwh))).xy) + inPos * vec2((*((ivec4*)(&irect.ltwh))).zw);
					outPoscoord = outPoscoord * 2.0f / renderArea - 1.0f;
					output.mPosition = vec4(outPoscoord, 1.0f, 1.0f);
				};


				static void VertexShader6(const ShaderResources& resources, const ShaderVertexPhaseInput& input, ShaderVertexPhaseOutput& output)
				{
					using namespace soft3d;
					vec2 inPos = ((vec2*)input.mAttributes)[0];
					vec2 inTex = ((vec2*)input.mAttributes)[1];
					uint32_t inCol = ((uint32_t*)(input.mAttributes))[4];
					auto& irect = *((PushContant6*)resources.mPushContants);
					auto& renderArea = ((RenderInfo*)resources.mBuffers[0].data())->renderArea;
	
					auto& color = output.mAttributes[0];
					color = math::unpackUnorm4x8(inCol);
					auto& texcoord = output.mAttributes[1];
					texcoord.xy = inTex;
					vec2 poscoord = (irect.model * math::vec3(inPos, 1.0f)).xy;
					poscoord = poscoord * 2.0f / renderArea - 1.0f;
					output.mPosition = vec4(poscoord, 1.0f, 1.0f);
				};
				static void FragmentShader6(const ShaderResources& resources, const ShaderFragmentPhaseInput& input, ShaderFragmentPhaseOutput& output)
				{
					using soft3d::SamplerFast;
					auto& incolor = input.mAttributes[0];
					auto& intexcoord = input.mAttributes[1].xy;
					auto& sampler = (resources.mSamplers[0]);
					auto& image = resources.mImages[0];
					auto tex_color = sampler.texture2D(image, intexcoord, 0, 0);
					output.mColors[0] = incolor * tex_color;
				};


				using Pipeline = soft3d::Pipeline;
				using Shader = soft3d::Shader;
				
				Pipeline mPipelines[16];
				Shader   mShaders[16];

				void init(

				)
				{

				}

				void clear()
				{
					for (int i = 0; i < 11; i++)
					{
						soft3d::destroyPipeline(mPipelines[i]);
					}
					for (int i = 0; i < 6; i++)
					{
						soft3d::destroyShader(mShaders[i]);
					}
				}


				void create()
				{
					int id = 0;
					mShaders[id++] = soft3d::createShader(VertexShader0, FragmentShader0);
					mShaders[id++] = soft3d::createShader(VertexShader1, FragmentShader1);
					mShaders[id++] = soft3d::createShader(VertexShader2, FragmentShader2);
					mShaders[id++] = soft3d::createShader(VertexShader0, FragmentShader0);
					mShaders[id++] = soft3d::createShader(VertexShader4, FragmentShader2);
					mShaders[id++] = soft3d::createShader(VertexShader0, FragmentShader0);
					mShaders[id++] = soft3d::createShader(VertexShader6, FragmentShader6);
					id = 0;
					mPipelines[id] = soft3d::createPipeline(mShaders[id], sizeof(float) * 2, 1, 
						soft3d::PipelineFrontFace::eClockWire, soft3d::PipelineCullFace::eCullNone, 
						soft3d::PipelinePolygonType::eTriangleList, soft3d::PipelinePolygonMode::eFillMode);
					mPipelines[id].enableColorBlend(true);
					mPipelines[id].enableScissorTest(true);
					id++;
					mPipelines[id] = soft3d::createPipeline(mShaders[id], sizeof(float) * 2, 1,
						soft3d::PipelineFrontFace::eClockWire, soft3d::PipelineCullFace::eCullNone, 
						soft3d::PipelinePolygonType::eTriangleList, soft3d::PipelinePolygonMode::eFillMode);
					mPipelines[id].enableColorBlend(true);
					mPipelines[id].enableScissorTest(true);
					id++;
					mPipelines[id] = soft3d::createPipeline(mShaders[id], sizeof(float) * 2, 1,
						soft3d::PipelineFrontFace::eClockWire, soft3d::PipelineCullFace::eCullNone,
						soft3d::PipelinePolygonType::eTriangleList, soft3d::PipelinePolygonMode::eFillMode);
					mPipelines[id].enableColorBlend(true);
					mPipelines[id].enableScissorTest(true);
					id++;
					mPipelines[id] = soft3d::createPipeline(mShaders[id], sizeof(float) * 2, 1,
						soft3d::PipelineFrontFace::eClockWire, soft3d::PipelineCullFace::eCullNone,
						soft3d::PipelinePolygonType::eLineStrip, soft3d::PipelinePolygonMode::eLineMode);
					mPipelines[id].enableColorBlend(true);
					mPipelines[id].enableScissorTest(true);
					id++;
					mPipelines[id] = soft3d::createPipeline(mShaders[id], sizeof(float) * 2, 1,
						soft3d::PipelineFrontFace::eClockWire, soft3d::PipelineCullFace::eCullNone,
						soft3d::PipelinePolygonType::eTriangleList, soft3d::PipelinePolygonMode::eFillMode);
					mPipelines[id].enableColorBlend(true);
					mPipelines[id].enableScissorTest(true);
					id++;

					//
					mPipelines[id] = soft3d::createPipeline(mShaders[id], sizeof(float) * 2, 1,
						soft3d::PipelineFrontFace::eClockWire, soft3d::PipelineCullFace::eCullNone,
						soft3d::PipelinePolygonType::eLineStrip, soft3d::PipelinePolygonMode::eLineMode);
					mPipelines[id].enableColorBlend(true);
					mPipelines[id].enableScissorTest(true);
					id++;
					//

					mPipelines[id] = soft3d::createPipeline(mShaders[6], sizeof(float) * 5, 1,
						soft3d::PipelineFrontFace::eClockWire, soft3d::PipelineCullFace::eCullNone,
						soft3d::PipelinePolygonType::eTriangleList, soft3d::PipelinePolygonMode::eFillMode);
					mPipelines[id].enableColorBlend(true);
					mPipelines[id].enableScissorTest(true);
					id++;
					mPipelines[id] = soft3d::createPipeline(mShaders[6], sizeof(float) * 5, 1,
						soft3d::PipelineFrontFace::eClockWire, soft3d::PipelineCullFace::eCullNone,
						soft3d::PipelinePolygonType::eTriangleList, soft3d::PipelinePolygonMode::eLineMode);
					mPipelines[id].enableColorBlend(true);
					mPipelines[id].enableScissorTest(true);
					id++;
					mPipelines[id] = soft3d::createPipeline(mShaders[6], sizeof(float) * 5, 1,
						soft3d::PipelineFrontFace::eClockWire, soft3d::PipelineCullFace::eCullNone,
						soft3d::PipelinePolygonType::eLineList, soft3d::PipelinePolygonMode::eLineMode);
					mPipelines[id].enableColorBlend(true);
					mPipelines[id].enableScissorTest(true);
					id++;
					mPipelines[id] = soft3d::createPipeline(mShaders[6], sizeof(float) * 5, 1,
						soft3d::PipelineFrontFace::eClockWire, soft3d::PipelineCullFace::eCullNone,
						soft3d::PipelinePolygonType::eLineStrip, soft3d::PipelinePolygonMode::eLineMode);
					mPipelines[id].enableColorBlend(true);
					mPipelines[id].enableScissorTest(true);
					id++;
					mPipelines[id] = soft3d::createPipeline(mShaders[6], sizeof(float) * 5, 1,
						soft3d::PipelineFrontFace::eClockWire, soft3d::PipelineCullFace::eCullNone,
						soft3d::PipelinePolygonType::ePointList, soft3d::PipelinePolygonMode::ePointMode);
					mPipelines[id].enableColorBlend(true);
					mPipelines[id].enableScissorTest(true);
					id++;


				}

			};

		}

	};



}
#endif // !CRAFT_ENGINE_GUI_VULKAN_GUI_PIPELINE_H_