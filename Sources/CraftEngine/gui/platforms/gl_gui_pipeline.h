#pragma once
#ifndef CRAFT_ENGINE_GUI_VULKAN_GUI_PIPELINE_H_
#define CRAFT_ENGINE_GUI_VULKAN_GUI_PIPELINE_H_

#include "../../advance/opengl/OpenGLSystem.h"

namespace CraftEngine
{
	namespace gui
	{

		namespace _OpenGL_Render_System_Detail
		{


			class OpenGLGuiRendererPipeline
			{
			private:

				opengl::Shader m_shaders[2];

			public:

				const opengl::Shader& getShader(int idx) { return m_shaders[idx]; }

				void init()
				{


				}

				void clear()
				{
					for (int i = 0; i < 2; i++)
					{
						m_shaders[i].clear();
					}
				}

				void create()
				{
					static const char* source_string[] = {
					{"#version 450\nlayout(location=0)in vec2 inPos;\nlayout(location=1)in vec2 inTex;"
					"\nlayout(location=2)in uint inCol;\nlayout(location=0)out vec4 outColor;\nlayout"
					"(location=1)out vec2 outTexcoord;\nlayout(location=0)uniform mat4 transformMatri"
					"x;\nout gl_PerVertex\n{\nvec4 gl_Position;\n};\nvoid main()\n{\noutColor=unpackU"
					"norm4x8(inCol);\noutTexcoord=inTex;\nvec2 pos=(transformMatrix*vec4(inPos,1.0,1."
					"0)).xy;\ngl_Position=vec4(pos,1.0,1.0);\n}\n"},
					{"#version 450\nlayout(location=1)uniform sampler2D image;\nlayout(location=0)in v"
					"ec4 inColor;\nlayout(location=1)in vec2 inTexcoord;\nlayout(location=0)out vec4 "
					"outColor;\nvoid main()\n{\noutColor=inColor*texture(image,inTexcoord);\n}"},
					{"#version 450\nlayout(location=0)in vec2 inPos;\nlayout(location=1)in vec2 inTex;"
					"\nlayout(location=2)in uint inCol;\nlayout(location=0)out vec4 outColor;\nlayout"
					"(location=1)out vec2 outTexcoord;\nlayout(location=0)uniform mat4 transformMatri"
					"x;\nout gl_PerVertex\n{\nvec4 gl_Position;\n};\nvoid main()\n{\noutColor=unpackU"
					"norm4x8(inCol);\noutTexcoord=inTex;\nvec2 pos=(transformMatrix*vec4(inPos,1.0,1."
					"0)).xy;\ngl_Position=vec4(pos,1.0,1.0);\n}\n"},
					{"#version 450\nlayout(location=1)uniform sampler2D image;\nlayout(location=0)in v"
					"ec4 inColor;\nlayout(location=1)in vec2 inTexcoord;\nlayout(location=0)out vec4 "
					"outColor;\nfloat SingleSapmle(vec2 texcoord)\n{\nfloat sdf=texture(image,texcoor"
					"d).r;\nfloat afwidth=length(vec2(dFdx(sdf),dFdy(sdf)))*0.70710678118;\nreturn sm"
					"oothstep(0.5-afwidth,0.5+afwidth,sdf);\n}\nvoid main()\n{\nfloat power=SingleSap"
					"mle(inTexcoord);\noutColor=vec4(inColor.rgb,inColor.a*power);\n}"},
					}
					;

					for (int i = 0; i < 2; i++)
						if (!m_shaders[i].loadShaderFromMemory(source_string[2 * i], source_string[2 * i + 1], nullptr))
						{
							std::cout << "faild to create shader" + std::to_string(i) << std::endl;
							throw std::runtime_error("faild to create shader" + std::to_string(i));

						}
				

				}




			};



		}

	};



}
#endif // !CRAFT_ENGINE_GUI_VULKAN_GUI_PIPELINE_H_