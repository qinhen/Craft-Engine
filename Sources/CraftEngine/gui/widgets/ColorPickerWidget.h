#pragma once
#include "./PushButton.h"
#include "./Label.h"
#include "./TextLine.h"
#include "./TextBox.h"
#include "./ComboBox.h"
#include "./RichTextLine.h"
#include "./CheckBox.h"
#include "./Slider.h"
#include "./SpinBox.h"


namespace CraftEngine
{
	namespace gui
	{


		class ColorPickerWidget : public CraftEngine::gui::Widget
		{
		private:
			using vec2 = CraftEngine::math::vec2;
			using vec3 = CraftEngine::math::vec3;
			using mat3 = CraftEngine::math::mat3;
			using mat2 = CraftEngine::math::mat2;
			using vec4 = CraftEngine::math::vec4;

			class ColorRingWidget : public Widget
			{
			private:

				using PosBuffer = CraftEngine::core::ArrayList<vec2>;
				using ColBuffer = CraftEngine::core::ArrayList<Color>;
				float m_hValue = 0.0f;
				PosBuffer m_ringPosBuffer;
				ColBuffer m_ringColBuffer;
				PosBuffer m_circleFillPosBuffer;
				ColBuffer m_circleFillColBuffer;
				PosBuffer m_circlePosBuffer;

				vec2 m_trianglePosBuffer[3];
				Color m_triangleColBuffer[3];

				vec3 m_power = vec3(1, 0, 0);
				vec3 m_curColor;
				Point m_curPoint;
				bool m_editTriangle = false;
				bool m_editRing = false;
				float m_tempHValue = 0.0f;


			public:
				craft_engine_gui_signal(colorChanged, void(const vec3& color));


				ColorRingWidget(Widget* parent) :Widget(parent)
				{
					this->setDragable(true);
					//this->getPalette().mFillMode = CraftEngine::gui::Palette::eFillMode_Color;
					//this->getPalette().mFrameMode = CraftEngine::gui::Palette::eFrameMode_Surround;
					m_power = vec3(1, 0, 0);
					m_hValue = 0.0f;
					_Create_All_Vertex_List();
				}

				void setHValue(float h)
				{
					m_hValue = CraftEngine::math::clamp(h, 0.0f, 1.0f);
					_Power_Update_TriangleColor_CurPoint();
					_Power_Update_CurColor();
				}

				void setRGB(const vec3& color)
				{
					auto hsv = CraftEngine::math::rgb2hsv(color);
					vec3 fullColor = CraftEngine::math::hsv2rgb(CraftEngine::math::vec3(hsv.x, 1.0f, 1.0f));
					//for (int i = 0; i < 3; i++)
					//	fullColor[i] = fullColor[i] < 0.000001f ? 0.0f : fullColor[i];
					const mat3 cm = {
						fullColor,
						vec3(1.0f),
						vec3(0.0f),
					};
					vec3 cv = color;
					auto max = CraftEngine::math::max(fullColor.x, fullColor.y, fullColor.z);

					int mode = 0;
					if (fullColor.x == max) {
						if (fullColor.x != fullColor.y)mode = 0;
						else mode = 2;
					}
					else if (fullColor.y == max) {
						if (fullColor.x != fullColor.y)mode = 0;
						else mode = 1;
					}
					else {
						if (fullColor.z != fullColor.y)mode = 1;
						else mode = 2;
					}
					if (mode == 0)
					{
						vec2 xy = CraftEngine::math::solve2(mat2(fullColor.x, 1.0f, fullColor.y, 1.0f), vec2(color.x, color.y));
						m_power[0] = xy.x;
						m_power[1] = xy.y;
						m_power[2] = 1 - xy.x - xy.y;
					}
					else if (mode == 1)
					{
						vec2 xy = CraftEngine::math::solve2(mat2(fullColor.y, 1.0f, fullColor.z, 1.0f), vec2(color.y, color.z));
						m_power[0] = xy.x;
						m_power[1] = xy.y;
						m_power[2] = 1 - xy.x - xy.y;
					}
					else
					{
						vec2 xy = CraftEngine::math::solve2(mat2(fullColor.x, 1.0f, fullColor.z, 1.0f), vec2(color.x, color.z));
						m_power[0] = xy.x;
						m_power[1] = xy.y;
						m_power[2] = 1 - xy.x - xy.y;
					}

					for (int i = 0; i < 3; i++)
						m_power[i] = CraftEngine::math::clamp(m_power[i], 0.0f, 1.0f);
					m_power *= 1 / m_power.dot(vec3(1, 1, 1));
					m_curColor = fullColor * m_power[0] + vec3(1.0f) * m_power[1] + vec3(0.0f) * m_power[2];
					m_hValue = CraftEngine::math::clamp(hsv.x, 0.0f, 1.0f);
					_Power_Update_CurColor();
					_Power_Update_TriangleColor_CurPoint();
				}

				vec3 getColor()
				{
					return this->m_curColor;
				}

				float getHValue()
				{
					return m_hValue;
				}

				virtual void onPaintEvent() override
				{
					auto painter = getPainter();
					Widget::onPaintEvent();

					PolygonInfo info{};
					//info.mode = info.ePolygonMode_Triangle;
					info.vertexCount = 3;
					info.posBuffer = m_trianglePosBuffer;
					info.colBuffer = m_triangleColBuffer;
					info.scale = vec2(CraftEngine::math::min(getHeight(), getWidth())) * 0.5f;
					info.translate = vec2(getSize()) * 0.5f;
					info.rotation = 0;
					painter.drawPolygonEx(info);

					//info.mode = info.ePolygonMode_TriangleFrame;
					info.lineWidth = 4.0f;
					info.close = true;
					painter.drawPolylineEx(info, getPalette().mForegroundColor);

					//info.mode = info.ePolygonMode_Triangle;
					info.vertexCount = m_ringPosBuffer.size();
					info.posBuffer = m_ringPosBuffer.data();
					info.colBuffer = m_ringColBuffer.data();
					info.scale = vec2(CraftEngine::math::min(getHeight(), getWidth())) * 0.5f;
					info.translate = vec2(getSize()) * 0.5f;
					info.rotation = -m_hValue * CraftEngine::math::two_pi();
					painter.drawPolygonEx(info);

					info.vertexCount = m_circleFillPosBuffer.size();
					info.posBuffer = m_circleFillPosBuffer.data();
					info.colBuffer = m_circleFillColBuffer.data();
					info.scale = vec2(5.0f);
					info.translate = vec2(m_curPoint);
					info.rotation = 0;
					painter.drawPolygonEx(info);

					//info.mode = info.ePolygonMode_LineStrip;
					info.vertexCount = m_circlePosBuffer.size();
					info.posBuffer = m_circlePosBuffer.data();
					info.scale = vec2(5.0f);
					info.translate = vec2(m_curPoint);
					info.rotation = 0;
					painter.drawPolygonEx(info, Color(0, 0, 0, 255));
				}

			private:
				Color rgb32f2color(const vec3& rgb)
				{
					return Color(decltype(Color::xyz)(rgb * 255), 255);
				}

				virtual void onResizeEvent(const ResizeEvent& resizeEvent) override
				{
					_Power_Update_TriangleColor_CurPoint();
				}

				virtual void onDrag(const MouseEvent& mouseEvent) override
				{
					auto localPos = getLocalPos(mouseEvent.global);
					localPos += mouseEvent.offset;

					if (m_editTriangle)
						_CurPoint_Update_All(localPos);
					else if (m_editRing)
					{
						auto centerPoint = vec2(getSize()) * 0.5f;
						auto downPoint = vec2(getLocalPos(mouseEvent.global));
						auto center2DownPoint = downPoint - centerPoint;
						auto downPoint2CurPoint = vec2(mouseEvent.offset);
						auto center2CurPoint = center2DownPoint + vec2(mouseEvent.offset);
						auto a = center2DownPoint.normalize();
						auto b = center2CurPoint.normalize();
						auto angle = CraftEngine::math::acos(a.dot(b));
						auto delta = angle / CraftEngine::math::two_pi();
						auto tangent = vec2(-center2DownPoint.y, center2DownPoint.x);
						if (tangent.dot(downPoint2CurPoint) > 0)
							delta = -delta;
						auto val = m_tempHValue + delta;
						if (val < 0.0f)
							val = 1.0f - ((-val) - int(-val));
						else
							val = val - int(val);
						setHValue(val);
					}
					else
						return;
					craft_engine_gui_emit(colorChanged, m_curColor);
				}

				//virtual void onDragApply(const MouseEvent& mouseEvent) override
				//{
				//	if (m_editRing)
				//		m_hValue = m_tempHValue;
				//	craft_engine_gui_emit(colorChanged, m_curColor); //
				//}

				virtual void onPressed(const MouseEvent& mouseEvent)override
				{
					auto localPos = getLocalPos(mouseEvent.global);
					m_editTriangle = _Is_CurPoint_Inside_Triangle(localPos);
					m_tempHValue = m_hValue;
					m_editRing = false;
					if (m_editTriangle)
						_CurPoint_Update_All(localPos);
					else
						m_editRing = _Is_CurPoint_Inside_Ring(localPos);
					craft_engine_gui_emit(colorChanged, m_curColor);
				}

				void _Create_All_Vertex_List()
				{
					m_ringPosBuffer.resize(48 * 3 * 2);
					m_ringColBuffer.resize(48 * 3 * 2);
					vec2 coords[48];
					Color colors[48];
					for (int i = 0; i < 48; i++)
					{
						float theta = CraftEngine::math::radians(i / 48.0 * 360.0);
						coords[i] = vec2(CraftEngine::math::cos(theta - CraftEngine::math::half_pi()), CraftEngine::math::sin(theta - CraftEngine::math::half_pi()));
						colors[i] = rgb32f2color(CraftEngine::math::hsv2rgb(vec3(i / 48.0, 1.0f, 1.0f)));
					}
					for (int i = 0; i < 48; i++)
					{
						int idx = i * 6;
						int next = (i + 1) % 48;
						m_ringPosBuffer[idx] = coords[i];
						m_ringColBuffer[idx] = colors[i];
						m_ringPosBuffer[idx + 1] = coords[i] * 0.8;
						m_ringColBuffer[idx + 1] = colors[i];
						m_ringPosBuffer[idx + 2] = coords[next] * 0.8;
						m_ringColBuffer[idx + 2] = colors[next];
						m_ringPosBuffer[idx + 3] = coords[next] * 0.8;
						m_ringColBuffer[idx + 3] = colors[next];
						m_ringPosBuffer[idx + 4] = coords[next];
						m_ringColBuffer[idx + 4] = colors[next];
						m_ringPosBuffer[idx + 5] = coords[i];
						m_ringColBuffer[idx + 5] = colors[i];
					}

					const int level = 12;
					vec2 circleVertex[level + 1];
					for (int i = 0; i < level; i++)
					{
						float theta = CraftEngine::math::radians(i / float(level) * 360.0);
						circleVertex[i] = vec2(CraftEngine::math::cos(theta), CraftEngine::math::sin(theta));
					}
					circleVertex[level] = circleVertex[0];
					m_circleFillPosBuffer.resize(level * 3);
					m_circleFillColBuffer.resize(level * 3);
					for (int i = 0; i < level; i++)
					{
						m_circleFillPosBuffer[i * 3 + 0] = circleVertex[i + 0];
						m_circleFillPosBuffer[i * 3 + 1] = circleVertex[i + 1];
						m_circleFillPosBuffer[i * 3 + 2] = vec2(0);
						m_circleFillColBuffer[i * 3 + 0] = Color(150, 150, 150, 255);
						m_circleFillColBuffer[i * 3 + 1] = Color(150, 150, 150, 255);
						m_circleFillColBuffer[i * 3 + 2] = Color(150, 150, 150, 255);
					}

					m_circlePosBuffer.resize(level + 1);
					for (int i = 0; i < level + 1; i++)
					{
						m_circlePosBuffer[i] = circleVertex[i];
					}

					auto half_pi = CraftEngine::math::half_pi();
					m_trianglePosBuffer[0] = 0.8 * vec2(CraftEngine::math::cos(CraftEngine::math::radians(0) - half_pi), CraftEngine::math::sin(CraftEngine::math::radians(0) - half_pi));
					m_trianglePosBuffer[1] = 0.8 * vec2(CraftEngine::math::cos(CraftEngine::math::radians(120.0f) - half_pi), CraftEngine::math::sin(CraftEngine::math::radians(120.0f) - half_pi));
					m_trianglePosBuffer[2] = 0.8 * vec2(CraftEngine::math::cos(CraftEngine::math::radians(240.0f) - half_pi), CraftEngine::math::sin(CraftEngine::math::radians(240.0f) - half_pi));

					_Power_Update_CurColor();
					_Power_Update_TriangleColor_CurPoint();
				}

				void _Power_Update_CurColor()
				{
					vec3 triangleColors[3];
					triangleColors[0] = CraftEngine::math::hsv2rgb(vec3(m_hValue, 1.0f, 1.0f));
					triangleColors[1] = vec3(1.0f, 1.0f, 1.0f);
					triangleColors[2] = vec3(0.0f, 0.0f, 0.0f);
					m_curColor = triangleColors[0] * m_power[0] + triangleColors[1] * m_power[1] + triangleColors[2] * m_power[2];
				}

				void _Power_Update_TriangleColor_CurPoint()
				{
					auto half_pi = CraftEngine::math::half_pi();
					m_triangleColBuffer[0] = rgb32f2color(CraftEngine::math::hsv2rgb(vec3(m_hValue, 1.0f, 1.0f)));
					m_triangleColBuffer[1] = Color(255, 255, 255, 255);
					m_triangleColBuffer[2] = Color(0, 0, 0, 255);
					auto centerPoint = vec2(getSize()) * 0.5f;
					auto scale = CraftEngine::math::min(getHeight(), getWidth()) * 0.5f;
					vec2 trianglePoints[3] = { m_trianglePosBuffer[0], m_trianglePosBuffer[1], m_trianglePosBuffer[2] };
					for (int i = 0; i < 3; i++)
						trianglePoints[i] = trianglePoints[i] * scale + centerPoint;
					m_curPoint = Point(trianglePoints[0] * m_power[0] + trianglePoints[1] * m_power[1] + trianglePoints[2] * m_power[2]);
				}


				bool _Is_CurPoint_Inside_Triangle(const Point& localPos) const
				{
					auto centerPoint = vec2(getSize()) * 0.5f;
					auto scale = CraftEngine::math::min(getHeight(), getWidth()) * 0.5f;
					vec2 trianglePoints[3];
					trianglePoints[0] = m_trianglePosBuffer[0];
					trianglePoints[1] = m_trianglePosBuffer[1];
					trianglePoints[2] = m_trianglePosBuffer[2];
					for (int i = 0; i < 3; i++)
						trianglePoints[i] = trianglePoints[i] * scale + centerPoint;
					mat3 cm;
					for (int i = 0; i < 3; i++)
						cm[i] = vec3(trianglePoints[i], 1.0f);
					vec3 vm = vec3(vec2(localPos), 1.0f);
					auto power = CraftEngine::math::solve3(cm.transpose(), vm);
					return power[0] >= 0.0f && power[0] <= 1.0f &&
						power[1] >= 0.0f && power[1] <= 1.0f &&
						power[2] >= 0.0f && power[2] <= 1.0f;
				}

				bool _Is_CurPoint_Inside_Ring(const Point& localPos) const
				{
					auto centerPoint = vec2(getSize()) * 0.5f;
					auto scale = CraftEngine::math::min(getHeight(), getWidth()) * 0.5f;
					auto length = (vec2(localPos) - centerPoint).length();
					return length < scale * 1.0f;
				}

				void _CurPoint_Update_All(const Point& localPos)
				{
					auto centerPoint = vec2(getSize()) * 0.5f;
					auto scale = CraftEngine::math::min(getHeight(), getWidth()) * 0.5f;
					vec2 trianglePoints[3];
					vec3 triangleColors[3];
					trianglePoints[0] = m_trianglePosBuffer[0];
					trianglePoints[1] = m_trianglePosBuffer[1];
					trianglePoints[2] = m_trianglePosBuffer[2];
					for (int i = 0; i < 3; i++)
						trianglePoints[i] = trianglePoints[i] * scale + centerPoint;
					triangleColors[0] = CraftEngine::math::hsv2rgb(vec3(m_hValue, 1.0f, 1.0f));
					triangleColors[1] = vec3(1.0f, 1.0f, 1.0f);
					triangleColors[2] = vec3(0.0f, 0.0f, 0.0f);
					mat3 cm;
					for (int i = 0; i < 3; i++)
						cm[i] = vec3(trianglePoints[i], 1.0f);
					vec3 vm = vec3(vec2(localPos), 1.0f);
					m_power = CraftEngine::math::solve3(cm.transpose(), vm);
					for (int i = 0; i < 3; i++)
						m_power[i] = CraftEngine::math::clamp(m_power[i], 0.0f, 1.0f);
					m_power *= 1 / m_power.dot(vec3(1, 1, 1));
					m_curColor = vec3(triangleColors[0] * m_power[0] + triangleColors[1] * m_power[1] + triangleColors[2] * m_power[2]);
					m_curPoint = Point(trianglePoints[0] * m_power[0] + trianglePoints[1] * m_power[1] + trianglePoints[2] * m_power[2]);
				}

			};

			class ColorWidget : public Widget
			{
			private:
				Color m_color;
				vec2 m_backgroundPosBuffer[6];
				vec2 m_backgroundTexBuffer[6];
				Color m_backgroundColBuffer[6];
				HandleImage m_image = CraftEngine::gui::GuiAssetsManager::loadImage("Mosaic");
			public:

				ColorWidget(Widget* parent) : Widget(parent)
				{
					this->getPalette().mFrameMode = CraftEngine::gui::Palette::eFrameMode_Surround;
					m_backgroundPosBuffer[0] = vec2(0.0f, 0.0f);
					m_backgroundPosBuffer[1] = vec2(1.0f, 0.0f);
					m_backgroundPosBuffer[2] = vec2(1.0f, 1.0f);
					m_backgroundPosBuffer[3] = vec2(1.0f, 1.0f);
					m_backgroundPosBuffer[4] = vec2(0.0f, 1.0f);
					m_backgroundPosBuffer[5] = vec2(0.0f, 0.0f);
					for (int i = 0; i < 6; i++)
						m_backgroundTexBuffer[i] = m_backgroundPosBuffer[i];
				}

				void setColor(const Color& color)
				{
					m_color = color;
					for (int i = 0; i < 6; i++)
						m_backgroundColBuffer[i] = color;
				}

				virtual void onPaintEvent() override
				{
					auto painter = getPainter();
					for (int i = 0; i < 6; i++)
						m_backgroundTexBuffer[i] = m_backgroundPosBuffer[i] * 0.0625f * vec2(getSize());

					PolygonInfo info;
					//info.mode = info.ePolygonMode_Triangle;
					info.vertexCount = 6;
					info.posBuffer = m_backgroundPosBuffer;
					info.texBuffer = m_backgroundTexBuffer;
					info.colBuffer = m_backgroundColBuffer;
					info.scale = vec2(getSize());
					info.translate = vec2(0);
					info.rotation = 0;
					info.image = m_image;
					painter.drawPolygonEx(info);
					info.image = HandleImage(nullptr);
					painter.drawPolygonEx(info);
					drawFrame();
				}

			};

			class AChannelSlider : public VSliderFloat
			{
			private:
				Color m_color;
				vec2 m_backgroundPosBuffer[6];
				vec2 m_backgroundTexBuffer[6];
				Color m_backgroundColBuffer[6];
				HandleImage m_image = CraftEngine::gui::GuiAssetsManager::loadImage("Mosaic");
			public:

				AChannelSlider(Widget* parent) : VSliderFloat(parent)
				{
					m_backgroundPosBuffer[0] = vec2(0.0f, 0.0f);
					m_backgroundPosBuffer[1] = vec2(1.0f, 0.0f);
					m_backgroundPosBuffer[2] = vec2(1.0f, 1.0f);
					m_backgroundPosBuffer[3] = vec2(1.0f, 1.0f);
					m_backgroundPosBuffer[4] = vec2(0.0f, 1.0f);
					m_backgroundPosBuffer[5] = vec2(0.0f, 0.0f);
					for (int i = 0; i < 6; i++)
						m_backgroundTexBuffer[i] = m_backgroundPosBuffer[i];
				}

				void setColor(const Color& color)
				{
					m_color = color;
					for (int i = 0; i < 6; i++)
						m_backgroundColBuffer[i] = color;

					m_backgroundColBuffer[0].a = 255;
					m_backgroundColBuffer[1].a = 255;
					m_backgroundColBuffer[2].a = 0;
					m_backgroundColBuffer[3].a = 0;
					m_backgroundColBuffer[4].a = 0;
					m_backgroundColBuffer[5].a = 255;
				}

				virtual void onPaintEvent() override
				{
					auto painter = getPainter();
					for (int i = 0; i < 6; i++)
						m_backgroundTexBuffer[i] = m_backgroundPosBuffer[i] * 0.0625f * vec2(getSize());
					PolygonInfo info;
					//info.mode = info.ePolygonMode_Triangle;
					info.vertexCount = 6;
					info.posBuffer = m_backgroundPosBuffer;
					info.texBuffer = m_backgroundTexBuffer;
					info.colBuffer = m_backgroundColBuffer;
					info.scale = vec2(getSize());
					info.translate = vec2(0);
					info.rotation = 0;
					info.image = m_image;
					painter.drawPolygonEx(info);
					info.image = HandleImage(nullptr);
					painter.drawPolygonEx(info);
					drawChild(getSlider());
					drawFrame();
				}

			};


			ColorWidget* m_colorWidget;
			AChannelSlider* m_aChannelSlider;
			ColorRingWidget* m_colorRingWidget;
			Label* m_labels[4];
			Color m_curColor;

		public:
			craft_engine_gui_signal(colorChanged, void(const Color& color));

			ColorPickerWidget(Widget* parent) : Widget(Rect(0, 0, 250, 200), parent)
			{
				this->getPalette().mFillMode = CraftEngine::gui::Palette::eFillMode_Color;
				this->getPalette().mFrameMode = CraftEngine::gui::Palette::eFrameMode_Surround;
				m_colorRingWidget = new ColorRingWidget(this);
				m_aChannelSlider = new AChannelSlider(this);
				m_aChannelSlider->setValue(1.0f, 1.0f, 0);
				m_colorWidget = new ColorWidget(this);
				m_labels[0] = new Label(L"RGBA:", this);
				m_labels[0]->getPalette().mFrameMode = CraftEngine::gui::Palette::eFrameMode_Surround;
				craft_engine_gui_connect_v2(m_aChannelSlider, slid, [=](float v)
				{
					auto color = m_colorRingWidget->getColor();
					m_curColor = Color(vec4(color, m_aChannelSlider->getValue()) * 255.0f);
					m_colorWidget->setColor(m_curColor);
					m_labels[0]->setText(L"RGBA:[" + StringTool::fromValue(m_curColor[0]) + L"," + StringTool::fromValue(m_curColor[1]) + L"," + StringTool::fromValue(m_curColor[2]) + L"," + StringTool::fromValue(m_curColor[3]) + L"]");
					craft_engine_gui_emit(colorChanged, m_curColor);
				});
				craft_engine_gui_connect_v2(m_colorRingWidget, colorChanged, [=](const vec3& color)
				{
					m_curColor = Color(vec4(color, m_aChannelSlider->getValue()) * 255.0f);
					m_aChannelSlider->setColor(m_curColor);
					m_colorWidget->setColor(m_curColor);
					m_labels[0]->setText(L"RGBA:[" + StringTool::fromValue(m_curColor[0]) + L"," + StringTool::fromValue(m_curColor[1]) + L"," + StringTool::fromValue(m_curColor[2]) + L"," + StringTool::fromValue(m_curColor[3]) + L"]");
					craft_engine_gui_emit(colorChanged, m_curColor);
				});
			}

			void setColor(const Color& color)
			{
				m_curColor = color;
				m_aChannelSlider->setValue(color.a / 255.0f, 1.0f, 0);
				m_colorRingWidget->setRGB(vec3(color.r, color.g, color.b) / 255.0f);
				m_aChannelSlider->setColor(m_curColor);
				m_colorWidget->setColor(m_curColor);
				m_labels[0]->setText(L"RGBA:[" + StringTool::fromValue(m_curColor[0]) + L"," + StringTool::fromValue(m_curColor[1]) + L"," + StringTool::fromValue(m_curColor[2]) + L"," + StringTool::fromValue(m_curColor[3]) + L"]");
			}

			void _Adjust_Layout()
			{
				int h = getHeight() - 22;
				int w = getWidth() - 44;
				int y = 22;
				m_aChannelSlider->setRect(Rect(getWidth() - 22, 0, 22, getHeight()));
				m_colorRingWidget->setRect(Rect(0, y, getWidth() - 22, h));
				m_colorWidget->setRect(Rect(w, 0, 22, y));
				m_labels[0]->setRect(Rect(0, 0, w, 22));
			}

		protected:
			virtual void onResizeEvent(const ResizeEvent& resizeEvent)
			{
				_Adjust_Layout();
			}


		};


	}
}
