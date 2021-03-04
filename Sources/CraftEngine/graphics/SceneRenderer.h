#pragma once
#include "./Common.h"
#include "./Scene.h"
#include "./RenderResources.h"
#include "./RenderAssetsManager.h"
#include "./light/PhysicalSky.h"
namespace CraftEngine
{
	namespace graphics
	{

		void fillScreen() {
			static unsigned int VAO, EBO;
			if (VAO == 0) {
				unsigned int VBO;

				float vertices[] = {
					//位置
					1.0,	-1.0,	
					1.0,	1.0,
					-1.0,	1.0,
					-1.0,	-1.0,
				};

				unsigned int indices[] = {
					0, 1, 3,		//第一个三角形
					1, 2, 3			//第二个三角形
				};
				glGenVertexArrays(1, &VAO);
				glBindVertexArray(VAO);
				glGenBuffers(1, &VBO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);
				glGenBuffers(1, &EBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
				glBindVertexArray(0);
			}
			//渲染盒子
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		class SceneRenderer : public AbstractSceneRenderer
		{
		private:
			static opengl::VertexArrayObject createVAO()
			{
				opengl::VertexArrayObject vao;
				vao.create();
				vao.format(0, 3, GL_FLOAT, offsetof(Vertex, mPosition), 0);
				vao.format(1, 3, GL_FLOAT, offsetof(Vertex, mNormal), 0);
				vao.format(2, 2, GL_FLOAT, offsetof(Vertex, mTexcoords[0]), 0);
				vao.format(3, 2, GL_FLOAT, offsetof(Vertex, mTexcoords[1]), 0);
				vao.format(4, 3, GL_FLOAT, offsetof(Vertex, mTangent), 0);
				vao.formati(5, 3, GL_UNSIGNED_INT, offsetof(Vertex, mBoneIndices), 0);
				return vao;
			}

			RenderResources* m_resources;

			struct CameraInfo
			{
				math::mat4 view;
				math::mat4 proj;
				math::vec4 viewDir;
				math::vec4 viewPos;
				math::vec4 lightPos;
				math::vec4 lightColor;
				math::vec4 lightAmbient;
			};
			CameraInfo cameraInfo;
			math::vec4 m_frameColor = math::vec4(0.6f, 0.5f, 0.5f, 0.35f);

			opengl::VertexArrayObject VAO;

			opengl::Buffer UBO;
			opengl::FrameBuffer FBO;
			opengl::Shader shader;


			RenderAssetsManager* m_assets;

			opengl::FrameBuffer FBO2;
			opengl::Shader shader2;
			opengl::Buffer UBO2;
			lights::PhysicalSky m_sky;

			struct SkyInfo
			{
				math::mat4 inverseVpMatrix;
				math::vec4 viewPos;
				math::vec4 direction;
				math::vec4 params[10];
				math::vec2 inverseDepthFactor;
			};

			SkyInfo m_skyInfo;
			float m_skyTime = 0.0f;
			float m_skyTurbidity = 4.0f;
			gui::Color m_groundColor = gui::Color(0,0,0,255);
			std::unordered_map<int, IDType> m_instanceIdxMap;


			opengl::FrameBuffer FBO4;
			opengl::Shader shader4;
			std::vector<int> m_frameInstanceList;
		public:
			void clearFrameInstance()
			{
				m_frameInstanceList.clear();
			}

			void addFrameInstance(IDType id)
			{
				m_frameInstanceList.push_back(id);
			}

			int width, height;
			opengl::FrameBuffer FBO3;

			IDType getInstanceID(int x, int y)
			{
				FBO3.bind();
				float p[4];
				glReadPixels(x, height - y, 1, 1, GL_RGBA, GL_FLOAT, p);
				auto it = m_instanceIdxMap.find(round(p[0]));
				if (it == m_instanceIdxMap.end())
					return -1;
				return it->second;
			}

			float getSkyTime()
			{
				return m_skyTime;
			}
			float getSkyTurbidity()
			{
				return m_skyTurbidity;
			}
			gui::Color getGroundColor()
			{
				return m_groundColor;
			}
			void setSkyTime(float t)
			{
				m_skyTime = t;
			}
			void setSkyTurbidity(float t)
			{
				m_skyTurbidity = t;
			}
			void setGroundColor(gui::Color c)
			{
				m_groundColor = c;
			}
			RenderResources* getRenderResources()
			{
				return m_resources;
			}
			
			SceneRenderer(RenderAssetsManager* assets, int w, int h)
			{
				VAO = createVAO();
				m_assets = assets;
				width = w;
				height = h;
				m_resources = new RenderResources();
				m_resources->createColorTarget(width, height);
				m_resources->createDepthTarget(width, height);
				m_resources->createColorTarget(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
				//m_resources->createColorTarget(width, height);

				FBO.create();
				FBO.attachColorAttachment(m_resources->getImage(0), 0);
				FBO.attachDepthAttachment(m_resources->getImage(1));
				FBO.attachColorAttachment(m_resources->getImage(2), 1);
				const GLenum attachments[3] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				};
				glDrawBuffers(2, attachments);
				if (!FBO.check()) {
					std::cout << glGetError();
					throw 0;
				}

				m_resources->createBuffer(GL_UNIFORM_BUFFER, sizeof(CameraInfo));
				shader.loadShaderFromFile("./shaders/glsl/plight.vert", "./shaders/glsl/plight.frag");

				UBO = opengl::createUniformBuffer(sizeof(CameraInfo));


				FBO2.create();
				FBO2.attachColorAttachment(m_resources->getImage(0), 0);
				if (!FBO2.check()) {
					std::cout << glGetError();
					throw 0;
				}
				shader2.loadShaderFromFile("./shaders/glsl/sky.vert", "./shaders/glsl/sky.frag");
				UBO2 = opengl::createUniformBuffer(sizeof(SkyInfo));

				FBO3.create();
				FBO3.attachColorAttachment(m_resources->getImage(2), 0);
				if (!FBO3.check()) {
					std::cout << glGetError();
					throw 0;
				}

				FBO4.create();
				FBO4.attachColorAttachment(m_resources->getImage(0), 0);
				FBO4.attachDepthAttachment(m_resources->getImage(1));
				if (!FBO4.check()) {
					std::cout << glGetError();
					throw 0;
				}
				shader4.loadShaderFromFile("./shaders/glsl/lineframe.vert", "./shaders/glsl/lineframe.frag");
			}

			void resize(int w, int h)
			{
				width = w;
				height = h;
				m_resources->recreateColorTarget(0, width, height);
				m_resources->recreateDepthTarget(1, width, height);
				m_resources->recreateColorTarget(2, width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
				//m_resources->recreateColorTarget(2, width, height);

				FBO.attachColorAttachment(m_resources->getImage(0), 0);
				FBO.attachDepthAttachment(m_resources->getImage(1));
				FBO.attachColorAttachment(m_resources->getImage(2), 1);
				if (!FBO.check()) {
					throw 0;
				}

				FBO2.attachColorAttachment(m_resources->getImage(0), 0);
				if (!FBO2.check()) {
					std::cout << glGetError();
					throw 0;
				}

				FBO3.attachColorAttachment(m_resources->getImage(2), 0);
				if (!FBO3.check()) {
					std::cout << glGetError();
					throw 0;
				}

				FBO4.attachColorAttachment(m_resources->getImage(0), 0);
				FBO4.attachDepthAttachment(m_resources->getImage(1));
				if (!FBO4.check()) {
					std::cout << glGetError();
					throw 0;
				}
			}

			virtual void renderScene(AbstraceScene* absScene) override
			{
				//FBO3.bind();
				//glPixel
				FBO3.bind();
				glViewport(0, 0, width, height);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				Scene* scene = (Scene*)absScene;

				cameraInfo.view = scene->getCamera().matrices.view;
				cameraInfo.proj = scene->getCamera().matrices.perspective;
				cameraInfo.lightPos = { 50.0f, 50.0f, 50.0f, 0.0f };
				cameraInfo.viewPos = math::vec4(scene->getCamera().getPosition(), 0.0f);
				cameraInfo.viewDir = math::vec4(scene->getCamera().getFrontDir(), 0.0f);

				m_skyInfo.inverseVpMatrix = math::transpose(math::inverse(cameraInfo.proj * cameraInfo.view));
				m_skyInfo.inverseDepthFactor = { 1.0f / 0.1, -((64.0 - 0.1) / (64.0 * 0.1)) };
				m_skyInfo.direction = math::vec4(0, math::cos(m_skyTime), math::sin(m_skyTime), 0);
				m_skyInfo.viewPos = math::vec4(scene->getCamera().getPosition(), 0);
				m_sky.setDircetion(m_skyInfo.direction.xyz);
				m_sky.setSkyTurbidity(m_skyTurbidity, math::vec3(m_groundColor.xyz) / 255.0f);
				cameraInfo.lightColor = m_sky.radiosity;
				cameraInfo.lightAmbient = m_sky.ambient;

				memcpy(m_skyInfo.params, m_sky.params, sizeof(m_sky.params));
				
				cameraInfo.lightPos = m_skyInfo.direction * 50.0f;

				FBO2.bind();
				glViewport(0, 0, width, height);
				glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glDisable(GL_DEPTH_TEST);
				glPolygonMode(GL_FRONT, GL_FILL);

				shader2.use();
				UBO2.write(&m_skyInfo, sizeof(m_skyInfo));
				UBO2.bindBase(0);

				fillScreen();


				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				//glDisable(GL_DEPTH_TEST);
				//glDisable(GL_CULL_FACE);

				glCullFace(GL_BACK);
				FBO.bind();
				glClear(GL_DEPTH_BUFFER_BIT);


				shader.use();
				UBO.write(&cameraInfo, sizeof(cameraInfo));
				UBO.bindBase(0);
				VAO.bind();
				auto insList = scene->getInstanceList();
				m_instanceIdxMap.clear();
				int idx = 1;
				for (auto it : insList)
				{
					const auto& ins = scene->getInstance(it);
					const auto& data = m_assets->getModel(ins.mModel);

					shader.setUniform(0, ins.mModelMatrix);
					VAO.bindVertexBuffer(data.modelRT.mVertexBuffer, 0, sizeof(graphics::Vertex), 0);

					m_instanceIdxMap.emplace(std::make_pair(idx, it));
					shader.setUniform(5, float(idx++));

					for (const auto& mesh : data.modelRT.mMeshList)
					{

						auto& mat = data.matRT.mMaterialList[mesh.mMaterial];

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, data.matRT.mTextureList[mat.mMaps[mat.eAlbedoMap]].mImage);
						shader.setUniform(1, 0);
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, data.matRT.mTextureList[mat.mMaps[mat.eNormalMap]].mImage);
						shader.setUniform(2, 1);

						glActiveTexture(GL_TEXTURE2);
						glBindTexture(GL_TEXTURE_2D, data.matRT.mTextureList[mat.mMaps[mat.eMetallicMap]].mImage);
						shader.setUniform(3, 2);
						glActiveTexture(GL_TEXTURE3);
						glBindTexture(GL_TEXTURE_2D, data.matRT.mTextureList[mat.mMaps[mat.eRoughnessMap]].mImage);
						shader.setUniform(4, 3);

						VAO.bindElementBuffer(mesh.mIndexBuffer);
						glDrawElementsBaseVertex(GL_TRIANGLES, mesh.mCount, GL_UNSIGNED_INT, 0, mesh.mBase);
						//glDrawElements(GL_TRIANGLES, mesh.mCount, GL_UNSIGNED_INT, 0);
					}

				}


				FBO4.bind();
				glDisable(GL_DEPTH_TEST);
				//glDepthFunc(GL_LEQUAL);
	
				glPolygonMode(GL_FRONT, GL_LINE);
				shader4.use();
				UBO.bindBase(0);
				shader4.setUniform(1, m_frameColor);
				for (auto it : m_frameInstanceList)
				{
					const auto& ins = scene->getInstance(it);
					const auto& data = m_assets->getModel(ins.mModel);
					shader4.setUniform(0, ins.mModelMatrix);
				
					VAO.bindVertexBuffer(data.modelRT.mVertexBuffer, 0, sizeof(graphics::Vertex), 0);
					for (const auto& mesh : data.modelRT.mMeshList)
					{
						auto& mat = data.matRT.mMaterialList[mesh.mMaterial];

						VAO.bindElementBuffer(mesh.mIndexBuffer);
						glDrawElementsBaseVertex(GL_TRIANGLES, mesh.mCount, GL_UNSIGNED_INT, 0, mesh.mBase);
						//glDrawElements(GL_TRIANGLES, mesh.mCount, GL_UNSIGNED_INT, 0);
					}
				}
				

			}
		};



	}
}


