#pragma once
#ifdef __Save_BMP_H__
#endif __Save_BMP_H__

#include <stdlib.h>
extern "C" {
#include <libavformat/avformat.h>   
#include <libavcodec/avcodec.h>   
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <SDL.h>
#include <SDL_thread.h>
}

void SaveBmp(AVCodecContext* CodecContex, AVFrame* Picture, int width, int height, int num);