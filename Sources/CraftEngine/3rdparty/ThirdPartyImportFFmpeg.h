#pragma once
#ifndef CRAFT_ENGINE_THIRD_PARTY_IMPORT_FFMPEG_H_
#define CRAFT_ENGINE_THIRD_PARTY_IMPORT_FFMPEG_H_

//#pragma comment( lib, "libavcodec.lib" ) 
//#pragma comment( lib, "libavutil.lib" ) 
//#pragma comment( lib, "libavformat.lib" ) 
//#pragma comment( lib, "libswscale.lib" ) 
//#pragma comment( lib, "libswresample.lib" ) 
//#pragma comment( lib, "libswscale.lib" ) 

#pragma comment( lib, "avcodec.lib" ) 
#pragma comment( lib, "avutil.lib" ) 
#pragma comment( lib, "avformat.lib" ) 
#pragma comment( lib, "swscale.lib" ) 
#pragma comment( lib, "swresample.lib" ) 
#pragma comment( lib, "swscale.lib" ) 

#pragma comment( lib, "bcrypt.lib")

extern "C"
{
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavutil/channel_layout.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}


#endif // !CRAFT_ENGINE_THIRD_PARTY_IMPORT_FFMPEG_H_