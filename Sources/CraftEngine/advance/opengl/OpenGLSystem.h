#pragma once

#include "./OpenGLBase.h"

namespace CraftEngine
{
	namespace opengl
	{

		class OpenGLSystem
		{
		private:
			static std::mutex m_contextLock;
			static bool m_isInitialized;
		public:

			static bool initSystem()
			{
				if (!isInitialized())
				{
					int flag = glewInit();
					if (flag != GLEW_OK) {
						std::cout << "glew Init fail!" + std::to_string(flag) << std::endl;
						throw std::runtime_error("glew Init fail!" + std::to_string(flag));
					}
					m_isInitialized = true;
					std::cout << "OpenGL System is initialized." << std::endl;
					return true;
				}
				else
				{
					throw std::runtime_error("");
				}
				return true;
			}

			static void destroySystem()
			{
				if (isInitialized())
				{
					std::cout << "OpenGL System is destroyed." << std::endl;
				}
				else
				{
					throw std::runtime_error("");
				}
			}

			static bool isInitialized() { return m_isInitialized; }
			static void lockContext() { m_contextLock.lock(); }
			static void unlockContext() { m_contextLock.unlock(); }

		};
		std::mutex OpenGLSystem::m_contextLock;
		bool OpenGLSystem::m_isInitialized = false;


	}
}