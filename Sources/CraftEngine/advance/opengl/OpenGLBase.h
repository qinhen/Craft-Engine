#pragma once
#ifndef CRAFT_ENGINE_OPENGL_BASE_H_
#define CRAFT_ENGINE_OPENGL_BASE_H_

#include "../../3rdparty/ThirdPartyImportOpenGL.h"
#include "../../math/LinearMath.h"
#include "../../core/Core.h"

namespace CraftEngine
{
	namespace opengl
	{

		struct ImageTexture
		{
			GLuint mImage;
			GLuint mWidth;
			GLuint mHeight;
			GLuint mDepth;
			GLenum mInternalFormat;
			GLenum mFormat;
			GLenum mType;
			GLuint mMipLevel;
			GLuint mTarget;
		};

		class FrameBuffer
		{
		private:
			GLuint m_framebuffer = 0;

		public:
			bool create()
			{
				clear();
				glGenFramebuffers(1, &m_framebuffer);
				return true;
			}
			void clear()
			{
				if (m_framebuffer != 0)
					glDeleteFramebuffers(1, &m_framebuffer);
				m_framebuffer = 0;
			}
			GLuint getFramebuffer()const { return m_framebuffer; }
			void attachColorAttachment(const ImageTexture& attachment, int idx) const
			{
				bind();
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + idx, GL_TEXTURE_2D, attachment.mImage, 0);
			}
			void attachDepthAttachment(const ImageTexture& attachment) const
			{
				bind();
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, attachment.mImage, 0);
			}
			void bind() const
			{
				glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
			}
			bool check() const
			{
				return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
			}
			void unbind() const
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			void drawBuffers(int count) const
			{
				bind();
				const GLenum attachments[10] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3,
				GL_COLOR_ATTACHMENT4,
				GL_COLOR_ATTACHMENT5,
				GL_COLOR_ATTACHMENT6,
				GL_COLOR_ATTACHMENT7,
				GL_COLOR_ATTACHMENT8,
				GL_COLOR_ATTACHMENT9,
				};
				glDrawBuffers(count, attachments);
			}
		};


		struct Buffer
		{
			GLuint mBuffer;
			GLuint mTarget;
			GLenum mUsage;
			GLuint mSize;
			void*  mMapped;
			void bind()
			{
				glBindBuffer(mTarget, mBuffer);
			}
			void bindBase(uint32_t index = 0)
			{
				glBindBufferBase(GL_UNIFORM_BUFFER, index, mBuffer);
			}
			void map()
			{
				if (mMapped != nullptr)
					return;
				bind();
				void* mapBlock = glMapBufferRange(mTarget,
					0,
					mSize,
					GL_MAP_WRITE_BIT);
				mMapped = mapBlock;
			}
			void unmap()
			{
				glUnmapBuffer(mTarget);
				mMapped = nullptr;
			}
			void write(void* data, size_t size, size_t offset = 0)
			{
				bind();
				glBufferSubData(mTarget, offset, size, data);
			}
		};

		Buffer createBuffer(GLenum target, GLuint size, const void* data, GLenum usage = GL_STATIC_DRAW)
		{
			Buffer buf = {};
			glGenBuffers(1, &buf.mBuffer);
			glBindBuffer(target, buf.mBuffer);
			buf.mTarget = target;
			buf.mUsage = usage;
			buf.mSize = size;
			if (data == nullptr && target == GL_STATIC_DRAW)
				buf.mUsage = GL_DYNAMIC_DRAW;	
			else
				glBufferData(target, size, data, usage);
			return buf;
		}

		void destroyBuffer(Buffer& buffer)
		{
			glDeleteBuffers(1, &buffer.mBuffer);
			buffer = {};
		}

		Buffer createVertexBuffer(GLuint size, const void* data = nullptr, GLenum usage = GL_STATIC_DRAW) // GL_DYNAMIC_DRAW
		{
			if (data == nullptr)
				usage = GL_DYNAMIC_DRAW;
			return createBuffer(GL_ARRAY_BUFFER, size, data, usage);
		}
		Buffer createIndexBuffer(GLuint size, const void* data = nullptr, GLenum usage = GL_STATIC_DRAW) // GL_DYNAMIC_DRAW
		{
			if (data == nullptr)
				usage = GL_DYNAMIC_DRAW;
			return createBuffer(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
		}
		Buffer createUniformBuffer(GLuint size, const void* data = nullptr, GLenum usage = GL_STATIC_DRAW) // GL_DYNAMIC_DRAW
		{
			if (data == nullptr)
				usage = GL_DYNAMIC_DRAW;
			return createBuffer(GL_UNIFORM_BUFFER, size, data, usage);
		}
		
		struct VertexArrayObject
		{
			GLuint m_VAO = 0;

			bool create()
			{
				clear();
				glCreateVertexArrays(1, &m_VAO);
				return true;
			}

			void clear()
			{
				if (m_VAO != 0)
					glDeleteVertexArrays(1, &m_VAO);
				m_VAO = 0;
			}

			void bind() const
			{
				glBindVertexArray(m_VAO);
			}

			void format(uint32_t location, uint32_t length, GLenum type, uint32_t offset, uint32_t binding = 0) const
			{
				glEnableVertexArrayAttrib(m_VAO, location);
				glVertexArrayAttribFormat(m_VAO, location, length, type, GL_FALSE, offset);
				glVertexArrayAttribBinding(m_VAO, location, binding);
			}
			void formati(uint32_t location, uint32_t length, GLenum type, uint32_t offset, uint32_t binding = 0) const
			{
				glEnableVertexArrayAttrib(m_VAO, location);
				glVertexArrayAttribIFormat(m_VAO, location, length, type, offset);
				glVertexArrayAttribBinding(m_VAO, location, binding);
			}

			void disable(uint32_t location) const
			{
				glDisableVertexArrayAttrib(m_VAO, location);
			}
			void bindVertexBuffer(const Buffer& vbo, uint32_t offset, uint32_t stride, uint32_t binding = 0) const
			{
				glVertexArrayVertexBuffer(m_VAO, binding, vbo.mBuffer, offset, stride);
			}
			void bindElementBuffer(const Buffer& veo) const
			{
				glVertexArrayElementBuffer(m_VAO, veo.mBuffer);
			}
		};



		ImageTexture createTexture(
			uint32_t width, uint32_t height, uint32_t depth,
			GLint internalFormat, 
			GLint format,
			GLint type, // GL_RGBA
			const void* data, uint32_t size,
			bool mip = false,
			GLenum warpMode = GL_REPEAT //GL_CLAMP_TO_EDGE,
		)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			ImageTexture image;
			glGenTextures(1, &image.mImage);
			if (image.mImage == 0)
				throw std::runtime_error("faild to create a texture.");
			if (depth == 1)
			{
				if (height == 1)
				{
					image.mTarget = GL_TEXTURE_1D;
					glBindTexture(GL_TEXTURE_1D, image.mImage);
					glTexImage1D(GL_TEXTURE_1D, 0, internalFormat, width, 0, format, type, data);
					if (mip) glGenerateMipmap(GL_TEXTURE_1D);
					glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, warpMode);
				}
				else
				{
					image.mTarget = GL_TEXTURE_2D;
					glBindTexture(GL_TEXTURE_2D, image.mImage);
					glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
					if (mip) glGenerateMipmap(GL_TEXTURE_2D);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warpMode);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warpMode);
				}
			}
			else
			{
				image.mTarget = GL_TEXTURE_3D;
				glBindTexture(GL_TEXTURE_3D, image.mImage);
				glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0, format, type, data);
				if (mip) glGenerateMipmap(GL_TEXTURE_3D);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, warpMode);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, warpMode);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, warpMode);
			}
			glTexParameteri(image.mTarget, GL_TEXTURE_MIN_FILTER, mip ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			glTexParameteri(image.mTarget, GL_TEXTURE_MAG_FILTER, mip ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

			image.mWidth = width;
			image.mHeight = height;
			image.mDepth = depth;
			image.mInternalFormat = internalFormat;
			image.mFormat = format;
			image.mType = type;
			image.mMipLevel = std::max(std::max(std::log2(width), std::log2(height)), std::log2(depth));
			return image;
		}


		ImageTexture createTexture3D(uint32_t width, uint32_t height, uint32_t depth, GLint internalFormat, GLint format, GLint type, const void* data, uint32_t size, bool mip = false, GLenum warpMode = GL_REPEAT)
		{
			return createTexture(width, height, depth, internalFormat, format, type, data, size, mip, warpMode);
		}

		ImageTexture createTexture2D(uint32_t width, uint32_t height, GLint internalFormat, GLint format, GLint type, const void* data, uint32_t size, bool mip = false, GLenum warpMode = GL_REPEAT)
		{
			return createTexture(width, height, 1, internalFormat, format, type, data, size, mip, warpMode);
		}

		ImageTexture createTexture1D(uint32_t width, GLint internalFormat, GLint format, GLint type, const void* data, uint32_t size, bool mip = false, GLenum warpMode = GL_REPEAT)
		{
			return createTexture(width, 1, 1, internalFormat, format, type, data, size, mip, warpMode);
		}


		ImageTexture createTexture3D(uint32_t width, uint32_t height, uint32_t depth, GLint format, const void* data, uint32_t size, bool mip = false, GLenum warpMode = GL_REPEAT)
		{
			return createTexture(width, height, depth, format, format, GL_UNSIGNED_BYTE, data, size, mip, warpMode);
		}

		ImageTexture createTexture2D(uint32_t width, uint32_t height, GLint format, const void* data, uint32_t size, bool mip = false, GLenum warpMode = GL_REPEAT)
		{
			return createTexture(width, height, 1, format, format, GL_UNSIGNED_BYTE, data, size, mip, warpMode);
		}

		ImageTexture createTexture1D(uint32_t width, GLint format, const void* data, uint32_t size, bool mip = false, GLenum warpMode = GL_REPEAT)
		{
			return createTexture(width, 1, 1, format, format, GL_UNSIGNED_BYTE, data, size, mip, warpMode);
		}

		void destroyTexture(const ImageTexture& image)
		{
			glDeleteTextures(1, &image.mImage);
		}

		void updateTexture(const ImageTexture& dstImage, uint32_t x, uint32_t y, uint32_t z, uint32_t width, uint32_t height, uint32_t depth, const void* data, uint32_t size, bool mip = false)
		{
			glBindTexture(dstImage.mTarget, dstImage.mImage);
			if (dstImage.mTarget == GL_TEXTURE_3D)
				glTexSubImage3D(dstImage.mTarget, 0, x, y, z, width, height, depth, dstImage.mFormat, GL_UNSIGNED_BYTE, data);
			else if (dstImage.mTarget == GL_TEXTURE_2D)
				glTexSubImage2D(dstImage.mTarget, 0, x, y, width, height, dstImage.mFormat, GL_UNSIGNED_BYTE, data);
			else if (dstImage.mTarget == GL_TEXTURE_1D)
				glTexSubImage1D(dstImage.mTarget, 0, x, width, dstImage.mFormat, GL_UNSIGNED_BYTE, data);
			else
			{

			}
			if (mip) glGenerateMipmap(dstImage.mTarget);
		}

		void updateTexture3D(const ImageTexture& dstImage, uint32_t x, uint32_t y, uint32_t z, uint32_t width, uint32_t height, uint32_t depth, const void* data, uint32_t size, bool mip = false)
		{
			updateTexture(dstImage, x, y, z, width, height, depth, data, mip);
		}

		void updateTexture2D(const ImageTexture& dstImage, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const void* data, uint32_t size, bool mip = false)
		{
			updateTexture(dstImage, x, y, 0, width, height, 1, data, mip);
		}

		void updateTexture1D(const ImageTexture& dstImage, uint32_t x, uint32_t width, const void* data, uint32_t size, bool mip = false)
		{
			updateTexture(dstImage, x, 0, 0, width, 1, 1, data, mip);
		}


		ImageTexture createColorAttachment(uint32_t width, uint32_t height, GLint internalFormat = GL_RGBA, GLint format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE)
		{
			ImageTexture image;
			glGenTextures(1, &image.mImage);
			if (image.mImage == 0)
				throw std::runtime_error("faild to create a texture.");
			glBindTexture(GL_TEXTURE_2D, image.mImage);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			image.mWidth = width;
			image.mHeight = height;
			image.mDepth = 1;
			image.mFormat = format;
			image.mMipLevel = std::max(std::log2(width), std::log2(height));
			return image;
			return createTexture2D(width, height, internalFormat, format, type, nullptr, 0, false);
		}

		ImageTexture createDepthAttachment(uint32_t width, uint32_t height, GLint internalFormat = GL_DEPTH24_STENCIL8, GLint format = GL_DEPTH_STENCIL, GLenum type = GL_UNSIGNED_INT_24_8)
		{
			ImageTexture image;
			glGenTextures(1, &image.mImage);
			if (image.mImage == 0)
				throw std::runtime_error("faild to create a texture.");
			glBindTexture(GL_TEXTURE_2D, image.mImage);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			image.mWidth = width;
			image.mHeight = height;
			image.mDepth = 1;
			image.mFormat = format;
			image.mMipLevel = std::max(std::log2(width), std::log2(height));
			return image;
			return createTexture2D(width, height, internalFormat, format, type, nullptr, 0, false);
		}



		class Shader {
		private:
			using vec2 = math::vec2;
			using vec3 = math::vec3;
			using vec4 = math::vec4;
			using ivec2 = math::ivec2;
			using ivec3 = math::ivec3;
			using ivec4 = math::ivec4;
			using uvec2 = math::uvec2;
			using uvec3 = math::uvec3;
			using uvec4 = math::uvec4;
			using mat2 = math::mat2;
			using mat3 = math::mat3;
			using mat4 = math::mat4;
			GLuint m_ID = 0;
		public:

			bool loadShaderFromFile(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = NULL);
			bool loadShaderFromMemory(const GLchar* vertexShaderCode, const GLchar* fragmentShaderCode, const GLchar* geometryShaderCode = NULL);
			void clear() { glDeleteProgram(m_ID); }

			inline void use() const {
				glUseProgram(m_ID);
			}
			inline void disUse() const {
				glUseProgram(0);
			}

			inline int32_t getUniformLocation(const char* pName) const {
				return glGetUniformLocation(m_ID, pName);
			}


			inline void setUniform(int32_t location, int value) const {
				glUniform1i(location, value);
			}inline void setUniform(int32_t location, unsigned int value) const {
				glUniform1ui(location, value);
			}inline void setUniform(int32_t location, bool value) const {
				glUniform1i(location, (int)value);
			}inline void setUniform(int32_t location, const float value) const {
				glUniform1f(location, value);
			}inline void setUniform(int32_t location, const math::vec2& value) const {
				glUniform2fv(location, 1, &value[0]);
			}inline void setUniform(int32_t location, const math::vec3& value) const {
				glUniform3fv(location, 1, &value[0]);
			}inline void setUniform(int32_t location, const math::vec4& value) const {
				glUniform4fv(location, 1, &value[0]);
			}inline void setUniform(int32_t location, const math::mat2& value) const {
				glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
			}inline void setUniform(int32_t location, const math::mat3& value) const {
				glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
			}inline void setUniform(int32_t location, const math::mat4& value) const {
				glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
			}inline void setUniform(int32_t location, const math::ivec2& value) const {
				glUniform2iv(location, 1, &value[0]);
			}inline void setUniform(int32_t location, const math::ivec3& value) const {
				glUniform3iv(location, 1, &value[0]);
			}inline void setUniform(int32_t location, const math::ivec4& value) const {
				glUniform4iv(location, 1, &value[0]);
			}inline void setUniform(int32_t location, const math::uvec2& value) const {
				glUniform2uiv(location, 1, &value[0]);
			}inline void setUniform(int32_t location, const math::uvec3& value) const {
				glUniform3uiv(location, 1, &value[0]);
			}inline void setUniform(int32_t location, const math::uvec4& value) const {
				glUniform4uiv(location, 1, &value[0]);
			}

			inline void setUniform(int32_t location, const math::mat2* value, int count) const {
				glUniformMatrix2fv(location, count, GL_FALSE, (float*)value);
			}inline void setUniform(int32_t location, const math::mat3* value, int count) const {
				glUniformMatrix3fv(location, count, GL_FALSE, (float*)value);
			}inline void setUniform(int32_t location, const math::mat4* value, int count) const {
				glUniformMatrix4fv(location, count, GL_FALSE, (float*)value);
			}

			template<typename Type>
			inline void setUniformName(const std::string& name, const Type& value) const {
				this->setUniform(this->getUniformLocation(name.c_str()), value);
			}
		};

		bool Shader::loadShaderFromFile(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath) {
			//！1、读取着色器的代码
			std::string vertexCode;
			std::string fragmentCode;
			std::string geometryCode;
			std::ifstream vShaderFile;
			std::ifstream fShaderFile;
			std::ifstream gShaderFile;
			//确保文件流会输出异常
			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			if (geometryPath != NULL)
				gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

			try {
				//打开文件
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);
				if (geometryPath != NULL)
					gShaderFile.open(geometryPath);
				std::stringstream vShaderStream, fShaderStream, gShaderStream;
				//读取文件到流中
				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();
				if (geometryPath != NULL)
					gShaderStream << gShaderFile.rdbuf();
				//关闭文件
				vShaderFile.close();
				fShaderFile.close();
				if (geometryPath != NULL)
					gShaderFile.close();
				//将流转换为字符串
				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
				if (geometryPath != NULL)
					geometryCode = gShaderStream.str();
			}
			catch (std::ifstream::failure e) {
				std::cout << "错误：读取文件失败，请检查文件是否存在！" << std::endl;
				return false;
			}

			//！2、编译着色器
			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();
			const char* gShaderCode = geometryPath != NULL ? geometryCode.c_str() : NULL;

			return loadShaderFromMemory(vShaderCode, fShaderCode, gShaderCode);
		}


		bool Shader::loadShaderFromMemory(const GLchar* vertexShaderCode, const GLchar* fragmentShaderCode, const GLchar* geometryShaderCode)
		{
			unsigned int vertex, fragment, geometry;
			int success;
			char infoLog[512];

			//顶点着色器
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vertexShaderCode, NULL);
			glCompileShader(vertex);
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				std::cout << "编译顶点着色器失败，错误信息" << infoLog << std::endl;
				return false;
			}

			//片元着色器
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
			glCompileShader(fragment);
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(fragment, 512, NULL, infoLog);
				std::cout << "编译片元着色器失败，错误信息" << infoLog << std::endl;
				return false;
			}

			//几何着色器
			if (geometryShaderCode != NULL) {
				geometry = glCreateShader(GL_GEOMETRY_SHADER);
				glShaderSource(geometry, 1, &geometryShaderCode, NULL);
				glCompileShader(geometry);
				glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
				if (!success) {
					glGetShaderInfoLog(geometry, 512, NULL, infoLog);
					std::cout << "编译几何着色器失败，错误信息" << infoLog << std::endl;
					return false;
				}
			}

			//着色器程序
			m_ID = glCreateProgram();
			glAttachShader(m_ID, vertex);
			glAttachShader(m_ID, fragment);
			if (geometryShaderCode != NULL)
				glAttachShader(m_ID, geometry);
			glLinkProgram(m_ID);

			glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
				std::cout << "链接着色器程序失败，错误信息：" << infoLog << std::endl;
				return false;
			}

			//删除着色器程序
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			if (geometryShaderCode != NULL)
				glDeleteShader(geometry);
			return true;
		}








	}
}
#endif // !CRAFT_ENGINE_OPENGL_BASE_H_