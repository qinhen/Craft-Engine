#pragma once
#include "./Common.h"
#include "./ModelFile.h"
#include "./MaterialFile.h"
#include "../advance/opengl/OpenGLSystem.h"
#include "./ModelOGL.h"
namespace CraftEngine
{
	namespace graphics
	{

		class RenderAssetsManager : public AbstractRenderAssetsManager
		{
		private:

			using Texture = opengl::ImageTexture;
			using Buffer = opengl::Buffer;
			using VertexArrayObject = opengl::VertexArrayObject;

			struct ModelRT
			{
				ModelDataRT modelRT;
				MaterialDataRT matRT;
			};

			std::unordered_map<IDType, ModelRT> m_modelMap;
			IDType m_nextKey = 0;
		public:
			virtual IDType loadModel(std::string file_name) override
			{
				bool ok = false;
				ModelFile modelFile;
				MaterialFile matFile;
				ok = modelFile.readFromFile(file_name.c_str());
				if (!ok)
					return -1;
				ok = matFile.loadFromFile(modelFile.mHeader.materialName);
				if (!ok)
					return -1;
				ModelRT modelRT;
				modelRT.modelRT.load(modelFile);
				modelRT.matRT.load(matFile);
				auto id = m_nextKey++;
				m_modelMap.emplace(std::make_pair(id, modelRT));
				return id;
			}

			virtual void freeModel(IDType id) override
			{
				auto& m = m_modelMap[id];
				m.modelRT.clear();
				m.matRT.clear();
				m_modelMap.erase(id);
			}
			const ModelRT& getModel(IDType id) const
			{
				return m_modelMap.find(id)->second;
			}
		};



	}
}


