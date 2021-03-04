#pragma once
#include "../math/LinearMath.h"


namespace CraftEngine
{
	namespace graphics
	{

		class Camera
		{
		public:
			enum ViewMode
			{
				eViewMode_FirstPerson,
				eViewMode_ThirdPerson,
			};

		private:
			using vec4 = CraftEngine::math::vec4;
			using vec3 = CraftEngine::math::vec3;
			using vec2 = CraftEngine::math::vec2;
			using mat4 = CraftEngine::math::mat4;
		private:
			ViewMode m_viweMode = ViewMode::eViewMode_ThirdPerson;
			vec3 m_rotation = vec3(0);
			vec3 m_position = vec3(0);
			vec3 m_front_dir = vec3(0, 0, 1);

			float m_rotation_speed = 0.25f;
			float m_movement_speed = 2.0f;
			float m_distance = 1.0f;
			float m_aspect = 16.0f / 9.0f;
			float m_fov = 60.0f;
			float m_znear = 0.1f, m_zfar = 256.0f;

			bool m_updated = false;
		public:

			Camera()
			{
				setPerspective(60.0f, 16.0f / 9.0f, 0.1f, 256.0f);
				setPosition({ 0, 0, 0 });
				setRotation({ 0, 0, 0 });
			}

			float getRotationSpeed() const { return m_rotation_speed; }
			float getMovementSpeed() const { return m_movement_speed; }
			float getDistance() const { return m_distance; }
			vec3 getFrontDir() const { return m_front_dir; }
			vec3 getRotation() const { return m_rotation; }
			vec3 getPosition() const
			{ 
				switch (m_viweMode)
				{
				case ViewMode::eViewMode_FirstPerson:return m_position;
				case ViewMode::eViewMode_ThirdPerson:return m_position - m_distance * m_front_dir;
				}
			}
		
			struct
			{
				mat4 perspective;
				mat4 view;
			} matrices;

			struct
			{
				bool left = false;
				bool right = false;
				bool up = false;
				bool down = false;
			} keys;

			bool moving()
			{
				return keys.left || keys.right || keys.up || keys.down;
			}

			float getNearClip() const { return m_znear; }
			float getFarClip() const { return m_zfar; }
			float getAspectRatio() const { return m_aspect; }

			void setPerspective(float fov, float aspect, float znear, float zfar)
			{
				this->m_fov = fov;
				this->m_znear = znear;
				this->m_zfar = zfar;
				this->m_aspect = aspect;
				updateProjectionMatrix();
			};

			void setAspectRatio(float aspect = 16.0f / 9.0f)
			{
				this->m_aspect = aspect;
				updateProjectionMatrix();
			}

			void setPosition(math::vec3 m_position)
			{
				this->m_position = m_position;
				updateViewMatrix();
			}

			void setRotation(math::vec3 rotation)
			{
				this->m_rotation = rotation;
				updateViewMatrix();
			};

			void setDistance(float distance)
			{
				this->m_distance = distance;
				updateViewMatrix();
			}

			void setDirection(math::vec3 direction)
			{
				auto angle_y = math::degrees(math::orientedAngle(math::normalize(math::vec2(direction.x, direction.z)), vec2(0, 1)));
				auto angle_x = math::degrees(math::orientedAngle(math::vec2(math::length(math::vec2(direction.x, direction.z)), direction.y), vec2(1, 0)));
				this->m_rotation = vec3(angle_x, angle_y, 0);
				updateViewMatrix();
			}

			void rotate(math::vec3 delta)
			{
				this->m_rotation += delta;
				this->m_rotation.x = math::clamp(this->m_rotation.x, -90.0f + 0.1f, 90.0f - 0.1f);
				updateViewMatrix();
			}

			void update(float deltaTime)
			{
				m_updated = false;
				if (moving())
				{
					float moveSpeed = deltaTime * m_movement_speed;
					vec3 left_dir = math::normalize(math::cross(m_front_dir, math::vec3(0.0f, 1.0f, 0.0f)));
					if (CRAFT_ENGINE_MATH_CLIP_CONTROL & CRAFT_ENGINE_MATH_CLIP_WORLD_SPACE_LH)
						left_dir = math::normalize(math::cross(m_front_dir, math::vec3(0.0f, 1.0f, 0.0f)));
					else
						left_dir = math::normalize(math::cross(math::vec3(0.0f, 1.0f, 0.0f), m_front_dir));

					if (keys.up)
						m_position += m_front_dir * moveSpeed;
					if (keys.down)
						m_position -= m_front_dir * moveSpeed;
					if (keys.left)
						m_position += left_dir * moveSpeed;
					if (keys.right)
						m_position -= left_dir * moveSpeed;
					updateViewMatrix();		
				}
			};

		private:
			void updateViewMatrix()
			{
				vec3 relative_rotation = m_rotation;
				// 默认方向为:(0, 0, 1) x->y->z
				math::mat3 dir_rot_mat = math::rotate(vec3(math::radians(relative_rotation.x), math::radians(relative_rotation.y), math::radians(relative_rotation.z)));
				math::mat3 x_rot_mat = math::rotate(math::radians(relative_rotation.x), math::vec3(1, 0, 0));
				math::mat3 y_rot_mat = math::rotate(math::radians(relative_rotation.y), math::vec3(0, 1, 0));

				m_front_dir = y_rot_mat * (x_rot_mat * vec3(0, 0, 1));
				m_front_dir = math::normalize(m_front_dir);

				switch (m_viweMode)
				{
				case ViewMode::eViewMode_FirstPerson:
					matrices.view = math::lookAt(m_position, m_position + m_front_dir, vec3(0, 1, 0));
					break;
				case ViewMode::eViewMode_ThirdPerson:
					matrices.view = math::lookAt(m_position - m_distance * m_front_dir, m_position, vec3(0, 1, 0));
					break;
				}
				m_updated = true;
			};
			void updateProjectionMatrix()
			{
				matrices.perspective = math::perspective(math::radians(m_fov), m_aspect, m_znear, m_zfar);
			}

		};

	}
}


