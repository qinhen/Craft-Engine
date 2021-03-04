#pragma once

#include "../widgets/Widget.h"
#include "../../soft3d/Image.h"

namespace CraftEngine
{
	namespace gui
	{


		class Soft3DWidget :public Widget
		{
		private:
			soft3d::Image m_image;
			HandleImage   m_imageHandle = HandleImage(nullptr);
		public:
			craft_engine_gui_signal(drawFrame, void(void));

			Soft3DWidget(Widget* parent): Widget(parent)
			{

			}

			~Soft3DWidget()
			{
				clearBinding();
			}

			void clearBinding()
			{
				if (m_imageHandle != nullptr)
					GuiRenderSystem::deleteImage(m_imageHandle);
				m_imageHandle = HandleImage(nullptr);
				m_image = soft3d::Image();
			}

			void bindImage(const soft3d::Image& image)
			{
				clearBinding();
				if (image.valid())
				{
					m_image = image;
					m_imageHandle = GuiRenderSystem::createImage(image.memory().data(), image.width() * image.height() * 4, image.width(), image.height());
				}
			}

			void updateImage()
			{
				if (m_imageHandle != nullptr)
				{
					GuiRenderSystem::updateImage(m_imageHandle, m_image.memory().data(), m_image.width() * m_image.height() * 4, m_image.width(), m_image.height());
				}
			}

			virtual void onPaintEvent() override
			{
				craft_engine_gui_emit(drawFrame);
				if (m_imageHandle != nullptr)
				{
					auto painter = getPainter();
					painter.drawRect(Rect(Point(0), getSize()), m_imageHandle);
				}
			}

		};


	}
}
