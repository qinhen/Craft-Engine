#pragma once

#include "./Common.h"
#include "./ModelFile.h"
#include "../3rdparty/ThirdPartyImportAssimp.h"

namespace CraftEngine
{
	namespace graphics
	{

		bool importModel(const std::string& path, ModelFile& file)
		{
			std::string mBasePath;

			Assimp::Importer Importer;
			const aiScene* pScene;
			static const int defaultFlags =
				aiProcessPreset_TargetRealtime_Quality |
				aiProcess_FlipWindingOrder |
				aiProcess_Triangulate | 
				//aiProcess_PreTransformVertices |
				aiProcess_CalcTangentSpace | 
				aiProcess_GenSmoothNormals | 
				aiProcess_FixInfacingNormals | 
				aiProcess_LimitBoneWeights |
				aiProcess_JoinIdenticalVertices |
				//aiProcess_SplitByBoneCount |
				0;
			


			pScene = Importer.ReadFile(path, defaultFlags);
			if (!pScene) {
				std::string error = Importer.GetErrorString();
				std::cout << "[Assimp Libary]Error: " << error << "\n";
				return false;
			}
			auto pos = path.find_last_of('/');
			if (pos != path.npos)
				mBasePath = path.substr(0, pos);
			else
				mBasePath = "";

			//
			file.mStringBuffer.push_back(0);

			// Load meshes
			file.mMeshBuffer.meshData.resize(pScene->mNumMeshes);
			int vertex_count = 0;
			int index_count = 0;
			for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
			{
				const aiMesh* paiMesh = pScene->mMeshes[i];

				aiColor3D pColor(0.f, 0.f, 0.f);
				pScene->mMaterials[paiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);
				
				const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
				graphics::Vertex vertex;
		
				for (unsigned int j = 0; j < paiMesh->mNumVertices; j++)
				{
					const aiVector3D* pPos = &(paiMesh->mVertices[j]);
					const aiVector3D* pNormal = (paiMesh->HasNormals()) ? &(paiMesh->mNormals[j]) : &Zero3D;
					const aiVector3D* pTexCoord = (paiMesh->HasTextureCoords(0)) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;
					const aiVector3D* pTexCoord2 = (paiMesh->HasTextureCoords(1)) ? &(paiMesh->mTextureCoords[1][j]) : &Zero3D;
					const aiVector3D* pBiTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mTangents[j]) : &Zero3D;

					vertex.mPosition.x = pPos->x;
					vertex.mPosition.y = pPos->y;
					vertex.mPosition.z = pPos->z;
					vertex.mNormal.x = pNormal->x;
					vertex.mNormal.y = pNormal->y;
					vertex.mNormal.z = pNormal->z;
					vertex.mTexcoords[0].x = pTexCoord->x;
					vertex.mTexcoords[0].y = 1.0f - pTexCoord->y;
					vertex.mTexcoords[1].x = pTexCoord2->x;
					vertex.mTexcoords[1].y = 1.0f - pTexCoord2->y;
					vertex.mTangent.x = pBiTangent->x;
					vertex.mTangent.y = pBiTangent->y;
					vertex.mTangent.z = pBiTangent->z;

					for (unsigned int k = 0; k < 4; k++)
					{
						vertex.mBoneWeights[k] = 0;
						vertex.mBoneIndices[k] = 0;
					}

					file.mMeshBuffer.meshVertexData.push_back(vertex);
				}

				int curIndexCount = 0;
				for (unsigned int j = 0; j < paiMesh->mNumFaces; j++)
				{
					const aiFace& Face = paiMesh->mFaces[j];
					if (Face.mNumIndices != 3)
						continue;
					file.mMeshBuffer.meshIndexData.push_back(Face.mIndices[0]);
					file.mMeshBuffer.meshIndexData.push_back(Face.mIndices[1]);
					file.mMeshBuffer.meshIndexData.push_back(Face.mIndices[2]);
					curIndexCount += 3;
				}

				file.mMeshBuffer.meshData[i] = {};
				file.mMeshBuffer.meshData[i].vertexBase = vertex_count;
				file.mMeshBuffer.meshData[i].vertexIndexOffset = index_count;
				file.mMeshBuffer.meshData[i].vertexIndexCount = curIndexCount;
				file.mMeshBuffer.meshData[i].materialIndex = pScene->mMeshes[i]->mMaterialIndex;

				index_count = file.mMeshBuffer.meshIndexData.size();
				vertex_count = file.mMeshBuffer.meshVertexData.size();
			}

			// process nodes
			std::unordered_map<decltype(pScene->mRootNode), int> node_idx_mapping;
			std::function<void(decltype(pScene->mRootNode))> process_node;
			process_node = [&node_idx_mapping, &process_node, &file](decltype(pScene->mRootNode) node)
			{
				ModelFile::NodeData node_data;
				node_data.name.charOffset = file.mStringBuffer.size();
				node_data.name.charCount = node->mName.length;
				for (int c_idx = 0; c_idx < node->mName.length; c_idx++)
					file.mStringBuffer.push_back(node->mName.data[c_idx]);
				file.mStringBuffer.push_back(0);

				node_data.childIndexOffset = file.mNodeBuffer.nodeChildIndexData.size();
				node_data.childIndexCount = node->mNumChildren;
				node_data.meshIndexOffset = file.mNodeBuffer.nodeMeshIndexData.size();
				node_data.meshIndexCount = node->mNumMeshes;
				node_idx_mapping[node] = file.mNodeBuffer.nodeData.size();
				for (int x = 0; x < 4; x++)
					for (int y = 0; y < 4; y++)
						node_data.transform[x][y] = node->mTransformation[y][x];			

				file.mNodeBuffer.nodeData.push_back(node_data);
				for (int i = 0; i < node->mNumMeshes; i++)
					file.mNodeBuffer.nodeMeshIndexData.push_back(node->mMeshes[i]);
			
				for (int i = 0; i < node->mNumChildren; i++)
					file.mNodeBuffer.nodeChildIndexData.push_back(file.mNodeBuffer.nodeData.size());
				for (int i = 0; i < node->mNumChildren; i++)
				{
					file.mNodeBuffer.nodeChildIndexData[i + node_data.childIndexOffset] = file.mNodeBuffer.nodeData.size();
					process_node(node->mChildren[i]);
				}
			};
			process_node(pScene->mRootNode);




			// process bones
			std::unordered_map<std::string, int> name_bone_idx_mapping;
			for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
			{
				const aiMesh* paiMesh = pScene->mMeshes[i];
				if (!paiMesh->HasBones())
					continue;
				for (unsigned int j = 0; j < paiMesh->mNumBones; j++)
				{
					const aiBone* paiBone = paiMesh->mBones[j];
					auto it = name_bone_idx_mapping.find(paiBone->mName.data);
					int BoneIndex = file.mBoneBuffer.boneData.size();
					if (it == name_bone_idx_mapping.end())
					{
						name_bone_idx_mapping.emplace(std::make_pair(std::string(paiBone->mName.data), BoneIndex));
						ModelFile::BoneData bone_data;
						for (int x = 0; x < 4; x++)
							for (int y = 0; y < 4; y++)
								bone_data.offsetMatrix[x][y] = paiBone->mOffsetMatrix[y][x];

						auto node = pScene->mRootNode->FindNode(paiBone->mName.data);
						bone_data.nodeIndex = node_idx_mapping[node];

						//std::cout << "bone:" << paiBone->mName.data << " node:" << bone_data.nodeIndex << " idx:" << file.mBoneBuffer.boneData.size() << std::endl;

						file.mBoneBuffer.boneData.push_back(bone_data);
					}
					else
					{
						//assert(0); // Should never reach here.
						BoneIndex = it->second;
					}
					
					for (unsigned int k = 0; k < paiMesh->mBones[j]->mNumWeights; k++)
					{
						int VertexID = file.mMeshBuffer.meshData[i].vertexBase + paiMesh->mBones[j]->mWeights[k].mVertexId;
						float Weight = paiMesh->mBones[j]->mWeights[k].mWeight;
						for (unsigned int l = 0; l < 4; l++)
							if (file.mMeshBuffer.meshVertexData[VertexID].mBoneWeights[l] == 0)
							{
								file.mMeshBuffer.meshVertexData[VertexID].mBoneIndices[l] = BoneIndex;
								file.mMeshBuffer.meshVertexData[VertexID].mBoneWeights[l] = Weight; //(uint8_t)math::clamp(Weight * 255.0f, 0.0f, 255.0f);//
								break;
							}
					}
				}
			}
			//std::vector<int> node_parent_idx;
			//node_parent_idx.resize(file.mNodeBuffer.nodeData.size());
			//node_parent_idx[0] = -1;
			//for (int i = 0; i < file.mNodeBuffer.nodeData.size(); i++)
			//	for (int j = 0; j < file.mNodeBuffer.nodeData[i].childIndexCount; j++)
			//		node_parent_idx[file.mNodeBuffer.nodeChildIndexData[j + file.mNodeBuffer.nodeData[i].childIndexOffset]] = i;

			std::unordered_map<int, int> node_bone_mapping;
			for (int i = 0; i < file.mBoneBuffer.boneData.size(); i++)
				node_bone_mapping.emplace(std::make_pair(file.mBoneBuffer.boneData[i].nodeIndex, i));




			//std::vector<std::vector<int>> bond_children_list;
			//bond_children_list.resize(file.mBoneBuffer.boneData.size());

			//for (int i = 0; i < file.mBoneBuffer.boneData.size(); i++)
			//{
			//	auto& bone_data = file.mBoneBuffer.boneData[i];
			//	int node_index = bone_data.nodeIndex;
			//	while (1)
			//	{
			//		int parent_node_index = node_parent_idx[node_index];
			//		if (parent_node_index < 0)
			//			break;
			//		auto it = node_bone_mapping.find(parent_node_index);
			//		if (it != node_bone_mapping.end())
			//		{
			//			bond_children_list[it->second].push_back(i);
			//			break;
			//		}
			//		node_index = parent_node_index;
			//	}
			//}
			//for (int i = 0; i < file.mBoneBuffer.boneData.size(); i++)
			//{
			//	auto& bone_data = file.mBoneBuffer.boneData[i];
			//	bone_data.childIndexDataOffset = file.mBoneBuffer.boneChildIndexData.size();
			//	bone_data.childIndexDataCount = bond_children_list[i].size();
			//	bone_data.parentIndex = -1;
			//	for (int j = 0; j < bond_children_list[i].size(); j++)
			//		file.mBoneBuffer.boneChildIndexData.push_back(bond_children_list[i][j]);
			//}
			//for (int i = 0; i < file.mBoneBuffer.boneData.size(); i++)
			//{
			//	auto& bone_data = file.mBoneBuffer.boneData[i];
			//	for (int j = 0; j < bone_data.childIndexDataCount; j++)
			//		file.mBoneBuffer.boneData[file.mBoneBuffer.boneChildIndexData[j + bone_data.childIndexDataOffset]].parentIndex = i;
			//}

			// load animations
			if (pScene->HasAnimations())
			{
				for (int a = 0; a < pScene->mNumAnimations; a++)
				{
					auto pAnimation = pScene->mAnimations[a];
					ModelFile::AnimationInfoData animation_info;
					animation_info.channelOffset = file.mAnimationBuffer.animationInfoData.size();
					animation_info.channelCount = pAnimation->mNumChannels;
					animation_info.duration = pAnimation->mDuration;
					animation_info.ticksPerSecond = pAnimation->mTicksPerSecond;

					animation_info.name.charOffset = file.mStringBuffer.size();
					animation_info.name.charCount = pAnimation->mName.length;
					for (int c_idx = 0; c_idx < pAnimation->mName.length; c_idx++)
						file.mStringBuffer.push_back(pAnimation->mName.data[c_idx]);
					file.mStringBuffer.push_back(0);

					file.mAnimationBuffer.animationInfoData.emplace_back(animation_info);
					for (int c = 0; c < pAnimation->mNumChannels; c++)
					{
						auto pChannel = pAnimation->mChannels[c];
						ModelFile::AnimationChannelData channel_data;
						channel_data.nodeIndex = node_idx_mapping[pScene->mRootNode->FindNode(pChannel->mNodeName)];
						auto it = node_bone_mapping.find(channel_data.nodeIndex);
						channel_data.boneIndex = it != node_bone_mapping.end() ? it->second : -1;

						channel_data.positionKeyOffset = file.mAnimationBuffer.positionKeyData.size();
						channel_data.positionKeyCount = pChannel->mNumPositionKeys;
						channel_data.rotationKeyOffset = file.mAnimationBuffer.rotationKeyData.size();
						channel_data.rotationKeyCount = pChannel->mNumRotationKeys;
						channel_data.scalingKeyOffset = file.mAnimationBuffer.scalingKeyData.size();
						channel_data.scalingKeyCount = pChannel->mNumScalingKeys;
						file.mAnimationBuffer.channelData.push_back(channel_data);
						for (int i = 0; i < pChannel->mNumPositionKeys; i++)
						{
							ModelFile::PositionKey key;
							key.time = pChannel->mPositionKeys[i].mTime;
							key.position.x = pChannel->mPositionKeys[i].mValue.x;
							key.position.y = pChannel->mPositionKeys[i].mValue.y;
							key.position.z = pChannel->mPositionKeys[i].mValue.z;
							file.mAnimationBuffer.positionKeyData.push_back(key);
						}
						for (int i = 0; i < pChannel->mNumRotationKeys; i++)
						{
							ModelFile::RotationKey key;
							key.time = pChannel->mRotationKeys[i].mTime;
							key.rotation.x = pChannel->mRotationKeys[i].mValue.x;
							key.rotation.y = pChannel->mRotationKeys[i].mValue.y;
							key.rotation.z = pChannel->mRotationKeys[i].mValue.z;
							key.rotation.w = pChannel->mRotationKeys[i].mValue.w;
							file.mAnimationBuffer.rotationKeyData.push_back(key);
						}
						for (int i = 0; i < pChannel->mNumScalingKeys; i++)
						{
							ModelFile::ScalingKey key;
							key.time = pChannel->mScalingKeys[i].mTime;
							key.scaling.x = pChannel->mScalingKeys[i].mValue.x;
							key.scaling.y = pChannel->mScalingKeys[i].mValue.y;
							key.scaling.z = pChannel->mScalingKeys[i].mValue.z;
							file.mAnimationBuffer.scalingKeyData.push_back(key);
						}
					}
				}

			}



			file.genHeader();
			return true;
		}




	}
}

