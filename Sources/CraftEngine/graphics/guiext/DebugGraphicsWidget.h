#pragma once

#include "../Camera.h"
#include "../../gui/widgetsext/VulkanWidget.h"
#include "../RenderAssetsManager.h"

namespace CraftEngine
{
	namespace graphics
	{


		class DebugGraphicsWidget : public gui::VulkanWidget
		{
		private:

		public:
			Camera m_camera;
		private:

			float zoom = 0;

			static std::vector<const char*> args;

			// Defines a frame rate independent timer value clamped from -1.0...1.0
			// For use in animations, rotations, etc.
			float timer = 0.0f;
			// Multiplier for speeding up (or slowing down) the global timer
			float timerSpeed = 0.25f;

		public:
			AbstractRenderer* m_pipeline;

			DebugGraphicsWidget(AbstractRenderer* pipeline, Widget* parent) : VulkanWidget(parent)
			{
				float width = 1280;
				float height = 720;

				m_camera.setPerspective(60.0f, (float)width / (float)height, 0.1f, 512.0f);
				m_camera.setRotation(CraftEngine::math::vec3(0.0f, 0.0f, 0.0f));
				m_camera.setPosition(CraftEngine::math::vec3(0.0f, 0.0f, -1.5f));

				m_pipeline = pipeline;
				m_pipeline->init(width, height, &m_camera);

				//auto m = graphics::RenderAssetsManager::loadModel("./model/car/SportCar20.FBX");
				//auto m = graphics::RenderAssetsManager::loadModel("./models/reflection_test.dae");
				auto m = graphics::RenderAssetsManager::loadModel("./model/venus.fbx");
			
				auto i = m_pipeline->createModel(m);
				m_pipeline->createInstance(i);

				this->bindImage(m_pipeline->getView());
				this->setDragable(true);
			}


			Camera* getCamera() { return &m_camera; }

			void onPaintEvent(const PaintEvent& paintEvent)
			{
				m_pipeline->updateUniformBuffers();
				m_pipeline->callDraw();
				VulkanWidget::onPaintEvent(paintEvent);
			}


		};


	}
}