#pragma once
#include "./Common.h"

namespace CraftEngine
{
	namespace gui
	{
		namespace model_view
		{

			using NodeDataDescription = ItemDescription;


			struct NodePortDescription
			{
				struct PortInfo
				{
					String name;
					int    styleID = -1;
				};
				std::vector<PortInfo> inputs;
				std::vector<PortInfo> outputs;
			};

			struct NodePortStyle
			{
				enum PortShape
				{
					eShapeDefault,
					eShapeRect,
					eShapeCircle,
					eShapeTriangle,
				};
				enum LinkType
				{
					eLinkDefault,
					eLinkBspline,
					eLinkBezier,
					eLinkLine,
					eLinkStraight,
					eLinkArrow,
				};
				PortShape mShape = PortShape::eShapeDefault;
				LinkType  mLink = LinkType::eLinkDefault;
				Color     mColor = GuiColorStyle::getForegroundColor();
			};

			struct NodeStructure
			{
				enum NodeType
				{
					eNodeDefault,
					eNodeImage,
					eNodeTitle,

				};
				NodeDataDescription mDatas;
				NodePortDescription mPorts;
				NodeType            mType = NodeType::eNodeDefault;

				int getOutPortStyle(int index) const { return mPorts.outputs[index].styleID; }
				int getInPortStyle(int index) const { return mPorts.inputs[index].styleID; }
			};

			struct NodePortIndex
			{
				int mInstanceID;
				int mPortID;

				bool operator==(const NodePortIndex& other) const { return mInstanceID == other.mInstanceID && mPortID == other.mPortID; }
				bool operator!=(const NodePortIndex& other) const { return mInstanceID == other.mInstanceID && mPortID == other.mPortID; }
			};

			class NodeContext
			{
			private:

				std::unordered_map<int, NodeStructure> m_nodeStructureMap;
				std::unordered_map<int, NodePortStyle> m_portStyleMap;
				int m_nodeStructureNextKey = 0;
				int m_portStyleNextKey = 0;
			public:
				int registerNode(const NodeStructure& nodeStructure)
				{
					int key = m_nodeStructureNextKey++;
					m_nodeStructureMap.insert(std::make_pair(key, nodeStructure));
					return key;
				}
				const NodeStructure& getStructure(int key) const
				{
					return m_nodeStructureMap.find(key)->second;
				}
				int registerPortStyle(const NodePortStyle& nodePortStyle)
				{
					int key = m_portStyleNextKey++;
					m_portStyleMap.insert(std::make_pair(key, nodePortStyle));
					return key;
				}
				const NodePortStyle& getPortStyle(int key) const
				{
					return m_portStyleMap.find(key)->second;
				}
			};



			class NodeModel
			{
			private:
				struct NodeInstanceLinkData
				{
					NodePortIndex from;
					NodePortIndex to;
				};

				struct NodeInstanceData
				{
					int    typeID;
					int    instanceID;
					String title;
					core::ArrayList<ItemData>             dataList;
					core::ArrayList<NodeInstanceLinkData> beginLinkList;
					core::ArrayList<NodeInstanceLinkData> endLinkList;
				};
			public:
				class Link;
				class Instance;

				class Link
				{
				private:
					friend class Instance;
					const NodeInstanceLinkData* mLink;
					Link(const NodeInstanceLinkData* link) : mLink(link) { }
				public:
					Link() : mLink(nullptr) { }
					int beginPort() const { return mLink->from.mPortID; }
					int endPort() const { return mLink->to.mPortID; }
					int beginInstance() const { return mLink->from.mInstanceID; }
					int endInstance() const { return mLink->to.mInstanceID; }
					NodePortIndex beginIndex() const { return mLink->from; }
					NodePortIndex endIndex() const { return mLink->to; }
					bool equals(const Link& other) const { mLink->from == other.mLink->from && mLink->to == other.mLink->to; }
				};

				class Instance
				{
				private:
					friend class NodeModel;
					const NodeInstanceData* mInstance;
					const NodeModel*        mModel;
					Instance(const NodeInstanceData* ins, const NodeModel* mod) : mInstance(ins), mModel(mod) { }
				public:
					Instance() : mInstance(nullptr), mModel(nullptr) { }
					int typeID() const { return mInstance->typeID; }
					int instanceID() const { return mInstance->instanceID; }
					const String& title() const { return  mInstance->title; }
					const ItemData& data(int index) const { return mInstance->dataList[index]; }
					int  beginLinkCount() const { return mInstance->beginLinkList.size(); }
					int  endLinkCount() const { return mInstance->endLinkList.size(); }
					Link beginLink(int index) const { return Link(&mInstance->beginLinkList[index]); }
					Link endLinkCount(int index) const { return Link(&mInstance->endLinkList[index]); }
					const NodeStructure& structure() const { return mModel->getContext()->getStructure(typeID()); }
				};

			private:
				friend class NodeView;

				const NodeContext* m_nodeContext;
				core::ArrayList<int>               m_nodeInstancesMap;
				core::ArrayList<NodeInstanceData>  m_nodeInstances;

				craft_engine_gui_signal(instanceCreated, void(int typeKey, int instanceKey));
				craft_engine_gui_signal(instanceDestroyed, void(int instanceKey));
				craft_engine_gui_signal(linkAdded, void(const NodePortIndex& from, const NodePortIndex& to));
				craft_engine_gui_signal(linkRemoved, void(const NodePortIndex& from, const NodePortIndex& to));
				craft_engine_gui_signal(linksAllRemoved, void());
				craft_engine_gui_signal(instanceUpdated, void(int instanceID));
				craft_engine_gui_signal(instancesAllRemoved, void());
				craft_engine_gui_signal(modelDestroyed, void());
			public:

				NodeModel(const NodeContext* nodeContext)
				{
					m_nodeContext = nodeContext;
				}

				~NodeModel()
				{
					craft_engine_gui_emit(modelDestroyed);
				}

				void setNodeData(int instanceID, const ItemData& data, int role)
				{
					getInstanceData(instanceID)->dataList[role] = data;
					craft_engine_gui_emit(instanceUpdated, instanceID);
				}

				const NodeContext* getContext() const
				{
					return m_nodeContext;
				}

				int createInstance(int nodeKey)
				{
					int unusedIdx = -1;
					for (int i = 0; i < m_nodeInstancesMap.size(); i++) {
						if (m_nodeInstancesMap[i] < 0) {
							unusedIdx = i;
							break;
						}
					}
					if (unusedIdx < 0) {
						unusedIdx = m_nodeInstancesMap.size();
						m_nodeInstancesMap.resize(m_nodeInstancesMap.size() + 1);
					}

					m_nodeInstancesMap[unusedIdx] = m_nodeInstances.size();
					m_nodeInstances.resize(m_nodeInstances.size() + 1);
					auto instance = getInstanceData(unusedIdx);

					instance->typeID = nodeKey;
					instance->instanceID = unusedIdx;
					instance->dataList.resize(getContext()->getStructure(nodeKey).mDatas.size());
					instance->beginLinkList.resize(0);
					instance->endLinkList.resize(0);
					craft_engine_gui_emit(instanceCreated, nodeKey, unusedIdx);
					return unusedIdx;
				}

				void removeInstance(int key)
				{
					if (m_nodeInstancesMap[key] < 0)
						throw std::exception("error");
					int lastOneIdx = m_nodeInstances[m_nodeInstances.size() - 1].instanceID;
					m_nodeInstances[m_nodeInstancesMap[key]] = m_nodeInstances[m_nodeInstances.size() - 1];
					m_nodeInstances.resize(m_nodeInstances.size() - 1);
					m_nodeInstancesMap[lastOneIdx] = m_nodeInstancesMap[key];
					m_nodeInstancesMap[key] = -1;
					craft_engine_gui_emit(instanceDestroyed, key);
				}

				core::ArrayList<int> getInstanceKeyList() const
				{
					core::ArrayList<int> keys;
					keys.resize(m_nodeInstances.size());
					for (int i = 0; i < m_nodeInstances.size(); i++)
						keys[i] = m_nodeInstances[i].instanceID;
					return keys;
				}

				core::ArrayList<Instance> getInstanceList() const
				{
					core::ArrayList<Instance> instances;
					instances.resize(m_nodeInstances.size());
					for (int i = 0; i < m_nodeInstances.size(); i++)
						instances[i] = Instance(&m_nodeInstances[i], this);
					return instances;
				}

				Instance getInstance(int key)
				{
					return Instance(&m_nodeInstances[m_nodeInstancesMap[key]], this);
				}

				NodeInstanceData* getInstanceData(int key)
				{
					return &m_nodeInstances[m_nodeInstancesMap[key]];
				}

				bool isLinkExist(const NodePortIndex& from, const NodePortIndex& to)
				{
					auto fromNodeInstance = getInstance(from.mInstanceID);
					for (int i = 0; i < fromNodeInstance.beginLinkCount(); i++)
					{
						auto link = fromNodeInstance.beginLink(i);
						if (link.beginInstance() == from.mInstanceID &&
							link.endInstance() == to.mInstanceID &&
							link.beginPort() == from.mPortID &&
							link.endPort() == to.mPortID)
							return true;
					}
					return false;
				}

				void addLink(const NodePortIndex& from, const NodePortIndex& to)
				{
					if (isLinkExist(from, to))
						return;
					auto cxt = getContext();
					auto fromNodeInstance = getInstance(from.mInstanceID);
					auto toNodeInstance = getInstance(to.mInstanceID);
					auto fromNodeStructure = cxt->getStructure(fromNodeInstance.typeID());
					auto toNodeStructure = cxt->getStructure(toNodeInstance.typeID());

					if (from.mPortID >= fromNodeStructure.mPorts.outputs.size())
						throw std::exception("error");
					if (to.mPortID >= toNodeStructure.mPorts.inputs.size())
						throw std::exception("error");
					if (fromNodeStructure.mPorts.outputs[from.mPortID].styleID != toNodeStructure.mPorts.inputs[to.mPortID].styleID)
						throw std::exception("error");
					NodeInstanceLinkData newLink;
					newLink.from = from;
					newLink.to = to;
					getInstanceData(from.mInstanceID)->beginLinkList.push_back(newLink);
					getInstanceData(to.mInstanceID)->endLinkList.push_back(newLink);
					craft_engine_gui_emit(linkAdded, from, to);
				}

				void removeLink(const NodePortIndex& from, const NodePortIndex& to)
				{
					auto success = false;
					auto fromNodeIns = getInstanceData(from.mInstanceID);
					auto toNodeIns = getInstanceData(to.mInstanceID);
					for (int i = 0; i < fromNodeIns->beginLinkList.size(); i++)
					{
						auto& beginLink = fromNodeIns->beginLinkList[i];
						if (beginLink.from.mPortID == from.mPortID && beginLink.to.mPortID == to.mPortID)
						{
							for (int j = 0; j < toNodeIns->endLinkList.size(); j++)
							{
								auto& endLink = toNodeIns->endLinkList[j];
								if (endLink.from.mPortID == from.mPortID && endLink.to.mPortID == to.mPortID)
								{
									fromNodeIns->beginLinkList.erase(i);
									toNodeIns->endLinkList.erase(j);
									success = true;
								}
							}
						}
					}
					if (!success)
						throw std::exception("error");
					craft_engine_gui_emit(linkRemoved, from, to);
				}

				void removeAllLink()
				{
					for (auto it : m_nodeInstances)
					{
						it.beginLinkList.resize(0);
						it.endLinkList.resize(0);
					}
					craft_engine_gui_emit(linksAllRemoved, );
				}

				void removeAllInstance()
				{
					m_nodeInstancesMap.resize(0);
					m_nodeInstances.resize(0);
					craft_engine_gui_emit(instancesAllRemoved, );
				}
			};


		}
	}
}
