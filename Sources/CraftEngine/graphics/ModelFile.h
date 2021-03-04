#pragma once
#include "./Common.h"


namespace CraftEngine
{
	namespace graphics
	{

		struct ModelFile
		{
			using vec3 = math::vec3;
			using mat4 = math::mat4;
			using quat = math::quat;
			using VertexData = Vertex;

			struct StringData
			{
				int charOffset;
				int charCount;
			};

			struct BoneData
			{
				mat4  offsetMatrix;
				int   nodeIndex;
			};

			struct BoneBuffer
			{
				std::vector<BoneData> boneData;
			};

			struct BoneHeader
			{
				int  boneDataBase;
				int  boneDataCount;
			};

			struct NodeData
			{
				mat4 transform;
				StringData name;
				int meshIndexOffset;
				int meshIndexCount;
				int childIndexOffset;
				int childIndexCount;
			};

			struct NodeHeader
			{
				int  nodeDataBase;
				int  nodeDataCount;
				int  nodeMeshIndexDataBase;
				int  nodeMeshIndexDataCount;
				int  nodeChildIndexDataBase;
				int  nodeChildIndexDataCount;
			};

			struct MeshData
			{
				int vertexBase;
				int vertexIndexOffset;
				int vertexIndexCount;
				int materialIndex;
			};


			struct PositionKey
			{
				double time;
				vec3 position;
			};
			struct RotationKey
			{
				double time;
				quat rotation;
			};
			struct ScalingKey
			{
				double time;
				vec3 scaling;
			};

			struct AnimationInfoData
			{
				StringData name;
				int channelOffset;
				int channelCount;
				double duration;
				double ticksPerSecond;
			};

			struct AnimationChannelData
			{
				int nodeIndex;
				int boneIndex;
				int positionKeyOffset;
				int positionKeyCount;
				int rotationKeyOffset;
				int rotationKeyCount;
				int scalingKeyOffset;
				int scalingKeyCount;
			};

			struct AnimationBuffer
			{
				std::vector<AnimationInfoData> animationInfoData;
				std::vector<AnimationChannelData> channelData;
				std::vector<PositionKey> positionKeyData;
				std::vector<RotationKey> rotationKeyData;
				std::vector<ScalingKey> scalingKeyData;
			};

			struct AnimationHeader
			{
				int animationInfoDataBase;
				int animationInfoDataCount;
				int channelDataBase;
				int channelDataCount;
				int positionKeyDataBase;
				int positionKeyDataCount;
				int rotationKeyDataBase;
				int rotationKeyDataCount;
				int scalingKeyDataBase;
				int scalingKeyDataCount;
			};

			struct MaterialInfo
			{
				char materialName[128];
				int  materialCount;
				int  materialType;
			};

			struct NodeBuffer
			{
				std::vector<NodeData> nodeData;
				std::vector<int> nodeMeshIndexData;
				std::vector<int> nodeChildIndexData;
			};

			struct MeshHeader
			{
				int  meshDataBase;
				int  meshDataCount;
				int  vertexDataBase;
				int  vertexDataCount;
				int  indexDataBase;
				int  indexDataCount;
			};

			struct MeshBuffer
			{
				std::vector<MeshData> meshData;
				std::vector<VertexData> meshVertexData;
				std::vector<uint32_t> meshIndexData;
			};

			union ModelFileHeader
			{
				struct {
					char title[16];
					char name[32];
					NodeHeader nodeHeader;
					MeshHeader meshHeader;
					BoneHeader boneHeader;
					AnimationHeader animationHeader;
					int  stringBufferDataBase;
					int  stringBufferDataCount;
					int  metaDataBase;
					int  metaDataLength;
					int  fileLength;
					char materialName[128];
				};
				char placeholder[512];
			};

			ModelFileHeader           mHeader;
			NodeBuffer                mNodeBuffer;
			MeshBuffer                mMeshBuffer;
			BoneBuffer                mBoneBuffer;
			AnimationBuffer           mAnimationBuffer;
			std::vector<char>         mStringBuffer;

			bool                      mValid;

			bool checkHeader() const
			{
				int sizeAcc = sizeof(ModelFileHeader);
				if (mHeader.nodeHeader.nodeDataBase != sizeAcc)
					return false;
				sizeAcc += sizeof(NodeData) * mHeader.nodeHeader.nodeDataCount;
				if (mHeader.nodeHeader.nodeMeshIndexDataBase != sizeAcc)
					return false;
				sizeAcc += sizeof(int) * mHeader.nodeHeader.nodeMeshIndexDataCount;
				if (mHeader.nodeHeader.nodeChildIndexDataBase != sizeAcc)
					return false;
				sizeAcc += sizeof(int) * mHeader.nodeHeader.nodeChildIndexDataCount;
				if (mHeader.meshHeader.meshDataBase != sizeAcc)
					return false;
				sizeAcc += sizeof(MeshData) * mHeader.meshHeader.meshDataCount;
				if (mHeader.meshHeader.vertexDataBase != sizeAcc)
					return false;
				sizeAcc += sizeof(VertexData) * mHeader.meshHeader.vertexDataCount;
				if (mHeader.meshHeader.indexDataBase != sizeAcc)
					return false;
				sizeAcc += sizeof(uint32_t) * mHeader.meshHeader.indexDataCount;
				if (mHeader.boneHeader.boneDataBase != sizeAcc)
					return false;
				sizeAcc += sizeof(BoneData) * mHeader.boneHeader.boneDataCount;


				if (mHeader.animationHeader.animationInfoDataBase != sizeAcc)
					return false;
				sizeAcc += sizeof(AnimationInfoData) * mHeader.animationHeader.animationInfoDataCount;
				if (mHeader.animationHeader.channelDataBase != sizeAcc)
					return false;
				sizeAcc += sizeof(AnimationChannelData) * mHeader.animationHeader.channelDataCount;
				if (mHeader.animationHeader.positionKeyDataBase != sizeAcc)
					return false;
				sizeAcc += sizeof(PositionKey) * mHeader.animationHeader.positionKeyDataCount;
				if (mHeader.animationHeader.rotationKeyDataBase != sizeAcc)
					return false;
				sizeAcc += sizeof(RotationKey) * mHeader.animationHeader.rotationKeyDataCount;
				if (mHeader.animationHeader.scalingKeyDataBase != sizeAcc)
					return false;
				sizeAcc += sizeof(ScalingKey) * mHeader.animationHeader.scalingKeyDataCount;

				if (mHeader.stringBufferDataBase != sizeAcc)
					return false;
				sizeAcc += sizeof(char) * mHeader.stringBufferDataCount;

				if (mHeader.fileLength != sizeAcc)
					return false;
				if (mHeader.metaDataLength + sizeof(ModelFileHeader) != sizeAcc)
					return false;
				if (mHeader.metaDataBase != sizeof(ModelFileHeader))
					return false;
				return true;
			}

			void genHeader()
			{

				int sizeAcc = sizeof(ModelFileHeader);
				mHeader.nodeHeader.nodeDataBase = sizeAcc;
				mHeader.nodeHeader.nodeDataCount = mNodeBuffer.nodeData.size();
				sizeAcc += sizeof(NodeData) * mHeader.nodeHeader.nodeDataCount;
				mHeader.nodeHeader.nodeMeshIndexDataBase = sizeAcc;
				mHeader.nodeHeader.nodeMeshIndexDataCount = mNodeBuffer.nodeMeshIndexData.size();
				sizeAcc += sizeof(int) * mHeader.nodeHeader.nodeMeshIndexDataCount;
				mHeader.nodeHeader.nodeChildIndexDataBase = sizeAcc;
				mHeader.nodeHeader.nodeChildIndexDataCount = mNodeBuffer.nodeChildIndexData.size();
				sizeAcc += sizeof(int) * mHeader.nodeHeader.nodeChildIndexDataCount;
				mHeader.meshHeader.meshDataBase = sizeAcc;
				mHeader.meshHeader.meshDataCount = mMeshBuffer.meshData.size();
				sizeAcc += sizeof(MeshData) * mHeader.meshHeader.meshDataCount;
				mHeader.meshHeader.vertexDataBase = sizeAcc;
				mHeader.meshHeader.vertexDataCount = mMeshBuffer.meshVertexData.size();
				sizeAcc += sizeof(VertexData) * mHeader.meshHeader.vertexDataCount;
				mHeader.meshHeader.indexDataBase = sizeAcc;
				mHeader.meshHeader.indexDataCount = mMeshBuffer.meshIndexData.size();
				sizeAcc += sizeof(uint32_t) * mHeader.meshHeader.indexDataCount;
				mHeader.boneHeader.boneDataBase = sizeAcc;
				mHeader.boneHeader.boneDataCount = mBoneBuffer.boneData.size();
				sizeAcc += sizeof(BoneData) * mHeader.boneHeader.boneDataCount;
				//mHeader.boneHeader.boneChildIndexDataBase = sizeAcc;
				//mHeader.boneHeader.boneChildIndexDataCount = mBoneBuffer.boneChildIndexData.size();
				//sizeAcc += sizeof(uint32_t) * mHeader.boneHeader.boneChildIndexDataCount;

				// animation
				mHeader.animationHeader.animationInfoDataBase = sizeAcc;
				mHeader.animationHeader.animationInfoDataCount = mAnimationBuffer.animationInfoData.size();
				sizeAcc += sizeof(AnimationInfoData) * mAnimationBuffer.animationInfoData.size();
				mHeader.animationHeader.channelDataBase = sizeAcc;
				mHeader.animationHeader.channelDataCount = mAnimationBuffer.channelData.size();
				sizeAcc += sizeof(AnimationChannelData) * mAnimationBuffer.channelData.size();
				mHeader.animationHeader.positionKeyDataBase = sizeAcc;
				mHeader.animationHeader.positionKeyDataCount = mAnimationBuffer.positionKeyData.size();
				sizeAcc += sizeof(PositionKey) * mAnimationBuffer.positionKeyData.size();
				mHeader.animationHeader.rotationKeyDataBase = sizeAcc;
				mHeader.animationHeader.rotationKeyDataCount = mAnimationBuffer.rotationKeyData.size();
				sizeAcc += sizeof(RotationKey) * mAnimationBuffer.rotationKeyData.size();
				mHeader.animationHeader.scalingKeyDataBase = sizeAcc;
				mHeader.animationHeader.scalingKeyDataCount = mAnimationBuffer.scalingKeyData.size();
				sizeAcc += sizeof(ScalingKey) * mAnimationBuffer.scalingKeyData.size();

				mHeader.stringBufferDataBase = sizeAcc;
				mHeader.stringBufferDataCount = mStringBuffer.size();
				sizeAcc += sizeof(char) * mStringBuffer.size();

				// file length
				mHeader.fileLength = sizeAcc;
				mHeader.metaDataLength = sizeAcc - sizeof(ModelFileHeader);
				mHeader.metaDataBase = sizeof(ModelFileHeader);
			}

			bool readFromMemory(const void* buffer, size_t size)
			{
				const char* dst = (const char*)buffer;
				memcpy(&mHeader, dst, sizeof(ModelFileHeader));
				if (!checkHeader())
					return false;
				dst += sizeof(ModelFileHeader);

#define ReadFromMemory(type, container, count)\
				container.resize(count);\
				memcpy(container.data(), dst, sizeof(type) * count);\
				dst += sizeof(type) * count;

				mNodeBuffer.nodeData.resize(mHeader.nodeHeader.nodeDataCount);
				memcpy(mNodeBuffer.nodeData.data(), dst, sizeof(NodeData) * mHeader.nodeHeader.nodeDataCount);
				dst += sizeof(NodeData) * mHeader.nodeHeader.nodeDataCount;
				mNodeBuffer.nodeMeshIndexData.resize(mHeader.nodeHeader.nodeMeshIndexDataCount);
				memcpy(mNodeBuffer.nodeMeshIndexData.data(), dst, sizeof(int) * mHeader.nodeHeader.nodeMeshIndexDataCount);
				dst += sizeof(int) * mHeader.nodeHeader.nodeMeshIndexDataCount;
				mNodeBuffer.nodeChildIndexData.resize(mHeader.nodeHeader.nodeChildIndexDataCount);
				memcpy(mNodeBuffer.nodeChildIndexData.data(), dst, sizeof(int) * mHeader.nodeHeader.nodeChildIndexDataCount);
				dst += sizeof(int) * mHeader.nodeHeader.nodeChildIndexDataCount;
				mMeshBuffer.meshData.resize(mHeader.meshHeader.meshDataCount);
				memcpy(mMeshBuffer.meshData.data(), dst, sizeof(MeshData) * mHeader.meshHeader.meshDataCount);
				dst += sizeof(MeshData) * mHeader.meshHeader.meshDataCount;
				mMeshBuffer.meshVertexData.resize(mHeader.meshHeader.vertexDataCount);
				memcpy(mMeshBuffer.meshVertexData.data(), dst, sizeof(VertexData) * mHeader.meshHeader.vertexDataCount);
				dst += sizeof(VertexData) * mHeader.meshHeader.vertexDataCount;
				mMeshBuffer.meshIndexData.resize(mHeader.meshHeader.indexDataCount);
				memcpy(mMeshBuffer.meshIndexData.data(), dst, sizeof(uint32_t) * mHeader.meshHeader.indexDataCount);
				dst += sizeof(uint32_t) * mHeader.meshHeader.indexDataCount;
				mBoneBuffer.boneData.resize(mHeader.boneHeader.boneDataCount);
				memcpy(mBoneBuffer.boneData.data(), dst, sizeof(BoneData) * mHeader.boneHeader.boneDataCount);
				dst += sizeof(BoneData) * mHeader.boneHeader.boneDataCount;
				//mBoneBuffer.boneChildIndexData.resize(mHeader.boneHeader.boneChildIndexDataCount);
				//memcpy(mBoneBuffer.boneChildIndexData.data(), dst, sizeof(uint32_t) * mHeader.boneHeader.boneChildIndexDataCount);
				//dst += sizeof(uint32_t) * mHeader.boneHeader.boneChildIndexDataCount;
				// animation
				ReadFromMemory(AnimationInfoData, mAnimationBuffer.animationInfoData, mHeader.animationHeader.animationInfoDataCount);
				ReadFromMemory(AnimationChannelData, mAnimationBuffer.channelData, mHeader.animationHeader.channelDataCount);
				ReadFromMemory(PositionKey, mAnimationBuffer.positionKeyData, mHeader.animationHeader.positionKeyDataCount);
				ReadFromMemory(RotationKey, mAnimationBuffer.rotationKeyData, mHeader.animationHeader.rotationKeyDataCount);
				ReadFromMemory(ScalingKey, mAnimationBuffer.scalingKeyData, mHeader.animationHeader.scalingKeyDataCount);

				ReadFromMemory(char, mStringBuffer, mHeader.stringBufferDataCount);
#undef ReadFromMemory
				return true;
			}

			bool readFromFile(const char* path)
			{
				std::ifstream file;
				file.open(path, std::ios::binary);
				if (!file.is_open())
					return false;
				// header
				file.read((char*)&mHeader, sizeof(ModelFileHeader));
				// meta data
				mNodeBuffer.nodeData.resize(mHeader.nodeHeader.nodeDataCount);
				file.read((char*)mNodeBuffer.nodeData.data(), sizeof(NodeData) * mHeader.nodeHeader.nodeDataCount);
				mNodeBuffer.nodeMeshIndexData.resize(mHeader.nodeHeader.nodeMeshIndexDataCount);
				file.read((char*)mNodeBuffer.nodeMeshIndexData.data(), sizeof(int) * mHeader.nodeHeader.nodeMeshIndexDataCount);
				mNodeBuffer.nodeChildIndexData.resize(mHeader.nodeHeader.nodeChildIndexDataCount);
				file.read((char*)mNodeBuffer.nodeChildIndexData.data(), sizeof(int) * mHeader.nodeHeader.nodeChildIndexDataCount);
				mMeshBuffer.meshData.resize(mHeader.meshHeader.meshDataCount);
				file.read((char*)mMeshBuffer.meshData.data(), sizeof(MeshData) * mHeader.meshHeader.meshDataCount);
				mMeshBuffer.meshVertexData.resize(mHeader.meshHeader.vertexDataCount);
				file.read((char*)mMeshBuffer.meshVertexData.data(), sizeof(VertexData) * mHeader.meshHeader.vertexDataCount);
				mMeshBuffer.meshIndexData.resize(mHeader.meshHeader.indexDataCount);
				file.read((char*)mMeshBuffer.meshIndexData.data(), sizeof(uint32_t) * mHeader.meshHeader.indexDataCount);
				mBoneBuffer.boneData.resize(mHeader.boneHeader.boneDataCount);
				file.read((char*)mBoneBuffer.boneData.data(), sizeof(BoneData) * mHeader.boneHeader.boneDataCount);
				//mBoneBuffer.boneChildIndexData.resize(mHeader.boneHeader.boneChildIndexDataCount);
				//file.read((char*)mBoneBuffer.boneChildIndexData.data(), sizeof(uint32_t) * mHeader.boneHeader.boneChildIndexDataCount);


				mAnimationBuffer.animationInfoData.resize(mHeader.animationHeader.animationInfoDataCount);
				file.read((char*)mAnimationBuffer.animationInfoData.data(), sizeof(AnimationInfoData) * mHeader.animationHeader.animationInfoDataCount);
				mAnimationBuffer.channelData.resize(mHeader.animationHeader.channelDataCount);
				file.read((char*)mAnimationBuffer.channelData.data(), sizeof(AnimationChannelData) * mHeader.animationHeader.channelDataCount);
				mAnimationBuffer.positionKeyData.resize(mHeader.animationHeader.positionKeyDataCount);
				file.read((char*)mAnimationBuffer.positionKeyData.data(), sizeof(PositionKey) * mHeader.animationHeader.positionKeyDataCount);
				mAnimationBuffer.rotationKeyData.resize(mHeader.animationHeader.rotationKeyDataCount);
				file.read((char*)mAnimationBuffer.rotationKeyData.data(), sizeof(RotationKey) * mHeader.animationHeader.rotationKeyDataCount);
				mAnimationBuffer.scalingKeyData.resize(mHeader.animationHeader.scalingKeyDataCount);
				file.read((char*)mAnimationBuffer.scalingKeyData.data(), sizeof(ScalingKey) * mHeader.animationHeader.scalingKeyDataCount);

				mStringBuffer.resize(mHeader.stringBufferDataCount);
				file.read((char*)mStringBuffer.data(), sizeof(char) * mHeader.stringBufferDataCount);

				return true;
			}

			bool writeToFile(const char* path) const
			{
				if (!checkHeader())
					return false;
				std::ofstream file;
				file.open(path, std::ios::binary);
				if (!file.is_open())
					return false;
				int writeCount = 0;
				// header
				file.write((char*)&mHeader, sizeof(ModelFileHeader));
				// meta data
				file.write((char*)mNodeBuffer.nodeData.data(), sizeof(NodeData) * mHeader.nodeHeader.nodeDataCount);
				file.write((char*)mNodeBuffer.nodeMeshIndexData.data(), sizeof(int) * mHeader.nodeHeader.nodeMeshIndexDataCount);
				file.write((char*)mNodeBuffer.nodeChildIndexData.data(), sizeof(int) * mHeader.nodeHeader.nodeChildIndexDataCount);
				file.write((char*)mMeshBuffer.meshData.data(), sizeof(MeshData) * mHeader.meshHeader.meshDataCount);
				file.write((char*)mMeshBuffer.meshVertexData.data(), sizeof(VertexData) * mHeader.meshHeader.vertexDataCount);
				file.write((char*)mMeshBuffer.meshIndexData.data(), sizeof(uint32_t) * mHeader.meshHeader.indexDataCount);
				file.write((char*)mBoneBuffer.boneData.data(), sizeof(BoneData) * mHeader.boneHeader.boneDataCount);
				//file.write((char*)mBoneBuffer.boneChildIndexData.data(), sizeof(uint32_t) * mHeader.boneHeader.boneChildIndexDataCount);

				file.write((char*)mAnimationBuffer.animationInfoData.data(), sizeof(AnimationInfoData) * mHeader.animationHeader.animationInfoDataCount);
				file.write((char*)mAnimationBuffer.channelData.data(), sizeof(AnimationChannelData) * mHeader.animationHeader.channelDataCount);
				file.write((char*)mAnimationBuffer.positionKeyData.data(), sizeof(PositionKey) * mHeader.animationHeader.positionKeyDataCount);
				file.write((char*)mAnimationBuffer.rotationKeyData.data(), sizeof(RotationKey) * mHeader.animationHeader.rotationKeyDataCount);
				file.write((char*)mAnimationBuffer.scalingKeyData.data(), sizeof(ScalingKey) * mHeader.animationHeader.scalingKeyDataCount);

				file.write((char*)mStringBuffer.data(), sizeof(char) * mHeader.stringBufferDataCount);

				return true;
			}

			bool writeToMemory(void* buffer, size_t size) const
			{
				if (!checkHeader())
					return false;
				if (mHeader.fileLength > size)
					return false;
				char* dst = (char*)buffer;
				memcpy(dst, &mHeader, sizeof(ModelFileHeader));
				dst += sizeof(ModelFileHeader);
				memcpy(dst, mNodeBuffer.nodeData.data(), sizeof(NodeData) * mHeader.nodeHeader.nodeDataCount);
				dst += sizeof(NodeData) * mHeader.nodeHeader.nodeDataCount;
				memcpy(dst, mNodeBuffer.nodeMeshIndexData.data(), sizeof(int) * mHeader.nodeHeader.nodeMeshIndexDataCount);
				dst += sizeof(int) * mHeader.nodeHeader.nodeMeshIndexDataCount;
				memcpy(dst, mNodeBuffer.nodeChildIndexData.data(), sizeof(int) * mHeader.nodeHeader.nodeChildIndexDataCount);
				dst += sizeof(int) * mHeader.nodeHeader.nodeChildIndexDataCount;
				memcpy(dst, mMeshBuffer.meshData.data(), sizeof(MeshData) * mHeader.meshHeader.meshDataCount);
				dst += sizeof(MeshData) * mHeader.meshHeader.meshDataCount;
				memcpy(dst, mMeshBuffer.meshVertexData.data(), sizeof(VertexData) * mHeader.meshHeader.vertexDataCount);
				dst += sizeof(VertexData) * mHeader.meshHeader.vertexDataCount;
				memcpy(dst, mMeshBuffer.meshIndexData.data(), sizeof(uint32_t) * mHeader.meshHeader.indexDataCount);
				dst += sizeof(uint32_t) * mHeader.meshHeader.indexDataCount;
				memcpy(dst, mBoneBuffer.boneData.data(), sizeof(BoneData) * mHeader.boneHeader.boneDataCount);
				dst += sizeof(BoneData) * mHeader.boneHeader.boneDataCount;
				//memcpy(dst, mBoneBuffer.boneChildIndexData.data(), sizeof(uint32_t) * mHeader.boneHeader.boneChildIndexDataCount);
				//dst += sizeof(uint32_t) * mHeader.boneHeader.boneChildIndexDataCount;

				memcpy(dst, mAnimationBuffer.animationInfoData.data(), sizeof(AnimationInfoData) * mHeader.animationHeader.animationInfoDataCount);
				dst += sizeof(AnimationInfoData) * mHeader.animationHeader.animationInfoDataCount;
				memcpy(dst, mAnimationBuffer.channelData.data(), sizeof(AnimationChannelData) * mHeader.animationHeader.channelDataCount);
				dst += sizeof(AnimationChannelData) * mHeader.animationHeader.channelDataCount;
				memcpy(dst, mAnimationBuffer.positionKeyData.data(), sizeof(PositionKey) * mHeader.animationHeader.positionKeyDataCount);
				dst += sizeof(PositionKey) * mHeader.animationHeader.positionKeyDataCount;
				memcpy(dst, mAnimationBuffer.rotationKeyData.data(), sizeof(RotationKey) * mHeader.animationHeader.rotationKeyDataCount);
				dst += sizeof(RotationKey) * mHeader.animationHeader.rotationKeyDataCount;
				memcpy(dst, mAnimationBuffer.scalingKeyData.data(), sizeof(ScalingKey) * mHeader.animationHeader.scalingKeyDataCount);
				dst += sizeof(ScalingKey) * mHeader.animationHeader.scalingKeyDataCount;

				memcpy(dst, mStringBuffer.data(), sizeof(char) * mHeader.stringBufferDataCount);
				dst += sizeof(char) * mHeader.stringBufferDataCount;
				return true;
			}

		};


		struct ModelAuxiliaryData
		{
			struct Node
			{
				Node* parent;
				std::vector<Node*> children;
			};

		};



	}
}


