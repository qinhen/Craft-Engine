#pragma once
#ifndef CRAFT_ENGINE_THIRD_PARTY_IMPORT_OPENGL_H_
#define CRAFT_ENGINE_THIRD_PARTY_IMPORT_OPENGL_H_

#pragma comment( lib, "OpenGL32.lib")
#ifdef _DEBUG
#pragma comment( lib, "glew32d.lib" )  
#else
#pragma comment( lib, "glew32s.lib" )  
#endif


#define GLEW_STATIC  // 链接静态库，需定义GLEW_STATIC  
#include <gl/glew.h>
#include <GL/wglew.h>

#endif // !CRAFT_ENGINE_THIRD_PARTY_IMPORT_OPENGL_H_