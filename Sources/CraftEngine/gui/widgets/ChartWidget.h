#pragma once
#ifndef CRAFT_ENGINE_GUI_GRAPH_WIDGET_H_
#define CRAFT_ENGINE_GUI_GRAPH_WIDGET_H_
#include "./AbstractGraphicsWidget.h"
namespace CraftEngine
{
	namespace gui
	{


		class LineChart : public AbstractGraphicsWidget
		{
		private:
			std::vector<float> m_pointValue;
			std::vector<GraphicsRenderer::Vertex> m_vertexData;
			int m_maxPointCount = 0;
			float m_minValue;
			float m_maxValue;
			HandleVertexBuffer m_vertexBuffer;
			math::mat4 m_vpMatrix;
		public:

			virtual ~LineChart()
			{
				getThread().wait();
				if (m_vertexBuffer != nullptr)
					GuiRenderSystem::deleteVertexBuffer(m_vertexBuffer);
			}

			LineChart(const Rect& rect,Widget* parent) : AbstractGraphicsWidget(rect, parent)
			{
				setDragable(true);
			}

			LineChart(Widget* parent) : LineChart(Rect(0, 0, 400, 400), parent)
			{
				setChartInfo(0, 100, 100);
			}
	
			void setValue(int index, float value)
			{
				m_pointValue[index] = value;
			}

			void pushValue(float value)
			{
				for (int i = 0; i < m_maxPointCount - 1; i++)
				{
					setValue(i, m_pointValue[i + 1]);
				}
				setValue(m_maxPointCount - 1, value);
			}

			void update()
			{
				auto f = [&]
				{
					if (m_vertexBuffer.unused != nullptr)
					{
						GuiRenderSystem::deleteVertexBuffer(m_vertexBuffer);
					}
					for (int i = 0; i < m_pointValue.size() - 1; i++)
					{
						m_vertexData[2 * i].position = math::vec3(i, m_pointValue[i], 0);
						m_vertexData[2 * i].color = Color(100, 100, 100, 150);
						m_vertexData[2 * i + 1].position = math::vec3(i + 1, m_pointValue[i + 1], 0);
						m_vertexData[2 * i + 1].color = Color(100, 100, 100, 150);
					}
					m_vertexBuffer = GuiRenderSystem::createVertexBuffer(m_vertexData.data(), m_vertexData.size() * sizeof(GraphicsRenderer::Vertex), m_vertexData.size());
				};
				getThread().push(f);
			}

			void setChartInfo(float min, float max)
			{
				m_minValue = min;
				m_maxValue = max;
				m_vpMatrix =
					math::ortho(0.0f, (float)m_pointValue.size() - 1, min, max, -1.0f, 1.0f) *
					math::lookAt(math::vec3(0, 0, 0), math::vec3(0, 0, 1), math::vec3(0, 1, 0));
				getGraphRenderer()->setVPMatrix(m_vpMatrix);
			}

			void setChartInfo(float min, float max, int count)
			{
				auto f = [&]
				{
					if (m_vertexBuffer.unused != nullptr)
					{
						GuiRenderSystem::deleteVertexBuffer(m_vertexBuffer);
					}
					m_pointValue.resize(count, 0);
					m_vertexData.resize(2 * (count - 1), {});
					m_maxPointCount = count;
					m_vertexBuffer = GuiRenderSystem::createVertexBuffer(m_vertexData.data(), m_vertexData.size() * sizeof(GraphicsRenderer::Vertex), m_vertexData.size());
				};
				getThread().push(f);			
				setChartInfo(min, max);
			}

		protected:

			virtual void onPaint(GraphicsRenderer* renderer)override
			{
				GraphicsRenderer::PaintingParameter param;
				param.type = gui::GraphicsRenderer::PrimitiveType::LineList;
				param.model = math::mat4(1);
				param.frameMode = false;
				renderer->drawPrimitive(m_vertexBuffer, param);
			}

		};










	}

}





#endif // !CRAFT_ENGINE_GUI_GRAPH_WIDGET_H_


