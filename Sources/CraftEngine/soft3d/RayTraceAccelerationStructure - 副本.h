#pragma once
#include "./Memory.h"
#include "./Buffer.h"
namespace CraftEngine
{
	namespace soft3d
	{


		struct RayTraceAABB
		{
			vec3 mMin;
			vec3 mMax;
		};



		namespace detail
		{
			struct RayTraceBottomLevelAccelerationStructureData
			{
				Buffer mVertexBuffer;
				size_t mVertexCount;
				size_t mVertexStride;
				size_t mVertexOffset;
				Buffer mIndexBuffer;
				size_t mIndexCount;
				IndexType mIndexType;
				void* mBVHData;
			};



			struct RayTraceBVHTriangle
			{
				vec3 position[3];
			};

			struct RayTraceBVHNode
			{
				enum RayTraceBVHSplitAxis: int
				{
					eNoAxis = -1,
					eXAxis = 0,
					eYAxis = 1,
					eZAxis = 2,
				};

				RayTraceAABB mAABB;
				RayTraceBVHNode* mChildren[2];
				//RayTraceBVHTriangle mPrimitive;
				uint32_t mPrimitiveIndex;
				RayTraceBVHSplitAxis mSplitAxis;
			};

			struct RayTraceBVHNodeData
			{
				RayTraceAABB mAABB;
				union
				{
					int mPrimitiveIndex;  // leaf
					int mSecondChildOffset;  // interior
				};
				int mIsLeaf;
			};



			RayTraceAABB rayTraceAABBUnion(const RayTraceAABB& a, const RayTraceAABB& b)
			{
				RayTraceAABB aabb;
				aabb.mMin = math::min(a.mMin, b.mMin);
				aabb.mMax = math::max(a.mMax, b.mMax);
				return aabb;
			}

			float rayTraceAABBVolume(const RayTraceAABB& a)
			{
				auto aabb_size = a.mMax - a.mMin;
				return aabb_size.x * aabb_size.y * aabb_size.z;
			}


			struct RayTraceConstructBVHNodeResources
			{
				RayTraceBVHNode* nodeBuffer;
				uint32_t curNodeIndex;
				uint32_t* primitiveOrder;
				const RayTraceAABB* primitiveAABB;
			};

			
			template<size_t Axis>
			void rayTraceConstructBVHNode_BuildHeap(int idx, int length, int offset, RayTraceConstructBVHNodeResources* resources)
			{
				int idx_parent, idx_child_left, idx_child_right;
				float val_parent, val_child_left, val_child_right;
				int end_index = length + offset;
				int k;
				float t;
				for (int i = idx; i < length / 2;)
				{
					idx_parent = offset + i;
					idx_child_left = offset + 2 * i + 1;
					idx_child_right = offset + 2 * i + 2;

					val_parent = (resources->primitiveAABB[resources->primitiveOrder[idx_parent]].mMax[Axis] +
						resources->primitiveAABB[resources->primitiveOrder[idx_parent]].mMin[Axis]) * 0.5f;
					val_child_left = (resources->primitiveAABB[resources->primitiveOrder[idx_child_left]].mMax[Axis] +
						resources->primitiveAABB[resources->primitiveOrder[idx_child_left]].mMin[Axis]) * 0.5f;
					k = idx_child_left;
					t = val_child_left;
					if (idx_child_right < end_index)
					{
						val_child_right = (resources->primitiveAABB[resources->primitiveOrder[idx_child_right]].mMax[Axis] +
							resources->primitiveAABB[resources->primitiveOrder[idx_child_right]].mMin[Axis]) * 0.5f;
						if (val_child_right > val_child_left)
						{
							k = idx_child_right;
							t = val_child_right;
						}
					}
					if (t > val_parent)
					{
						//std::cout << "swap<" << idx_parent << ", " << k << ">\n";
						std::swap(resources->primitiveOrder[idx_parent], resources->primitiveOrder[k]);
						i = k - offset;
					}
					else
					{
						break;
					}
				}

			}


			void rayTraceConstructBVHNode(
				RayTraceBVHNode* parentNode,
				int beginIndex,
				int endIndex,
				RayTraceConstructBVHNodeResources* resources
			)
			{
				int primitive_count = endIndex - beginIndex;
				if (primitive_count == 1)
				{
					parentNode->mSplitAxis = RayTraceBVHNode::eNoAxis;
					parentNode->mAABB = resources->primitiveAABB[resources->primitiveOrder[beginIndex]];
					parentNode->mPrimitiveIndex = resources->primitiveOrder[beginIndex];
					parentNode->mChildren[0] = nullptr;
					parentNode->mChildren[1] = nullptr;
				}
				else
				{
					int i = beginIndex;
					RayTraceAABB aabb = resources->primitiveAABB[resources->primitiveOrder[i]];
					for (i = beginIndex + 1; i < endIndex; i++)
						aabb = rayTraceAABBUnion(aabb, resources->primitiveAABB[resources->primitiveOrder[i]]);
					parentNode->mAABB = aabb;

					float temp_axis_length = aabb.mMax.x - aabb.mMin.x;
					float max_axis_length = temp_axis_length;
					RayTraceBVHNode::RayTraceBVHSplitAxis axis = RayTraceBVHNode::eXAxis;
					temp_axis_length = aabb.mMax.y - aabb.mMin.y;
					if (temp_axis_length > max_axis_length)
					{
						axis = RayTraceBVHNode::eYAxis;
						max_axis_length = temp_axis_length;
					}
					temp_axis_length = aabb.mMax.z - aabb.mMin.z;
					if (temp_axis_length > max_axis_length)
					{
						axis = RayTraceBVHNode::eZAxis;
						max_axis_length = temp_axis_length;
					}
					parentNode->mSplitAxis = axis;
					parentNode->mPrimitiveIndex = -1;


					//std::cout << "sort from " << beginIndex << " to " << endIndex << "\n";
					////
					//std::cout << "before order:";
					//for (int i = beginIndex; i < endIndex; i++)
					//{
					//	std::cout << resources->primitiveOrder[i] << ", ";
					//}
					//std::cout << "\n";
					//std::cout << "before value:";
					//for (int i = beginIndex; i < endIndex; i++)
					//{
					//	float value = (resources->primitiveAABB[resources->primitiveOrder[i]].mMax[axis] +
					//		resources->primitiveAABB[resources->primitiveOrder[i]].mMin[axis]) * 0.5f;
					//	std::cout << value << ", ";
					//}
					//std::cout << "\n";


					// heap sort
					switch (axis)
					{
					case RayTraceBVHNode::eXAxis:
					{
						//std::cout << "spilt by x-axis\n";
						int last_inner_node_index = primitive_count / 2 - 1;
						for (int i = last_inner_node_index; i >= 0; i--)
							rayTraceConstructBVHNode_BuildHeap<0>(i, primitive_count, beginIndex, resources);
						int cur_range = primitive_count - 1;
						for (int i = primitive_count - 1; i > 0; i--, cur_range--)
						{
							std::swap(resources->primitiveOrder[beginIndex], resources->primitiveOrder[i + beginIndex]);
							rayTraceConstructBVHNode_BuildHeap<0>(0, cur_range, beginIndex, resources);
						}
					}
						break;
					case RayTraceBVHNode::eYAxis:
					{						
						//std::cout << "spilt by y-axis\n";
						int last_inner_node_index = primitive_count / 2 - 1;
						for (int i = last_inner_node_index; i >= 0; i--)
							rayTraceConstructBVHNode_BuildHeap<1>(i, primitive_count, beginIndex, resources);
						int cur_range = primitive_count;
						for (int i = primitive_count - 1; i > 0; i--, cur_range--)
						{
							std::swap(resources->primitiveOrder[beginIndex], resources->primitiveOrder[i + beginIndex]);
							rayTraceConstructBVHNode_BuildHeap<1>(0, cur_range, beginIndex, resources);
						}
					}
						break;
					case RayTraceBVHNode::eZAxis:
					{
						//std::cout << "spilt by z-axis\n";
						int last_inner_node_index = primitive_count / 2 - 1;
						for (int i = last_inner_node_index; i >= 0; i--)
							rayTraceConstructBVHNode_BuildHeap<2>(i, primitive_count, beginIndex, resources);
						int cur_range = primitive_count;
						for (int i = primitive_count - 1; i > 0; i--, cur_range--)
						{
							std::swap(resources->primitiveOrder[beginIndex], resources->primitiveOrder[i + beginIndex]);
							rayTraceConstructBVHNode_BuildHeap<2>(0, cur_range, beginIndex, resources);
						}
					}
						break;
					}

					//
					//std::cout << "after order:";
					//for (int i = beginIndex; i < endIndex; i++)
					//{
					//	std::cout << resources->primitiveOrder[i] << ", ";
					//}
					//std::cout << "\n";
					//std::cout << "after value:";
					//for (int i = beginIndex; i < endIndex; i++)
					//{
					//	float value = (resources->primitiveAABB[resources->primitiveOrder[i]].mMax[axis] +
					//		resources->primitiveAABB[resources->primitiveOrder[i]].mMin[axis]) * 0.5f;
					//	std::cout << value << ", ";
					//}
					//std::cout << "\n";
					//std::cout << "\n";

					int mid_index = primitive_count / 2;
					parentNode->mChildren[0] = &resources->nodeBuffer[resources->curNodeIndex++];
					rayTraceConstructBVHNode(parentNode->mChildren[0], beginIndex, beginIndex + mid_index, resources);
					parentNode->mChildren[1] = &resources->nodeBuffer[resources->curNodeIndex++];
					rayTraceConstructBVHNode(parentNode->mChildren[1], beginIndex + mid_index, beginIndex + primitive_count, resources);
				}
			}

			int rayTraceConstructBVHDumpNodeData(
				const RayTraceBVHNode* node,
				RayTraceBVHNodeData* ptr,
				int nodeIndex
			)
			{
				auto pDstNode = ptr + nodeIndex;
				pDstNode->mAABB = node->mAABB;
				if (node->mSplitAxis == RayTraceBVHNode::eNoAxis)
				{
					pDstNode->mIsLeaf = 1;
					pDstNode->mPrimitiveIndex = node->mPrimitiveIndex;
					return nodeIndex;
				}
				else
				{
					pDstNode->mIsLeaf = 0;
					int index = rayTraceConstructBVHDumpNodeData(node->mChildren[0], ptr, nodeIndex + 1);
					pDstNode->mSecondChildOffset = index + 1;
					index = rayTraceConstructBVHDumpNodeData(node->mChildren[1], ptr, pDstNode->mSecondChildOffset);
					return index;
				}
			}


			void rayTraceConstructBottomLevelBVH(RayTraceBottomLevelAccelerationStructureData* acStructureData, const MemAllocator& allocator = MemAllocator())
			{
				int primitive_count = acStructureData->mIndexCount / 3;

				// buffer for bvh node
				const int max_node_count = primitive_count * 2;
				RayTraceBVHNode* temp_node_buffer = new RayTraceBVHNode[max_node_count];

				// buffer for primitives order
				uint32_t* primitive_order = new uint32_t[primitive_count];

				// initialize order as sequence: 0,1,2,...,n-1
				for (int i = 0; i < primitive_count; i++)
					primitive_order[i] = i;

				// buffer for triangle's aabb
				RayTraceAABB* primitive_aabb = new RayTraceAABB[primitive_count];

				// calcular every triangle's aabb
				const uint8_t const* vertex_buffer = (const uint8_t*)acStructureData->mVertexBuffer.data();
				const uint8_t const* index_buffer = (const uint8_t*)acStructureData->mIndexBuffer.data();
				uint8_t const* cur_index_ptr = index_buffer;
				const uint32_t vertex_stride = acStructureData->mVertexStride;
				const uint32_t vertex_offset = acStructureData->mVertexOffset;
				for (int i = 0; i < primitive_count; i++)
				{
					uint32_t indices[3];
					switch (acStructureData->mIndexType)
					{
					case IndexType::eUInt32:
						indices[0] = *(uint32_t*)(cur_index_ptr + sizeof(uint32_t) * 0);
						indices[1] = *(uint32_t*)(cur_index_ptr + sizeof(uint32_t) * 1);
						indices[2] = *(uint32_t*)(cur_index_ptr + sizeof(uint32_t) * 2);
						cur_index_ptr += 3 * sizeof(uint32_t);
						break;
					case IndexType::eUInt16:
						indices[0] = *(uint16_t*)(cur_index_ptr + sizeof(uint16_t) * 0);
						indices[1] = *(uint16_t*)(cur_index_ptr + sizeof(uint16_t) * 1);
						indices[2] = *(uint16_t*)(cur_index_ptr + sizeof(uint16_t) * 2);
						cur_index_ptr += 3 * sizeof(uint16_t);
						break;
					case IndexType::eUInt8:
						indices[0] = *(uint8_t*)(cur_index_ptr + sizeof(uint8_t) * 0);
						indices[1] = *(uint8_t*)(cur_index_ptr + sizeof(uint8_t) * 1);
						indices[2] = *(uint8_t*)(cur_index_ptr + sizeof(uint8_t) * 2);
						cur_index_ptr += 3 * sizeof(uint8_t);
						break;
					}
					vec3 v0 = *(vec3*)(vertex_buffer + vertex_stride * indices[0] + vertex_offset);
					vec3 v1 = *(vec3*)(vertex_buffer + vertex_stride * indices[1] + vertex_offset);
					vec3 v2 = *(vec3*)(vertex_buffer + vertex_stride * indices[2] + vertex_offset);
					primitive_aabb[i].mMin = math::min(v0, v1, v2);
					primitive_aabb[i].mMax = math::max(v0, v1, v2);
				}

				// prepare resource
				RayTraceConstructBVHNodeResources resources;
				resources.nodeBuffer = temp_node_buffer;
				resources.curNodeIndex = 0;
				resources.primitiveOrder = primitive_order;
				resources.primitiveAABB = primitive_aabb;

				// constuct bvh
				RayTraceBVHNode* root_node = &resources.nodeBuffer[resources.curNodeIndex++];
				rayTraceConstructBVHNode(root_node, 0, primitive_count, &resources);

				// buffer for bvh data
				RayTraceBVHNodeData* bvh_data_buffer = new RayTraceBVHNodeData[max_node_count];

				// dump bvh data to buffer
				rayTraceConstructBVHDumpNodeData(root_node, bvh_data_buffer, 0);
				acStructureData->mBVHData = bvh_data_buffer;


				allocator.free(temp_node_buffer);
				allocator.free(primitive_order);
				allocator.free(primitive_aabb);
			}


		}


		class RayTraceBottomLevelAccelerationStructure
		{
		private:
			detail::RayTraceBottomLevelAccelerationStructureData* m_rayTraceAccelerationStructureData;
		public:
			RayTraceBottomLevelAccelerationStructure(void* handle) :m_rayTraceAccelerationStructureData((detail::RayTraceBottomLevelAccelerationStructureData*)handle) { }
			RayTraceBottomLevelAccelerationStructure() :m_rayTraceAccelerationStructureData(nullptr) { }
		public:
			void* handle() const { return m_rayTraceAccelerationStructureData; }
			bool  valid() const { return handle() != nullptr; }
		};


		namespace detail
		{
			struct RayTraceRayHitBottomLevelAccelerationStructureResult
			{
				float t;
				uint32_t primitiveIndex;
				vec3 attribs;
			};

			struct RayTraceRayHitBottomLevelAccelerationStructureResource
			{
				const detail::RayTraceBottomLevelAccelerationStructureData* acStructureData;
				vec3 origin;
				vec3 direction;
				float tmin; 
				float tmax;
				float t;
				uint32_t primitiveIndex;
				vec3 attribs;
			};

			float rayTraceRayHitBottomLevelAccelerationStructure_RecursiveSearch(const detail::RayTraceBVHNodeData* root, uint32_t nodeIndex, RayTraceRayHitBottomLevelAccelerationStructureResource* resources)
			{
				//std::cout << "search:" << nodeIndex << std::endl;
				auto node = &root[nodeIndex];
				RayTraceAABB aabb = node->mAABB;
				constexpr float epsilon = std::numeric_limits<float>::epsilon() * 15;
				float t0, t1;
				float tNear = -std::numeric_limits<float>::max();
				float tFar = std::numeric_limits<float>::max();
				//for (int i = 0; i < 3; i++)
				//{
				//	if (math::abs(resources->direction[i]) < epsilon &&
				//		(resources->origin[i] < aabb.mMin[i] || resources->origin[i] > aabb.mMax[i]))
				//		return -1.0f;

				//	t0 = (aabb.mMin[i] - resources->origin[i]) / resources->direction[i];
				//	t1 = (aabb.mMax[i] - resources->origin[i]) / resources->direction[i];
				//	if (t0 > t1)
				//		std::swap(t0, t1);
				//	if (t0 > tNear)
				//		tNear = t0;
				//	if (t1 < tFar)
				//		tFar = t1;
				//	if (tNear > tFar || tFar < 0)
				//		return -1.0f;
				//}
				//if (tNear >= resources->t)
				//	return -1.0f;

				if (node->mIsLeaf)
				{
					const uint32_t primitive_index = node->mPrimitiveIndex;
					const uint8_t const* vertex_buffer = (const uint8_t*)resources->acStructureData->mVertexBuffer.data();
					const uint8_t const* index_buffer = (const uint8_t*)resources->acStructureData->mIndexBuffer.data();
					uint8_t const* cur_index_ptr = index_buffer;
					const uint32_t vertex_stride = resources->acStructureData->mVertexStride;
					const uint32_t vertex_offset = resources->acStructureData->mVertexOffset;
					uint32_t indices[3];
					switch (resources->acStructureData->mIndexType)
					{
					case IndexType::eUInt32:
						cur_index_ptr += primitive_index * (sizeof(uint32_t) * 3);
						indices[0] = *(uint32_t*)(cur_index_ptr + sizeof(uint32_t) * 0);
						indices[1] = *(uint32_t*)(cur_index_ptr + sizeof(uint32_t) * 1);
						indices[2] = *(uint32_t*)(cur_index_ptr + sizeof(uint32_t) * 2);
						break;
					case IndexType::eUInt16:
						cur_index_ptr += primitive_index * (sizeof(uint16_t) * 3);
						indices[0] = *(uint16_t*)(cur_index_ptr + sizeof(uint16_t) * 0);
						indices[1] = *(uint16_t*)(cur_index_ptr + sizeof(uint16_t) * 1);
						indices[2] = *(uint16_t*)(cur_index_ptr + sizeof(uint16_t) * 2);
						break;
					case IndexType::eUInt8:
						cur_index_ptr += primitive_index * (sizeof(uint8_t) * 3);
						indices[0] = *(uint8_t*)(cur_index_ptr + sizeof(uint8_t) * 0);
						indices[1] = *(uint8_t*)(cur_index_ptr + sizeof(uint8_t) * 1);
						indices[2] = *(uint8_t*)(cur_index_ptr + sizeof(uint8_t) * 2);
						break;
					}
					vec3 v0 = *(vec3*)(vertex_buffer + vertex_stride * indices[0] + vertex_offset);
					vec3 v1 = *(vec3*)(vertex_buffer + vertex_stride * indices[1] + vertex_offset);
					vec3 v2 = *(vec3*)(vertex_buffer + vertex_stride * indices[2] + vertex_offset);

					vec3 e1, e2, p, s, q;
					float t, u, v, tmp;
					e1 = v1 - v0;
					e2 = v2 - v0;
					p = math::cross(resources->direction, e2);
					tmp = math::dot(p, e1);
					if (math::abs(tmp) < epsilon)
						return -1.0f;
					tmp = math::inverse(tmp);
					s = resources->origin - v0;
					u = tmp * math::dot(s, p);
					if (u < 0.0f || u > 1.0f)
						return -1.0f;
					q = math::cross(s, e1);
					v = tmp * math::dot(resources->direction, q);
					if (v < 0.0f || v > 1.0f)
						return -1.0f;
					t = tmp * math::dot(e2, q);

					if (t >= resources->t || t < resources->tmin)
						return -1.0f;

					resources->t = t;
					resources->primitiveIndex = primitive_index;
					resources->attribs = vec3(1.0f - u - v, u, v);
					return t;
				}
				else
				{
					float t_left = rayTraceRayHitBottomLevelAccelerationStructure_RecursiveSearch(root, nodeIndex + 1, resources);
					float t_right = rayTraceRayHitBottomLevelAccelerationStructure_RecursiveSearch(root, node->mSecondChildOffset, resources);

					if (t_right >= 0.0f)
					{
						if (t_left >= 0.0f && t_left < t_right)
							return t_left;
						return t_right;
					}
					return t_left;
				}
			}


			RayTraceRayHitBottomLevelAccelerationStructureResult rayTraceRayHitBottomLevelAccelerationStructure(const RayTraceBottomLevelAccelerationStructure& acStructure, vec3 origin, vec3 direction, float tmin, float tmax)
			{
				const auto acs_data = (detail::RayTraceBottomLevelAccelerationStructureData*)acStructure.handle();
				const auto bvh_data = (const detail::RayTraceBVHNodeData*)acs_data->mBVHData;
				const auto bvh_root = bvh_data;
				RayTraceRayHitBottomLevelAccelerationStructureResource resources;
				resources.acStructureData = acs_data;
				resources.origin = origin;
				resources.direction = direction;
				resources.tmin = tmin;
				resources.tmax = tmax;
				resources.t = tmax;
				resources.primitiveIndex = 0xFFFFFFFF;
				RayTraceRayHitBottomLevelAccelerationStructureResult result;
				result.t = rayTraceRayHitBottomLevelAccelerationStructure_RecursiveSearch(bvh_root, 0, &resources);
				result.attribs = resources.attribs;
				result.primitiveIndex = resources.primitiveIndex;
				return result;
			}

		}



		RayTraceBottomLevelAccelerationStructure createRayTraceBottomLevelAccelerationStructure(
			const Buffer& vertexBuffer,
			size_t        vertexCount,
			size_t        vertexStride,
			size_t        vertexOffset,
			const Buffer& indexBuffer,
			size_t        indexCount,
			IndexType     indexType,
			const MemAllocator& allocator = MemAllocator()
		)
		{
			auto acs_data = (detail::RayTraceBottomLevelAccelerationStructureData*)allocator.alloc(sizeof(detail::RayTraceBottomLevelAccelerationStructureData));
			RayTraceBottomLevelAccelerationStructure ac_structure(acs_data);
			acs_data->mVertexBuffer = vertexBuffer;
			acs_data->mVertexCount = vertexCount;
			acs_data->mVertexStride = vertexStride;
			acs_data->mVertexOffset = vertexOffset;
			acs_data->mIndexBuffer = indexBuffer;
			acs_data->mIndexCount = indexCount;
			acs_data->mIndexType = indexType;
			detail::rayTraceConstructBottomLevelBVH(acs_data, allocator);
			return ac_structure;
		}

		void destroyRayTraceBottomLevelAccelerationStructure(const RayTraceBottomLevelAccelerationStructure& acStructure, const MemAllocator& allocator = MemAllocator())
		{
			auto acs_data = (detail::RayTraceBottomLevelAccelerationStructureData*)acStructure.handle();
			allocator.free(acs_data->mBVHData);
			allocator.free(acs_data);
		}




		struct RayTraceAccelerationStructureInstanceCreateInfo
		{
			using mat4 = math::mat4;
			mat4 mTransform;
			RayTraceBottomLevelAccelerationStructure mBLAS;
			uint32_t mShaderIndex;
			uint32_t mMask;
		};


		namespace detail
		{
			struct RayTraceBottomLevelAccelerationStructureInstanceData
			{
				using mat4 = math::mat4;
				using mat3 = math::mat3;
				mat4 mTransform;
				mat4 mInverseTransform;
				mat3 mDirectionTransform;
				RayTraceAABB mAABB;
				RayTraceBottomLevelAccelerationStructure mBLAS;
				uint32_t mShaderIndex;
				uint32_t mMask;
			};

			struct RayTraceTopLevelAccelerationStructureData
			{
				RayTraceBottomLevelAccelerationStructureInstanceData* mInstanceList;
				uint32_t mInstanceCount;
				void* mBVHData;
			};


			void rayTraceConstructTopLevelBVH(RayTraceTopLevelAccelerationStructureData* acStructureData, const MemAllocator& allocator = MemAllocator())
			{
				int primitive_count = acStructureData->mInstanceCount;

				// buffer for bvh node
				const int max_node_count = primitive_count * 2;
				RayTraceBVHNode* temp_node_buffer = new RayTraceBVHNode[max_node_count];

				// buffer for primitives order
				uint32_t* primitive_order = new uint32_t[primitive_count];

				// initialize order as sequence: 0,1,2,...,n-1
				for (int i = 0; i < primitive_count; i++)
					primitive_order[i] = i;

				// buffer for triangle's aabb
				RayTraceAABB* primitive_aabb = new RayTraceAABB[primitive_count];

				// copy every instance's aabb
				for (int i = 0; i < primitive_count; i++)
					primitive_aabb[i] = acStructureData->mInstanceList[i].mAABB;

				// prepare resource
				RayTraceConstructBVHNodeResources resources;
				resources.nodeBuffer = temp_node_buffer;
				resources.curNodeIndex = 0;
				resources.primitiveOrder = primitive_order;
				resources.primitiveAABB = primitive_aabb;

				// constuct bvh
				RayTraceBVHNode* root_node = &resources.nodeBuffer[resources.curNodeIndex++];
				rayTraceConstructBVHNode(root_node, 0, primitive_count, &resources);

				// buffer for bvh data
				RayTraceBVHNodeData* bvh_data_buffer = new RayTraceBVHNodeData[max_node_count];

				// dump bvh data to buffer
				rayTraceConstructBVHDumpNodeData(root_node, bvh_data_buffer, 0);
				acStructureData->mBVHData = bvh_data_buffer;

				allocator.free(temp_node_buffer);
				allocator.free(primitive_order);
				allocator.free(primitive_aabb);
			}

		}


		class RayTraceTopLevelAccelerationStructure
		{
		private:
			detail::RayTraceTopLevelAccelerationStructureData* m_rayTraceAccelerationStructureData;
		public:
			RayTraceTopLevelAccelerationStructure(void* handle) :m_rayTraceAccelerationStructureData((detail::RayTraceTopLevelAccelerationStructureData*)handle) { }
			RayTraceTopLevelAccelerationStructure() :m_rayTraceAccelerationStructureData(nullptr) { }
		public:
			void* handle() const { return m_rayTraceAccelerationStructureData; }
			bool  valid() const { return handle() != nullptr; }
		};





		RayTraceTopLevelAccelerationStructure createRayTraceTopLevelAccelerationStructure(
			RayTraceAccelerationStructureInstanceCreateInfo* instances,
			uint32_t instanceCount,
			const MemAllocator& allocator = MemAllocator()
		)
		{
			auto acs_data = (detail::RayTraceTopLevelAccelerationStructureData*)allocator.alloc(sizeof(detail::RayTraceTopLevelAccelerationStructureData));

			vec3 vertices[8];
			acs_data->mInstanceCount = instanceCount;
			acs_data->mInstanceList = (detail::RayTraceBottomLevelAccelerationStructureInstanceData*)allocator.alloc(
				instanceCount * sizeof(detail::RayTraceBottomLevelAccelerationStructureInstanceData));
			for (int i = 0; i < instanceCount; i++)
			{
				acs_data->mInstanceList[i].mTransform = instances[i].mTransform;
				acs_data->mInstanceList[i].mInverseTransform = (math::inverse(instances[i].mTransform));
				acs_data->mInstanceList[i].mDirectionTransform = math::transpose(math::inverse(math::mat3(instances[i].mTransform)));
				acs_data->mInstanceList[i].mShaderIndex = instances[i].mShaderIndex;
				acs_data->mInstanceList[i].mMask = instances[i].mMask;
				acs_data->mInstanceList[i].mBLAS = instances[i].mBLAS;

				auto transform = acs_data->mInstanceList[i].mTransform;

				auto btm_lv_as = (detail::RayTraceBottomLevelAccelerationStructureData*)instances[i].mBLAS.handle();

				auto aabb = ((detail::RayTraceBVHNodeData*)btm_lv_as->mBVHData)->mAABB;
				vertices[0] = aabb.mMin;
				vertices[1] = vec3(aabb.mMin.x, aabb.mMin.y, aabb.mMax.z);
				vertices[2] = vec3(aabb.mMin.x, aabb.mMax.y, aabb.mMin.z);
				vertices[3] = vec3(aabb.mMax.x, aabb.mMin.y, aabb.mMin.z);
				vertices[4] = aabb.mMax;
				vertices[5] = vec3(aabb.mMax.x, aabb.mMax.y, aabb.mMin.z);
				vertices[6] = vec3(aabb.mMax.x, aabb.mMin.y, aabb.mMax.z);
				vertices[7] = vec3(aabb.mMin.x, aabb.mMax.y, aabb.mMax.z);
				for (int j = 0; j < 8; j++)
					vertices[j] = vec3(transform * vec4(vertices[j], 1.0f));

				acs_data->mInstanceList[i].mAABB.mMin = vertices[0];
				acs_data->mInstanceList[i].mAABB.mMax = vertices[0];
				for (int j = 1; j < 8; j++)
				{
					acs_data->mInstanceList[i].mAABB.mMin = math::min(acs_data->mInstanceList[i].mAABB.mMin, vertices[j]);
					acs_data->mInstanceList[i].mAABB.mMax = math::max(acs_data->mInstanceList[i].mAABB.mMax, vertices[j]);
				}
			}
			detail::rayTraceConstructTopLevelBVH(acs_data, allocator);

			RayTraceTopLevelAccelerationStructure ac_structure(acs_data);
			return ac_structure;
		}

		void destroyRayTraceTopLevelAccelerationStructure(const RayTraceTopLevelAccelerationStructure& acStructure, const MemAllocator& allocator = MemAllocator())
		{
			auto acs_data = (detail::RayTraceTopLevelAccelerationStructureData*)acStructure.handle();
			allocator.free(acs_data->mBVHData);
			allocator.free(acs_data->mInstanceList);
			allocator.free(acs_data);
		}


		struct RayTraceResult
		{
			using mat4 = math::mat4;
			mat4 transform;
			vec3 attribs;
			float t;
			uint32_t shaderIndex;
			uint32_t mask;
			const void* vertices[3];
		};


		namespace detail
		{
			//struct RayTraceRayHitTopLevelAccelerationStructureResult
			//{
			//	float t;
			//	uint32_t primitiveIndex;
			//	uint32_t instanceIndex;
			//	vec3 attribs;
			//};

			struct RayTraceRayHitTopLevelAccelerationStructureResource
			{
				const detail::RayTraceTopLevelAccelerationStructureData* acStructureData;
				vec3 origin;
				vec3 direction;
				float tmin;
				float tmax;

				float t;
				uint32_t primitiveIndex;
				uint32_t blasIndex;
				vec3 attribs;
				int32_t* valueStack;
			};

			float rayTraceRayHitTopLevelAccelerationStructure_RecursiveSearch(const detail::RayTraceBVHNodeData* root, uint32_t nodeIndex, RayTraceRayHitTopLevelAccelerationStructureResource* resources)
			{
				//std::cout << "search:" << nodeIndex << std::endl;
				auto node = &root[nodeIndex];
				RayTraceAABB aabb = node->mAABB;
				constexpr float epsilon = std::numeric_limits<float>::epsilon() * 15;
				float t0, t1;
				float tNear = -std::numeric_limits<float>::max();
				float tFar = std::numeric_limits<float>::max();
				for (int i = 0; i < 3; i++)
				{
					if (math::abs(resources->direction[i]) < epsilon &&
						(resources->origin[i] < aabb.mMin[i] || resources->origin[i] > aabb.mMax[i]))
						return -1.0f;

					t0 = (aabb.mMin[i] - resources->origin[i]) / resources->direction[i];
					t1 = (aabb.mMax[i] - resources->origin[i]) / resources->direction[i];
					if (t0 > t1)
						std::swap(t0, t1);
					if (t0 > tNear)
						tNear = t0;
					if (t1 < tFar)
						tFar = t1;
					if (tNear > tFar || tFar < 0)
						return -1.0f;
				}
				if (tNear >= resources->t)
					return -1.0f;

				if (node->mIsLeaf)
				{	
					const RayTraceBottomLevelAccelerationStructureInstanceData* instance = &resources->acStructureData->mInstanceList[node->mPrimitiveIndex];

					vec3 origin = vec3(instance->mInverseTransform * vec4(resources->origin, 1.0f));
					vec3 direction = vec3(instance->mInverseTransform * vec4(resources->direction, 0.0f));
					auto result = detail::rayTraceRayHitBottomLevelAccelerationStructure(instance->mBLAS, origin, direction, resources->tmin, resources->t);
					if (result.t >= 0.0f)
					{
						resources->t = result.t;
						resources->attribs = result.attribs;
						resources->blasIndex = node->mPrimitiveIndex;
						resources->primitiveIndex = result.primitiveIndex;
						return result.t;
					}
					else
					{
						return -1.0f;
					}
				}
				else
				{
					float t_left = rayTraceRayHitTopLevelAccelerationStructure_RecursiveSearch(root, nodeIndex + 1, resources);
					float t_right = rayTraceRayHitTopLevelAccelerationStructure_RecursiveSearch(root, node->mSecondChildOffset, resources);

					if (t_right >= 0.0f)
					{
						if (t_left >= 0.0f && t_left < t_right)
							return t_left;
						return t_right;
					}
					return t_left;
				}
			}




			float rayTraceRayHitTopLevelAccelerationStructure_LoopSearch(const detail::RayTraceBVHNodeData* root, uint32_t nodeIndex, RayTraceRayHitTopLevelAccelerationStructureResource* resources)
			{
				//std::cout << "search:" << nodeIndex << std::endl;
		
				constexpr float epsilon = std::numeric_limits<float>::epsilon() * 15;
				float t0, t1;
				float tNear;
				float tFar;
				tNear = -std::numeric_limits<float>::max();
				tFar = std::numeric_limits<float>::max();
				int32_t* stack = resources->valueStack;
				int32_t stack_depth = -1;
				stack_depth++;
				stack[stack_depth] = 1;
				int32_t cur_node_index;
				const detail::RayTraceBVHNodeData* node = &root[nodeIndex];
				RayTraceAABB aabb = node->mAABB;
				//while (1)
				//{
				//	if (stack_depth < 0)
				//		break;


				//	if (stack[stack_depth] < 0)
				//		cur_node_index = -stack[stack_depth] - 1;
				//	else
				//		cur_node_index = stack[stack_depth] - 1;
				//	node = &root[cur_node_index];


				//	if (stack[stack_depth] > 0)
				//	{
				//		aabb = node->mAABB;
				//		tNear = -std::numeric_limits<float>::max();
				//		tFar = std::numeric_limits<float>::max();
				//		for (int i = 0; i < 3; i++)
				//		{
				//			if (math::abs(resources->direction[i]) < epsilon &&
				//				(resources->origin[i] < aabb.mMin[i] || resources->origin[i] > aabb.mMax[i]))
				//				return -1.0f;

				//			t0 = (aabb.mMin[i] - resources->origin[i]) / resources->direction[i];
				//			t1 = (aabb.mMax[i] - resources->origin[i]) / resources->direction[i];
				//			if (t0 > t1)
				//				std::swap(t0, t1);
				//			if (t0 > tNear)
				//				tNear = t0;
				//			if (t1 < tFar)
				//				tFar = t1;
				//			if (tNear > tFar || tFar < 0)
				//			{
				//				stack_depth--;
				//				continue;
				//			}
				//		}
				//		if (tNear >= resources->t)
				//		{
				//			stack_depth--;
				//			continue;
				//		}

				//		stack[stack_depth] = -stack[stack_depth];


				//		if (node->mIsLeaf)
				//		{
				//			const RayTraceBottomLevelAccelerationStructureInstanceData* instance = &resources->acStructureData->mInstanceList[node->mPrimitiveIndex];

				//			vec3 origin = vec3(instance->mInverseTransform * vec4(resources->origin, 1.0f));
				//			vec3 direction = vec3(instance->mInverseTransform * vec4(resources->direction, 0.0f));
				//			auto result = detail::rayTraceRayHitBottomLevelAccelerationStructure(instance->mBLAS, origin, direction, resources->tmin, resources->t);
				//			if (result.t >= 0.0f)
				//			{
				//				resources->t = result.t;
				//				resources->attribs = result.attribs;
				//				resources->blasIndex = node->mPrimitiveIndex;
				//				resources->primitiveIndex = result.primitiveIndex;
				//			}
				//			stack_depth--;
				//			continue;
				//		}
				//		else
				//		{
				//			stack_depth++;
				//			stack[stack_depth] = cur_node_index + 2;
				//			continue;
				//		}

				//	}
				//	else if(stack[stack_depth] == 0)
				//	{
				//		stack_depth--;
				//		continue;
				//	}
				//	else
				//	{
				//		stack_depth++;
				//		stack[stack_depth] = node->mSecondChildOffset + 1;
				//	}
				//}
				//return resources->t;

				//for (int i = 0; i < 3; i++)
				//{
				//	if (math::abs(resources->direction[i]) < epsilon &&
				//		(resources->origin[i] < aabb.mMin[i] || resources->origin[i] > aabb.mMax[i]))
				//		return -1.0f;

				//	t0 = (aabb.mMin[i] - resources->origin[i]) / resources->direction[i];
				//	t1 = (aabb.mMax[i] - resources->origin[i]) / resources->direction[i];
				//	if (t0 > t1)
				//		std::swap(t0, t1);
				//	if (t0 > tNear)
				//		tNear = t0;
				//	if (t1 < tFar)
				//		tFar = t1;
				//	if (tNear > tFar || tFar < 0)
				//		return -1.0f;
				//}
				//if (tNear >= resources->t)
				//	return -1.0f;

				if (node->mIsLeaf)
				{
					const RayTraceBottomLevelAccelerationStructureInstanceData* instance = &resources->acStructureData->mInstanceList[node->mPrimitiveIndex];

					vec3 origin = vec3(instance->mInverseTransform * vec4(resources->origin, 1.0f));
					vec3 direction = instance->mDirectionTransform * resources->direction;
					auto result = detail::rayTraceRayHitBottomLevelAccelerationStructure(instance->mBLAS, origin, direction, resources->tmin, resources->t);
					if (result.t >= 0.0f)
					{
						resources->t = result.t;
						resources->attribs = result.attribs;
						resources->blasIndex = node->mPrimitiveIndex;
						resources->primitiveIndex = result.primitiveIndex;
						return result.t;
					}
					else
					{
						return -1.0f;
					}
				}
				else
				{
					float t_left = rayTraceRayHitTopLevelAccelerationStructure_RecursiveSearch(root, nodeIndex + 1, resources);
					float t_right = rayTraceRayHitTopLevelAccelerationStructure_RecursiveSearch(root, node->mSecondChildOffset, resources);

					if (t_right >= 0.0f)
					{
						if (t_left >= 0.0f && t_left < t_right)
							return t_left;
						return t_right;
					}
					return t_left;
				}
			}




			RayTraceResult rayTraceRayHitTopLevelAccelerationStructure(const RayTraceTopLevelAccelerationStructure& acStructure, vec3 origin, vec3 direction, float tmin, float tmax)
			{
				const auto acs_data = (detail::RayTraceTopLevelAccelerationStructureData*)acStructure.handle();
				const auto bvh_data = (const detail::RayTraceBVHNodeData*)acs_data->mBVHData;
				const auto bvh_root = bvh_data;
				RayTraceRayHitTopLevelAccelerationStructureResource resources;
				resources.acStructureData = acs_data;
				resources.origin = origin;
				resources.direction = direction;
				resources.tmin = tmin;
				resources.tmax = tmax;
				resources.t = tmax;
				resources.primitiveIndex = 0xFFFFFFFF;
				resources.blasIndex = 0xFFFFFFFF;
				int stack[1024];
				resources.valueStack = stack;
				RayTraceResult result;
				result.t = rayTraceRayHitTopLevelAccelerationStructure_RecursiveSearch(bvh_root, 0, &resources);

				if (result.t >= 0)
				{
					auto instance = acs_data->mInstanceList[resources.blasIndex];
					result.transform = instance.mTransform;
					result.shaderIndex = instance.mShaderIndex;
					result.mask = instance.mMask;
					result.attribs = resources.attribs;
					auto btm_lv_as_data = (detail::RayTraceBottomLevelAccelerationStructureData*)instance.mBLAS.handle();

					const uint32_t primitive_index = resources.primitiveIndex;
					const uint8_t const* vertex_buffer = (const uint8_t*)btm_lv_as_data->mVertexBuffer.data();
					const uint8_t const* index_buffer = (const uint8_t*)btm_lv_as_data->mIndexBuffer.data();
					uint8_t const* cur_index_ptr = index_buffer;
					const uint32_t vertex_stride = btm_lv_as_data->mVertexStride;
					const uint32_t vertex_offset = btm_lv_as_data->mVertexOffset;
					uint32_t indices[3];
					switch (btm_lv_as_data->mIndexType)
					{
					case IndexType::eUInt32:
						cur_index_ptr += primitive_index * (sizeof(uint32_t) * 3);
						indices[0] = *(uint32_t*)(cur_index_ptr + sizeof(uint32_t) * 0);
						indices[1] = *(uint32_t*)(cur_index_ptr + sizeof(uint32_t) * 1);
						indices[2] = *(uint32_t*)(cur_index_ptr + sizeof(uint32_t) * 2);
						break;
					case IndexType::eUInt16:
						cur_index_ptr += primitive_index * (sizeof(uint16_t) * 3);
						indices[0] = *(uint16_t*)(cur_index_ptr + sizeof(uint16_t) * 0);
						indices[1] = *(uint16_t*)(cur_index_ptr + sizeof(uint16_t) * 1);
						indices[2] = *(uint16_t*)(cur_index_ptr + sizeof(uint16_t) * 2);
						break;
					case IndexType::eUInt8:
						cur_index_ptr += primitive_index * (sizeof(uint8_t) * 3);
						indices[0] = *(uint8_t*)(cur_index_ptr + sizeof(uint8_t) * 0);
						indices[1] = *(uint8_t*)(cur_index_ptr + sizeof(uint8_t) * 1);
						indices[2] = *(uint8_t*)(cur_index_ptr + sizeof(uint8_t) * 2);
						break;
					}
					result.vertices[0] = (vertex_buffer + vertex_stride * indices[0]);
					result.vertices[1] = (vertex_buffer + vertex_stride * indices[1]);
					result.vertices[2] = (vertex_buffer + vertex_stride * indices[2]);
				}
				return result;
			}

		}





	}
}

