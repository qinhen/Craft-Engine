#pragma once
#include "./Common.h"
#include "./Camera.h"

namespace CraftEngine
{
	namespace graphics
	{

		class Scene : public AbstraceScene
		{
		private:
			std::unordered_map<IDType, ModelInstance> m_instanceList;
			core::ArrayList<IDType>                   m_renderInstanceList;
			IDType m_nextID = 0;
			Camera m_camera;
		public:

			virtual IDType createInstance(int model) override
			{
				IDType id = m_nextID++;
				ModelInstance ins;
				ins.mModel = model;
				m_instanceList.emplace(std::make_pair(id, ins));
				return id;
			}
			virtual ModelInstance& getInstance(IDType id) override
			{
				return m_instanceList[id];
			}
			virtual void destroyInstance(IDType id) override
			{
				auto it = m_instanceList.find(id);
				if (it == m_instanceList.end())
					return;
				m_instanceList.erase(id);
			}
			virtual Camera& getCamera() override 
			{
				return m_camera;
			}
			virtual void updateScene() override 
			{
				for (auto& it : m_instanceList)
				{
					math::mat4 model = math::mat4();
					model = math::rotate(it.second.mTransform.mRotation, model);
					model = math::scale(it.second.mTransform.mScale, model);
					model = math::translate(it.second.mTransform.mTranslate, model);
					it.second.mModelMatrix = model;
				}
				m_renderInstanceList.resize(m_instanceList.size());
				int i = 0;
				for (auto& it : m_instanceList)
					m_renderInstanceList[i++] = it.first;
			}
			virtual core::ArrayList<int> getInstanceList() override
			{
				return m_renderInstanceList;
			}
		};



	}
}


