#pragma once
#ifndef CRAFT_ENGINE_THIRD_PARTY_IMPORT_ASSIMP_H_
#define CRAFT_ENGINE_THIRD_PARTY_IMPORT_ASSIMP_H_



#ifdef _DEBUG
#pragma comment( lib, "assimpd.lib" ) 
#else
#pragma comment( lib, "assimp-vc142-mt.lib" ) 
#endif
//#pragma comment( lib, "assimp.lib" ) 
//#pragma comment( lib, "IrrXML.lib" ) 
//#pragma comment( lib, "zlibstatic.lib" ) 
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#endif // !CRAFT_ENGINE_THIRD_PARTY_IMPORT_ASSIMP_H_