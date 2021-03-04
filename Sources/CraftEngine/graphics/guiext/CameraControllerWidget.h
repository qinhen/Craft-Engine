#pragma once
#ifndef CRAFT_ENGINE_GRAPHICS_EXTGUI_CAMERA_CONTROLLER_WIDGET_H_
#define CRAFT_ENGINE_GRAPHICS_EXTGUI_CAMERA_CONTROLLER_WIDGET_H_



#include "../../gui/widgets/Widget.h"
#include "../Camera.h"


namespace CraftEngine
{
	namespace graphics
	{
		namespace extgui
		{

			class CameraControllerWidget : public gui::Widget
			{
			private:

			public:
				Camera* m_camera = nullptr;
			private:
#ifdef CRAFT_ENGINE_USING_DEVICE_SPACE_LEFT_HAND
				bool  inverseControl = true; // openGL
#else 
				bool  inverseControl = false;
#endif

				float zoom = 0;
				static std::vector<const char*> args;

				// Defines a frame rate independent timer value clamped from -1.0...1.0
				// For use in animations, rotations, etc.
				float timer = 0.0f;
				// Multiplier for speeding up (or slowing down) the global timer
				float timerSpeed = 0.25f;

				bool changed = true;
			public:

				CameraControllerWidget(Widget* parent) : Widget(parent)
				{
					this->setDragable(true);
				}

				void setCamera(Camera* camera)
				{
					m_camera = camera;
				}

				bool getChangedState() const { return changed; }
				void setChangedState(bool c) { changed = c; }

				void onPaintEvent() override
				{
					auto painter = getPainter();

					if (m_camera != nullptr)
					{
						if (m_drag_offset_buffer.x != 0 || m_drag_offset_buffer.y != 0) {
							auto bias = 0.01f;
							auto offset = m_drag_offset_buffer * (painter.getFramDuration() * 0.015);
							if (offset.x > 0)
								offset.x = math::clamp(offset.x + bias, 0.0f, m_drag_offset_buffer.x);
							else
								offset.x = math::clamp(offset.x - bias, m_drag_offset_buffer.x, 0.0f);
							if (offset.y > 0)
								offset.y = math::clamp(offset.y + bias, 0.0f, m_drag_offset_buffer.y);
							else
								offset.y = math::clamp(offset.y - bias, m_drag_offset_buffer.y, 0.0f);

							m_drag_offset_buffer -= offset;
							if (inverseControl)
								offset.x = -offset.x;
							m_camera->rotate(CraftEngine::math::vec3(offset.y * m_camera->getRotationSpeed(), offset.x * m_camera->getRotationSpeed(), 0.0f));

							if (m_drag_offset_buffer.x != 0 || m_drag_offset_buffer.y != 0)
								sendRepaintEvent();
							changed = true;
						}
						if (isInputFocus())
						{
							if (gui::KeyBoard::isKeyDown(gui::KeyBoard::KeyVal('A')) || gui::KeyBoard::isKeyDown(gui::KeyBoard::KeyVal::eKey_Left))
							{
								sendRepaintEvent();
								m_camera->keys.left = true;
								changed = true;
							}
							else if (gui::KeyBoard::isKeyDown(gui::KeyBoard::KeyVal('D')) || gui::KeyBoard::isKeyDown(gui::KeyBoard::KeyVal::eKey_Right))
							{
								sendRepaintEvent();
								m_camera->keys.right = true;
								changed = true;
							}
							if (gui::KeyBoard::isKeyDown(gui::KeyBoard::KeyVal('W')) || gui::KeyBoard::isKeyDown(gui::KeyBoard::KeyVal::eKey_Up))
							{
								sendRepaintEvent();
								m_camera->keys.up = true;
								changed = true;
							}
							else if (gui::KeyBoard::isKeyDown(gui::KeyBoard::KeyVal('S')) || gui::KeyBoard::isKeyDown(gui::KeyBoard::KeyVal::eKey_Down))
							{
								sendRepaintEvent();
								m_camera->keys.down = true;
								changed = true;
							}
						}

						m_camera->update(painter.getFramDuration() / 1000.0f);
						m_camera->keys.left = false;
						m_camera->keys.right = false;
						m_camera->keys.up = false;
						m_camera->keys.down = false;
					}

				}

				math::vec2 m_drag_offset_buffer;

				virtual void onDrag(const MouseEvent& mouseEvent) override
				{
					auto real_offset = mouseEvent.offset - getDragOffset();
					m_drag_offset_buffer += math::vec2(real_offset);
					setDragOffset(mouseEvent.offset);
				}

				virtual void onDragApply(const MouseEvent& mouseEvent) override
				{
					setDragOffset({ 0, 0 });
				}

				//virtual Widget* onMoveIn(const MouseEvent& mouseEvent) { getMainWindow()->hideSystemPointer(true); return this; }
				//virtual Widget* onFocusIn(const MouseEvent& mouseEvent) { return this; }

				//virtual void onWheel(const MouseEvent& mouseEvent) { if (getParentWidget() != nullptr) getParentWidget()->onWheel(mouseEvent); }
				//virtual void onClicked(const MouseEvent& mouseEvent) { }
				//virtual void onReleased(const MouseEvent& mouseEvent) { }
				//virtual void onMoveOut(const MouseEvent& mouseEvent) { getMainWindow()->hideSystemPointer(false); }

			};



		}
	}
}

#endif // !CRAFT_ENGINE_GRAPHICS_EXTGUI_CAMERA_CONTROLLER_WIDGET_H_