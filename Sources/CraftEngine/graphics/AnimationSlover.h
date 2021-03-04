#pragma once
#include "ModelFile.h"

namespace CraftEngine
{
	namespace graphics
	{

		class AnimationSlover
		{
		private:
			using vec3 = math::vec3;
			using quat = math::quat;
			using mat4 = math::mat4;
			const ModelFile* m_modelFile;
		public:
			std::vector<mat4> mChannelTransformMatrices;
			std::vector<mat4> mBoneGlobalTransformData;
			std::vector<mat4> mBoneTransformMatrices;
			void bindModel(const ModelFile* modelFile)
			{
				m_modelFile = modelFile;
			}

			void evaluate(float timeInSceonds, int animationIndex)
			{
				auto& animationInfo = m_modelFile->mAnimationBuffer.animationInfoData[animationIndex];
				float TicksPerSecond = (float)(animationInfo.ticksPerSecond > 16.7 ? animationInfo.ticksPerSecond : 25.0f);
				//TicksPerSecond = 25.0f;//

				float TimeInTicks = timeInSceonds * TicksPerSecond;
				float AnimationTime = fmod(TimeInTicks, (float)animationInfo.duration);
				mChannelTransformMatrices.resize(animationInfo.channelCount);

				const auto* pChannels = &m_modelFile->mAnimationBuffer.channelData[animationInfo.channelOffset];
				for (int c = 0; c < animationInfo.channelCount; c++)
				{
					auto& channel = pChannels[c];
					auto pPosKeys = &m_modelFile->mAnimationBuffer.positionKeyData[channel.positionKeyOffset];
					auto pRotKeys = &m_modelFile->mAnimationBuffer.rotationKeyData[channel.rotationKeyOffset];
					auto pScaKeys = &m_modelFile->mAnimationBuffer.scalingKeyData[channel.scalingKeyOffset];
					vec3 position, scaling;
					quat rotation;
					// pos

					{
						unsigned int frame = 0;
						while (frame < channel.positionKeyCount - 1){
							if (AnimationTime < pPosKeys[frame + 1].time)
								break;
							frame++;
						}
						unsigned int nextFrame = (frame + 1) % channel.positionKeyCount;
						double diffTime = pPosKeys[nextFrame].time - pPosKeys[frame].time;
						if (diffTime < 0.0)
							diffTime += animationInfo.duration;
						if (diffTime > 0){
							float factor = float((AnimationTime - pPosKeys[frame].time) / diffTime);
							position = math::lerp(pPosKeys[frame].position, pPosKeys[nextFrame].position, factor);
						}else{
							position = pPosKeys[nextFrame].position;
						}
						//std::cout << "pos frame:" << frame;
						//position = vec3(0);
					}
					// rot
					{
						unsigned int frame = 0;
						while (frame < channel.rotationKeyCount - 1) {
							if (AnimationTime < pRotKeys[frame + 1].time)
								break;
							frame++;
						}
						unsigned int nextFrame = (frame + 1) % channel.rotationKeyCount;
						double diffTime = pRotKeys[nextFrame].time - pRotKeys[frame].time;
						if (diffTime < 0.0)
							diffTime += animationInfo.duration;
						if (diffTime > 0) {
							float factor = float((AnimationTime - pRotKeys[frame].time) / diffTime);
							rotation = math::splerp(pRotKeys[frame].rotation, pRotKeys[nextFrame].rotation, factor);
						}
						else {
							rotation = pRotKeys[nextFrame].rotation;
						}		
						//std::cout << "; rot frame:" << frame << " | " << nextFrame << ", factor:" << factor << "\n";
						//rotation = quat(0, 0, 0, 1);
					}
					// sca
					{
						unsigned int frame = 0;
						while (frame < channel.scalingKeyCount - 1) {
							if (AnimationTime < pScaKeys[frame + 1].time)
								break;
							frame++;
						}
						unsigned int nextFrame = (frame + 1) % channel.scalingKeyCount;
						double diffTime = pScaKeys[nextFrame].time - pScaKeys[frame].time;
						if (diffTime < 0.0)
							diffTime += animationInfo.duration;
						if (diffTime > 0) {
							float factor = float((AnimationTime - pScaKeys[frame].time) / diffTime);
							scaling = math::lerp(pScaKeys[frame].scaling, pScaKeys[nextFrame].scaling, factor);
						}
						else {
							scaling = pScaKeys[nextFrame].scaling;
						}	
						//std::cout << "; sca frame:" << frame << "\n";
					}

					mChannelTransformMatrices[c] = mat4(1.0f);
					mChannelTransformMatrices[c] = math::scale(scaling, mChannelTransformMatrices[c]);
					mChannelTransformMatrices[c] = math::rotate(rotation, mChannelTransformMatrices[c]);
					mChannelTransformMatrices[c] = math::translate(position, mChannelTransformMatrices[c]);
					//mBoneTransformData[channel.boneIndex].localTransform = mat4(1.0f);
				}
				//// calc global transformation
				//for (int i = 0; i < m_modelFile->mBoneBuffer.boneData.size(); i++)
				//{
				//	mBoneTransformData[i].globalTransform = mat4(1.0f);
				//	int parent_index = i;
				//	while (1)
				//	{
				//		mBoneTransformData[i].globalTransform = mBoneTransformData[parent_index].localTransform * mBoneTransformData[i].globalTransform;

				//		//mBoneTransformData[i].globalTransform =
				//		//	math::inverse(m_modelFile->mBoneBuffer.boneData[parent_index].offsetMatrix) *
				//		//	mBoneTransformData[parent_index].localTransform *
				//		//	m_modelFile->mBoneBuffer.boneData[parent_index].offsetMatrix *
				//		//	mBoneTransformData[i].globalTransform;
				//	
				//		if (m_modelFile->mBoneBuffer.boneData[parent_index].parentIndex < 0)
				//			break;
				//		parent_index = m_modelFile->mBoneBuffer.boneData[parent_index].parentIndex;
				//	}

				//}




				std::vector<int> node_bone_mapping;
				node_bone_mapping.resize(m_modelFile->mNodeBuffer.nodeData.size(), -1);
				for (int i = 0; i < m_modelFile->mBoneBuffer.boneData.size(); i++)
					node_bone_mapping[m_modelFile->mBoneBuffer.boneData[i].nodeIndex] = i;
				std::vector<int> node_channel_mapping;
				node_channel_mapping.resize(m_modelFile->mNodeBuffer.nodeData.size(), -1);
				for (int i = 0; i < animationInfo.channelCount; i++)
					node_channel_mapping[pChannels[i].nodeIndex] = i;
				mBoneGlobalTransformData.resize(m_modelFile->mBoneBuffer.boneData.size());

				auto inv_root_transform = math::inverse(m_modelFile->mNodeBuffer.nodeData[0].transform);
				std::function<void(int, const mat4&)> update_node;
				update_node = [&](int node_idx, const mat4& parentTrans)
				{

					auto& node = m_modelFile->mNodeBuffer.nodeData[node_idx];
					mat4 globalTransform = node.transform;
					auto bone_idx = node_bone_mapping[node_idx];
					auto channel_idx = node_channel_mapping[node_idx];
					//std::cout << "process bone_idx:" << bone_idx << ", channel_idx:" << channel_idx << ", name:" << &m_modelFile->mStringBuffer[m_modelFile->mNodeBuffer.nodeData[node_idx].name.charOffset] << std::endl;
					//std::cout << "process node:" << node_idx << "bone:" << bone_idx << std::endl;
					if (channel_idx >= 0)
					{
						globalTransform = mChannelTransformMatrices[channel_idx];
					}
					else
					{
						if (bone_idx >= 0)
						{
							globalTransform = mat4(1.0f);
						}
						else
						{

						}
						//globalTransform = mat4(1.0f);
					}
					globalTransform = parentTrans * globalTransform;
					if (bone_idx >= 0)
					{
						mBoneGlobalTransformData[bone_idx] =
							inv_root_transform *
							globalTransform *
							m_modelFile->mBoneBuffer.boneData[bone_idx].offsetMatrix;
					}
					for (int i = 0; i < node.childIndexCount; i++)
					{
						update_node(m_modelFile->mNodeBuffer.nodeChildIndexData[i + node.childIndexOffset], globalTransform);
					}
				};
				update_node(0, mat4(1.0f));

				mBoneTransformMatrices.resize(m_modelFile->mBoneBuffer.boneData.size());
				for (int i = 0; i < m_modelFile->mBoneBuffer.boneData.size(); i++)
				{
					mBoneTransformMatrices[i] = mBoneGlobalTransformData[i];
				}

				//for (int i = 0; i < m_modelFile->mBoneBuffer.boneData.size(); i++)
				//{
				//	mBoneTransformMatrices[i] = mat4(1.0f);
				//}
				//for (int i = 0; i < m_modelFile->mBoneBuffer.boneData.size(); i++)
				//{					
				//	int parent_index = i;
				//	while (1)
				//	{
				//		if (m_modelFile->mBoneBuffer.boneData[parent_index].parentIndex < 0)
				//			break;
				//		parent_index = m_modelFile->mBoneBuffer.boneData[parent_index].parentIndex;
				//	}
				//	mBoneTransformData[i].globalTransform = math::inverse(m_modelFile->mBoneBuffer.boneData[parent_index].offsetMatrix) * mBoneTransformData[i].globalTransform;
				//}
				//mBoneTransformMatrices.resize(m_modelFile->mBoneBuffer.boneData.size());
				//for (int i = 0; i < m_modelFile->mBoneBuffer.boneData.size(); i++)
				//{
				//	mBoneTransformMatrices[i] = mBoneTransformData[i].globalTransform * m_modelFile->mBoneBuffer.boneData[i].offsetMatrix;
				//}





	
			}
		private:


		};



	}
}


