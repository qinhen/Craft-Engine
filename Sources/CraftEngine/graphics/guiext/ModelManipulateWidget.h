#pragma once

#include "../../gui/widgets/Widget.h"
#include "../Camera.h"


namespace CraftEngine
{
	namespace graphics
	{
		namespace extgui
		{

			class ModelManipulateWidget : public gui::Widget
			{
			private:
				using vec3 = math::vec3;
				using vec2 = math::vec2;
				using Color = gui::Color;
			public:
				struct Transform
				{
					vec3 translate;
					vec3 rotation;
					vec3 scale;
				};


			private:
				Camera* m_camera;
				Transform m_transform;
				bool m_enableManipulate;

				struct OriginData
				{
					vec2 position;
					float radius;
					Color fillColor;
					Color frameColor;
				};
				struct AxisData
				{

					Color fillColor;
					Color frameColor;
				};
				struct TrackData
				{
					Color fillColor;
					Color frameColor;
				};
				struct ScaleData
				{
					Color fillColor;
					Color frameColor;
				};
				struct DrawData
				{
					OriginData originData;
					AxisData axisData[3];
					TrackData trackData[3];
					ScaleData scaleData[3];
				}m_drawData;

				std::vector<vec2> m_verticesBuffer;

			public:
				ModelManipulateWidget()
				{
					m_camera = nullptr;
					m_enableManipulate = false;
					setDragable(true);
				}
				void enableManipulate(bool enable)
				{
					m_enableManipulate = m_camera != nullptr && enable;
				}
				void setCamera(Camera* camera)
				{
					m_camera = camera;
				}
				void setTransform(const Transform& transform)
				{
					m_transform = transform;
				}
				const Transform& getTransform() const
				{
					return m_transform;
				}
				void setTranslate(vec3 translate)
				{

				}
				void setRotation(vec3 rotation)
				{

				}
				void setScale(vec3 scale)
				{

				}
				virtual void onDrag(const MouseEvent& mouseEvent) override
				{

				}
				virtual void onPaintEvent() override
				{
					if (m_enableManipulate == false)
						return;
					
					auto painter = getPainter();
					painter.drawCircle(m_drawData.originData.position, m_drawData.originData.radius, 0.5f, m_drawData.originData.fillColor);
					painter.drawCircleFrame(m_drawData.originData.position, m_drawData.originData.radius, 0.5f, m_drawData.originData.frameColor);

				}
			private:
				
				void updateDrawData()
				{



				}
			};


		}
	}
}
