// CJiaohuDlg.cpp: 实现文件
//

#include "pch.h"
#include "GetFeature.h"
#include "CJiaohuDlg.h"
#include "afxdialogex.h"


//*************************** ffmpeg ***************************
#include <iostream>

// LNK2019	无法解析的外部符号 SDL_main，该符号在函数 main_utf8 中被引用
#define SDL_MAIN_HANDLED 

//Refresh Event
#define SFM_REFRESH_EVENT     (SDL_USEREVENT + 1)
#define SFM_BREAK_EVENT       (SDL_USEREVENT + 2)

extern "C" {
#include <libavformat/avformat.h>   
#include <libavcodec/avcodec.h>   
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <SDL.h>
}
//**************************************************************

// CJiaohuDlg 对话框

IMPLEMENT_DYNAMIC(CJiaohuDlg, CDialogEx)

CJiaohuDlg::CJiaohuDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1_JIAOHU, pParent)
{

}

CJiaohuDlg::~CJiaohuDlg()
{
}

void CJiaohuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FEATURE, m_feature_folder_path);
}


BEGIN_MESSAGE_MAP(CJiaohuDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CJiaohuDlg::OnBnClickedButtonOpen)
	ON_STN_CLICKED(IDC_PICTURE_PLAY, &CJiaohuDlg::OnStnClickedPicturePlay)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CJiaohuDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_SET_FOLDER, &CJiaohuDlg::OnBnClickedButtonSetFolder)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FOLDER, &CJiaohuDlg::OnBnClickedButtonOpenFolder)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_SLOWLY, &CJiaohuDlg::OnBnClickedButtonPlaySlowly)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_FAST, &CJiaohuDlg::OnBnClickedButtonPlayFast)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_FRAME, &CJiaohuDlg::OnBnClickedButtonPlayFrame)
END_MESSAGE_MAP()


//*************************** ffmpeg ***************************

void log_s(const char* msg, int d = -1123) {
	if (d == -1123) {
		printf_s("%s\n", msg);
	}
	else {
		printf_s("%s  %d \n", msg, d);
	}
}


int thread_exit = 0;
int thread_pause = 0;

CWinThread* play_thread = NULL;

int is_playing_fast = 0;
int is_playing_slowly = 0;
int is_playing_frame = 0;

int sfp_refresh_thread(void* opaque) {
	while (!thread_exit) {
		if (!thread_pause) {
			SDL_Event event;
			event.type = SFM_REFRESH_EVENT;
			SDL_PushEvent(&event);

			//慢放
			if (is_playing_slowly) {
				SDL_Delay(20);
				continue;
			}
			//快放
			else if (is_playing_fast)
			{
				SDL_Delay(5);
				continue;
			}
			//正常播放
			SDL_Delay(10);
		}
		//若为单帧播放
		else if(is_playing_frame)
		{
			SDL_Event event;
			event.type = SFM_REFRESH_EVENT;
			SDL_PushEvent(&event);

			is_playing_frame = 0;
		}
	}
	thread_exit = 0;
	thread_pause = 0;
	//Break
	SDL_Event event;
	event.type = SFM_BREAK_EVENT;
	SDL_PushEvent(&event);

	return 0;
}

UINT videoplayer(LPVOID lpParam) {
	AVFormatContext* pFmtCtx = NULL;
	AVCodecContext* pCodecCtx = NULL;
	AVFrame* pFrame = NULL;
	AVFrame* pFrameYUV = NULL;
	uint8_t* outBuffer = NULL;
	AVPacket* pPacket = NULL;
	SwsContext* pSwsCtx = NULL;

	//SDL
	int screen_w, screen_h;
	SDL_Window* screen = NULL;
	SDL_Renderer* sdlRenderer = NULL;
	SDL_Texture* sdlTexture = NULL;
	SDL_Rect sdlRect;
	SDL_Thread* video_tid = NULL;
	SDL_Event event;

	//======================== 添加 ========================
	CJiaohuDlg* dlg = (CJiaohuDlg*)lpParam;
	//char filepath[500] = { 0 };
	////LPSTR被定义成是一个指向以NULL('\0')结尾的32位ANSI字符数组指针
	//GetWindowTextA(dlg->m_feature_folder_path, (LPSTR)filepath, 500);

	CString video_path = dlg->m_video_file_path;
	//AfxMessageBox(video_path);

	USES_CONVERSION;
	char* filepath = W2A(video_path);
	//printInConsole(filepath);

	//1. 初始化
	av_register_all();
	avformat_network_init();
	//2. AVFormatContext获取
	pFmtCtx = avformat_alloc_context();
	//3. 打开文件
	if (avformat_open_input(&pFmtCtx, filepath, NULL, NULL) != 0) {
		log_s("Couldn't open input stream.\n");
		return -1;
	}
	//4. 获取文件信息
	if (avformat_find_stream_info(pFmtCtx, NULL) < 0) {
		log_s("Couldn't find stream information.");
		return -1;
	}
	//5. 获取视频的index
	int i = 0, videoIndex = -1;
	for (; i < pFmtCtx->nb_streams; i++) {
		if (pFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoIndex = i;
			break;
		}
	}

	if (videoIndex == -1) {
		log_s("Didn't find a video stream.");
		return -1;
	}
	//6. 获取解码器并打开
	pCodecCtx = avcodec_alloc_context3(NULL);
	if (avcodec_parameters_to_context(pCodecCtx, pFmtCtx->streams[videoIndex]->codecpar) < 0) {
		log_s("Didn't parameters to contex.");
		return -1;
	}
	AVCodec* pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {
		log_s("Codec not found.");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {//打开解码器
		log_s("Could not open codec.");
		return -1;
	}
	//7. 解码播放开始准备工作
	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();

	//根据需要解码的类型，获取需要的buffer，不要忘记free
	outBuffer = (uint8_t*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1) * sizeof(uint8_t));
	//根据指定的图像参数和提供的数组设置数据指针和行数 ，数据填充到对应的pFrameYUV里面
	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, outBuffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

	pPacket = av_packet_alloc();
	log_s("--------------- File Information ----------------");
	av_dump_format(pFmtCtx, 0, filepath, 0);
	log_s("-------------------------------------------------");
	//获取SwsContext
	pSwsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, NULL, NULL, NULL, NULL);
	//----------------------------------------------------------------------------------------------------------------
	// 8. SDL相关初始化
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		log_s("Could not initialize SDL - ");
		log_s(SDL_GetError());
		return -1;
	}
	screen_w = pCodecCtx->width;
	screen_h = pCodecCtx->height;

	//======================== 添加 ========================
	/*screen = SDL_CreateWindow("WS ffmpeg player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		pCodecCtx->width / 2, pCodecCtx->height / 2, SDL_WINDOW_OPENGL);
	*/
	screen = SDL_CreateWindowFrom(dlg->GetDlgItem(IDC_PICTURE_PLAY)->GetSafeHwnd());
	
	if (!screen) {
		log_s("SDL: could not create window - exiting");
		log_s(SDL_GetError());
		return -1;
	}

	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);

	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = screen_w;
	sdlRect.h = screen_h;

	video_tid = SDL_CreateThread(sfp_refresh_thread, NULL, NULL);
	//----------------------------------------------------------------------------------------------------------------


	int count = 0;

	//9.读取数据播放
	for (;;) {
		//Wait
		SDL_WaitEvent(&event);
		if (event.type == SFM_REFRESH_EVENT) {
			if (av_read_frame(pFmtCtx, pPacket) == 0) {
				if (pPacket->stream_index == videoIndex) {
					if (avcodec_send_packet(pCodecCtx, pPacket) != 0) {//解码一帧压缩数据
						log_s("Decode end or Error.");
						break;
					}
					else {//处理解码数据
						avcodec_receive_frame(pCodecCtx, pFrame);

						if (sws_scale(pSwsCtx, (const uint8_t * const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
							pFrameYUV->data, pFrameYUV->linesize) == 0) {
							continue;
						}

						count++;

						//SDL播放---------------------------
						SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
						SDL_RenderClear(sdlRenderer);
						//SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect);  
						SDL_RenderCopy(sdlRenderer, sdlTexture, &sdlRect, NULL);
						SDL_RenderPresent(sdlRenderer);
						//SDL End-----------------------

						log_s("Succeed to play frame!", count);
					}
				}
			}
			else {
				//退出线程
				thread_exit = 1;
				av_packet_unref(pPacket);
			}
		}
		else if (event.type == SDL_KEYDOWN) {
			log_s("Pause");
			//Pause
			if (event.key.keysym.sym == SDLK_SPACE)
				thread_pause = !thread_pause;
		}
		else if (event.type == SDL_QUIT) {
			log_s("quit");
			thread_exit = 1;
			break;
		}
		else if (event.type == SFM_BREAK_EVENT) {
			log_s("break");
			break;
		}
	}
	//sdl退出
	SDL_Quit();

	//回收
	if (pSwsCtx != NULL) {
		sws_freeContext(pSwsCtx);
	}
	if (outBuffer != NULL) {
		av_free(outBuffer);
	}
	if (pFrameYUV != NULL) {
		av_frame_free(&pFrameYUV);
	}
	if (pFrame != NULL) {
		av_frame_free(&pFrame);
	}
	if (pCodecCtx != NULL) {
		avcodec_close(pCodecCtx);
	}
	if (pFmtCtx != NULL) {
		avformat_close_input(&pFmtCtx);
	}
}

//**************************************************************

// CJiaohuDlg 消息处理程序


void CJiaohuDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	// 设置过滤器   
	TCHAR szFilter[] = _T("All files(*.*)|*.*|AVI file(*.avi)|*.avi|wmv file(*.wmv)|*.wmv|asf file(*.asf)|*.asf|mpg file(*.mpg)|*.mpg||");

	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, NULL, NULL, 0, szFilter, this);
	CString strFolderPath;

	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		strFolderPath = fileDlg.GetFolderPath();
		m_video_file_path = fileDlg.GetPathName();

		//SetDlgItemText(IDC_EDIT_FEATURE, strFolderPath);
		m_feature_folder_path.SetWindowText(strFolderPath);
	}

	/*if (play_thread) {
		WaitForSingleObject(play_thread->m_hThread, INFINITE);
	}*/

	play_thread = AfxBeginThread(videoplayer, this);
	GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText((CString)"停止");
}


void CJiaohuDlg::OnStnClickedPicturePlay()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CJiaohuDlg::OnBnClickedButtonPlay()
{
	// TODO: 在此添加控件通知处理程序代码

	if (play_thread) {
		thread_pause = !thread_pause;
		if (thread_pause) {
			GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("播放"));
		}
		else
		{
			GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("停止"));
		}
	}
}


void CJiaohuDlg::OnBnClickedButtonSetFolder()
{
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO bi;// 选择路径的对话框
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = _T("请选择文件夹");
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.iImage = 0;

	//调用显示选择对话框，并返回选定的路径（格式为LPCITEMIDLIST）
	//注意下 这个函数会分配内存 但不会释放 需要手动释放
	LPCITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl)// 若为空，则说明上面的函数调用错误
	{
		TCHAR folderName[MAX_PATH];
		//将文件夹路径放到folderName中
		SHGetPathFromIDList(pidl, folderName);

		m_feature_folder_path.SetWindowText(folderName);
	}
}


void CJiaohuDlg::OnBnClickedButtonOpenFolder()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPath;
	GetDlgItem(IDC_EDIT_FEATURE)->GetWindowText(strPath);
	ShellExecute(NULL, NULL, _T("explorer"), strPath, NULL, SW_SHOW);
}


void CJiaohuDlg::OnBnClickedButtonPlaySlowly()
{
	// TODO: 在此添加控件通知处理程序代码
	thread_pause = 0;

	if (!is_playing_slowly) {
		is_playing_slowly = 1;
		is_playing_fast = 0;

		GetDlgItem(IDC_BUTTON_PLAY_SLOWLY)->SetWindowText(_T("正常"));
		GetDlgItem(IDC_BUTTON_PLAY_FAST)->SetWindowText(_T("快放"));
	}
	else
	{
		is_playing_slowly = 0;
		is_playing_fast = 0;

		GetDlgItem(IDC_BUTTON_PLAY_SLOWLY)->SetWindowText(_T("慢放"));
		GetDlgItem(IDC_BUTTON_PLAY_FAST)->SetWindowText(_T("快放"));
	}
}


void CJiaohuDlg::OnBnClickedButtonPlayFast()
{
	// TODO: 在此添加控件通知处理程序代码
	thread_pause = 0;

	if (!is_playing_fast) {
		is_playing_slowly = 0;
		is_playing_fast = 1;

		GetDlgItem(IDC_BUTTON_PLAY_FAST)->SetWindowText(_T("正常"));
		GetDlgItem(IDC_BUTTON_PLAY_SLOWLY)->SetWindowText(_T("慢放"));
	}
	else
	{
		is_playing_slowly = 0;
		is_playing_fast = 0;

		GetDlgItem(IDC_BUTTON_PLAY_SLOWLY)->SetWindowText(_T("慢放"));
		GetDlgItem(IDC_BUTTON_PLAY_FAST	)->SetWindowText(_T("快放"));
	}
}


void CJiaohuDlg::OnBnClickedButtonPlayFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	thread_pause = 1;
	GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("播放"));
	GetDlgItem(IDC_BUTTON_PLAY_SLOWLY)->SetWindowText(_T("慢放"));
	GetDlgItem(IDC_BUTTON_PLAY_FAST)->SetWindowText(_T("快放"));

	is_playing_frame = 1;
}
