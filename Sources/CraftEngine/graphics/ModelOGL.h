#pragma once

#include "./Common.h"
#include "./ModelFile.h"
#include "./MaterialFile.h"
#include "../advance/opengl/OpenGLSystem.h"


namespace CraftEngine
{
	namespace graphics
	{


		class ModelOGL
		{
		public:
			using Buffer = opengl::Buffer;
		public:
			struct Mesh
			{
				int mBase;
				int mCount;
				int mMaterial;
				Buffer mIndexBuffer;
			};

			Buffer    mVertexBuffer;
	
			std::vector<Mesh>     mMeshList;
			uint32_t              mTotalVertexCount = 0;
			uint32_t              mTotalIndexCount = 0;
			AABB                  mAABB;

			bool load(const ModelFile& file)
			{
				mAABB.mMin = { FLT_MAX, FLT_MAX, FLT_MAX }; 
				mAABB.mMax = { FLT_MIN, FLT_MIN, FLT_MIN };
				for (auto it : file.mMeshBuffer.meshVertexData)
				{
					for (int i = 0; i < 3; i++)
						if (it.mPosition[i] < mAABB.mMin[i])
							mAABB.mMin[i] = it.mPosition[i];
					for (int i = 0; i < 3; i++)
						if (it.mPosition[i] > mAABB.mMax[i])
							mAABB.mMax[i] = it.mPosition[i];
				}
				mVertexBuffer = opengl::createVertexBuffer(file.mMeshBuffer.meshVertexData.size() * sizeof(Vertex), file.mMeshBuffer.meshVertexData.data());
				for (auto it : file.mMeshBuffer.meshData)
				{
					Mesh mesh;
					mesh.mBase = it.vertexBase;
					mesh.mCount = it.vertexIndexCount;
					mesh.mMaterial = it.materialIndex;
					mesh.mIndexBuffer = opengl::createIndexBuffer(mesh.mCount * sizeof(uint32_t), file.mMeshBuffer.meshIndexData.data() + it.vertexIndexOffset);
					mMeshList.push_back(mesh);
				}
				mTotalVertexCount = file.mMeshBuffer.meshVertexData.size();
				mTotalIndexCount = file.mMeshBuffer.meshIndexData.size();
				return true;
			}

			void clear()
			{
				for (auto it : mMeshList)
					opengl::destroyBuffer(it.mIndexBuffer);
				opengl::destroyBuffer(mVertexBuffer);
			}
		};

		class MaterialDataRT
		{
		public:
			using Texture = opengl::ImageTexture;
			core::ArrayList<Material> mMaterialList;
			core::ArrayList<Texture>  mTextureList;

			void load(MaterialFile file)
			{
				int base = file.mTextureList.size();
				mTextureList.resize(file.mTextureList.size() + file.mColorList.size());
				for (int i = 0; i < file.mTextureList.size(); i++)
				{
					core::Bitmap map;
					map.loadFromFile(file.mTextureList[i].c_str());
					mTextureList[i] = opengl::createTexture2D(map.width(), map.height(), GL_RGBA, map.data(), map.size(), true);
				}

				for (int i = 0; i < file.mColorList.size(); i++)
				{
					uint8_t data[4];
					for (int j = 0; j < 4; j++)
						data[j] = file.mColorList[i][j] * 255.0f;
					mTextureList[base + i] = opengl::createTexture2D(1, 1, GL_RGBA, data, 4, false);
				}

				mMaterialList.resize(file.mMaterialDataList.size());
				for (int i = 0; i < file.mMaterialDataList.size(); i++)
				{
					for (int j = 0; j < Material::eMaxMap; j++)
					{
						auto& info = file.mMaterialDataList[i].mapList[j];
						switch (info.src)
						{
						case MaterialFile::eTextureSrc:
							mMaterialList[i].mMaps[j] = info.index;
							break;
						case MaterialFile::eColorSrc:
							mMaterialList[i].mMaps[j] = base + info.index;
							break;
						case MaterialFile::eDisableSrc:
							mMaterialList[i].mMaps[j] = -1;
							break;
						}
					}
				}

			}

			void clear()
			{
				for (auto it : mTextureList)
					opengl::destroyTexture(it);
			}

		};



		//class RenderAssetsManagerImpl: public AbstractRenderAssetsManager
		//{
		//private:
		//	using Texture = vulkan::ImageTexture;
		//	using Buffer = vulkan::BufferMemory;

		//	struct Material
		//	{
		//		Texture albedo;
		//		Texture normal;
		//		Texture attrib;
		//	};

		//	using vec4 = CraftEngine::math::vec4;
		//	using vec3 = CraftEngine::math::vec3;
		//	using vec2 = CraftEngine::math::vec2;
		//	using usvec4 = CraftEngine::math::Vector<4, unsigned short>;
		//	using uvec3 = CraftEngine::math::uvec3;


		//	core::ObjectPool<Texture, 4096> m_texturePool;
		//	core::ObjectPool<Buffer, 4096> m_bufferPool;
		//	std::unordered_map<std::string, Model> m_modelMap;

		//	vulkan::CommandPool m_cmdPool;
		//	vulkan::LogicalDevice* m_device;
		//public:
		//	RenderAssetsManagerImpl()
		//	{
		//		RenderAssetsManager::setRenderAssetsManager(this);
		//		m_device = vulkan::VulkanSystem::getLogicalDevice();
		//		m_cmdPool = m_device->createCommandPool();
		//	}
		//	~RenderAssetsManagerImpl()
		//	{
		//		m_device->destroy(m_cmdPool);
		//	}

		//	virtual HandleModel loadModel(std::string file_name)override
		//	{
		//		Model model;
		//		if (model.loadFromFile(m_device, file_name, m_cmdPool))
		//		{
		//			m_modelMap[file_name] = model;
		//		}
		//		return HandleModel(&m_modelMap[file_name]);
		//	}
		//	virtual void freeModel(HandleModel model)override
		//	{

		//	}
		//	virtual HandleTexture loadTexture(std::string file_name)override
		//	{
		//		return HandleTexture();
		//	}
		//	virtual void freeTexture(HandleTexture model)override
		//	{

		//	}
		//};


	}
}


