#pragma once
#ifndef CRAFT_ENGINE_GRAPHICS_COMMON_H_
#define CRAFT_ENGINE_GRAPHICS_COMMON_H_

#include "../core/Core.h"
#include "../core/Bitmap.h"
#include "../math/LinearMath.h"

#include <string>
namespace CraftEngine
{
	namespace graphics
	{

		class Camera;


		typedef int IDType;

		struct Vertex
		{
			using vec2 = math::vec2;
			using vec3 = math::vec3;
			using vec4 = math::vec4;
			using uvec4 = math::Vector<4, uint32_t>;
			vec3   mPosition;
			vec3   mNormal;
			vec2   mTexcoords[2];
			vec3   mTangent;
			uvec4  mBoneIndices;
			vec4   mBoneWeights;
		};

		struct Material
		{
			using vec4 = math::vec4;
			enum MapType
			{
				eAlbedoMap = 0,
				eNormalMap,
				eRoughnessMap,
				eMetallicMap,
				eOccasionMap,
				eSpecularMap,
				eHeightMap,
				eMaxMap,
			};
			IDType mMaps[eMaxMap];
			vec4   mConstantMaps[eMaxMap];
		};

		struct Transform
		{
			using vec2 = math::vec2;
			using vec3 = math::vec3;
			using quat = math::quat;
			vec3 mTranslate = vec3(0.0f);
			vec3 mScale = vec3(1.0f);
			quat mRotation = quat(0,0,0,1);
		};

		struct AABB
		{
			using vec3 = math::vec3;
			vec3 mMin;
			vec3 mMax;
		};

		struct ModelInstance
		{
			using mat4 = math::mat4;
			IDType    mModel;
			Transform mTransform;
			mat4      mModelMatrix = mat4(1.0f);
			AABB      mAABB;
		};


		class AbstraceScene
		{
		public:
			virtual int createInstance(IDType model) = 0;
			virtual ModelInstance& getInstance(IDType id) = 0;
			virtual core::ArrayList<int> getInstanceList() = 0;
			virtual void destroyInstance(IDType id) = 0;
			virtual Camera& getCamera() = 0;
			virtual void updateScene() = 0;
		};

		class AbstractSceneRenderer
		{
		public:
			virtual void renderScene(AbstraceScene* scene) = 0;
		};

		class AbstractRenderAssetsManager
		{
		public:
			virtual IDType loadModel(std::string file_name) = 0;
			virtual void freeModel(IDType model) = 0;
		};



	}
}


#endif // !CRAFT_ENGINE_GRAPHICS_COMMON_H_