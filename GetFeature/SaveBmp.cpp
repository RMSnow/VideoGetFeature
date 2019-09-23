#include "SaveBmp.h"
#include "pch.h"
#include "afxdialogex.h"
#include <iostream>

#include <stdlib.h>
extern "C" {
#include <libavformat/avformat.h>   
#include <libavcodec/avcodec.h>   
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <SDL.h>
#include <SDL_thread.h>
}
using namespace std;

void SaveBmp(AVCodecContext* CodecContex, AVFrame* Picture, int width, int height, int num)
{
	AVPicture pPictureRGB;//RGB图片

	static struct SwsContext* img_convert_ctx;
	img_convert_ctx = sws_getContext(width, height, CodecContex->pix_fmt, width, height, \
		AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
	// 确认所需缓冲区大小并且分配缓冲区空间
	avpicture_alloc(&pPictureRGB, AV_PIX_FMT_RGB24, width, height);
	sws_scale(img_convert_ctx, Picture->data, Picture->linesize, \
		0, height, pPictureRGB.data, pPictureRGB.linesize);

	int lineBytes = pPictureRGB.linesize[0], i = 0;
	char fileName[1024] = { 0 };
	char* bmpSavePath = "%d.bmp";
	sprintf(fileName, bmpSavePath, num);


	FILE* pDestFile = fopen(fileName, "wb");
	BITMAPFILEHEADER btfileHeader;

	btfileHeader.bfType = MAKEWORD(66, 77);
	btfileHeader.bfSize = lineBytes * height;
	btfileHeader.bfReserved1 = 0;
	btfileHeader.bfReserved2 = 0;
	btfileHeader.bfOffBits = 54;

	BITMAPINFOHEADER bitmapinfoheader;
	bitmapinfoheader.biSize = 40;
	bitmapinfoheader.biWidth = width;
	bitmapinfoheader.biHeight = height;
	bitmapinfoheader.biPlanes = 1;
	bitmapinfoheader.biBitCount = 24;
	bitmapinfoheader.biCompression = BI_RGB;
	bitmapinfoheader.biSizeImage = lineBytes * height;
	bitmapinfoheader.biXPelsPerMeter = 0;
	bitmapinfoheader.biYPelsPerMeter = 0;
	bitmapinfoheader.biClrUsed = 0;
	bitmapinfoheader.biClrImportant = 0;

	fwrite(&btfileHeader, 14, 1, pDestFile);
	fwrite(&bitmapinfoheader, 40, 1, pDestFile);
	for (i = height - 1; i >= 0; i--)
	{
		fwrite(pPictureRGB.data[0] + i * lineBytes, 1, width * 3, pDestFile);
	}

	fclose(pDestFile);
	avpicture_free(&pPictureRGB);
}
