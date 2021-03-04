#pragma once
#include "./Common.h"
#include "../advance/opengl/OpenGLSystem.h"

namespace CraftEngine
{
	namespace graphics
	{



		class RenderResources
		{
		private:
			using Texture = opengl::ImageTexture;
			using Buffer = opengl::Buffer;
			core::ArrayList<Texture> m_images;
			core::ArrayList<Buffer> m_buffers;
		public:

			int createImage(GLenum inFormat, GLenum format, GLenum type, int width, int height, int depth = 1)
			{
				Texture tex = opengl::createTexture(width, height, depth, inFormat, format, type, nullptr, 0);
				m_images.push_back(tex);
				return m_images.size() - 1;
			}
			int recreateImage(int index, GLenum inFormat, GLenum format, GLenum type, int width, int height, int depth = 1)
			{
				opengl::destroyTexture(m_images[index]);
				Texture tex = opengl::createTexture(width, height, depth, inFormat, format, type, nullptr, 0);
				m_images[index] = tex;
				return index;
			}

			int createColorTarget(int width, int height, GLint internalFormat = GL_RGBA, GLint format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE)
			{
				Texture tex = opengl::createColorAttachment(width, height, internalFormat, format, type);
				m_images.push_back(tex);
				return m_images.size() - 1;
			}
			int recreateColorTarget(int index, int width, int height, GLint internalFormat = GL_RGBA, GLint format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE)
			{
				opengl::destroyTexture(m_images[index]);
				Texture tex = opengl::createColorAttachment(width, height, internalFormat, format, type);
				m_images[index] = tex;
				return index;
			}
			int createDepthTarget(int width, int height)
			{
				Texture tex = opengl::createDepthAttachment(width, height);
				m_images.push_back(tex);
				return m_images.size() - 1;
			}
			int recreateDepthTarget(int index, int width, int height)
			{
				opengl::destroyTexture(m_images[index]);
				Texture tex = opengl::createDepthAttachment(width, height);
				m_images[index] = tex;
				return index;
			}

			int createBuffer(GLenum target, int size, void* data = nullptr, GLenum usage = GL_DYNAMIC_DRAW)
			{
				Buffer buf = opengl::createBuffer(target, size, data, usage);
				m_buffers.push_back(buf);
				return m_buffers.size() - 1;
			}

			Texture& getImage(int index) { return m_images[index]; }
			Buffer& getBuffer(int index) { return m_buffers[index]; }

			void clear()
			{
				for (auto it : m_images)
					opengl::destroyTexture(it);
				m_images.clear();

				for (auto it : m_buffers)
					opengl::destroyBuffer(it);
				m_buffers.clear();
			}

		};



	}
}


