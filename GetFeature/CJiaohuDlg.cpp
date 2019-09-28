// CJiaohuDlg.cpp: 实现文件
//

#include "pch.h"
#include "GetFeature.h"
#include "CJiaohuDlg.h"
#include "afxdialogex.h"
#include "SaveBmp.h"
#include "GetFeatureDlg.h"
#include "CTezhengDlg.h"
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include "CKindDlg.h"
#pragma comment(lib,"gdiplus.lib")

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
	DDX_Control(pDX, IDC_LIST_VIDEOCLIP, m_listbox_videoclip);
	DDX_Control(pDX, IDC_TEXT_TIMELENGTH, m_timelength);
	DDX_Control(pDX, IDC_SLIDER_SEEK, m_slider_seek);
	DDX_Control(pDX, IDC_TEXT_DESCRIBE, m_describe);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_COMBOX_FEATURE, m_combox_feature);
	DDX_Control(pDX, IDC_LIST_FRAMES, m_listbox_frame);
	DDX_Control(pDX, IDC_JIAORADIAO1, m_jradiao1);
	DDX_Control(pDX, IDC_JIAOHURADIAO2, m_jradiao2);
}


// FOR TEST
BEGIN_MESSAGE_MAP(CJiaohuDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CJiaohuDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CJiaohuDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_SET_FOLDER, &CJiaohuDlg::OnBnClickedButtonSetFolder)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FOLDER, &CJiaohuDlg::OnBnClickedButtonOpenFolder)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_SLOWLY, &CJiaohuDlg::OnBnClickedButtonPlaySlowly)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_FAST, &CJiaohuDlg::OnBnClickedButtonPlayFast)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_FRAME, &CJiaohuDlg::OnBnClickedButtonPlayFrame)
	ON_WM_HSCROLL()
	ON_LBN_DBLCLK(IDC_LIST_VIDEOCLIP, &CJiaohuDlg::OnLbnDblclkListVideoclip)
	ON_BN_CLICKED(IDC_BUTTON_CUTVIDEO, &CJiaohuDlg::OnBnClickedButtonCutvideo)
	ON_BN_CLICKED(IDC_BUTTON_GETTIME, &CJiaohuDlg::OnBnClickedButtonGettime)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_DELCLIP, &CJiaohuDlg::OnBnClickedButtonDelclip)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_FEATUREEXTRACT, &CJiaohuDlg::OnBnClickedButtonFeatureextract)
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_LIST_FRAMES, &CJiaohuDlg::OnLbnSelchangeListFrames)
	ON_BN_CLICKED(IDC_BUTTON_DELEFRAME, &CJiaohuDlg::OnBnClickedButtonDeleframe)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CJiaohuDlg::OnBnClickedButtonSave)
	
	ON_BN_CLICKED(IDC_BUTTON_QUICK, &CJiaohuDlg::OnBnClickedButtonQuick)
	ON_BN_CLICKED(IDC_BUTTON_JIAOCONFIG, &CJiaohuDlg::OnBnClickedButtonJiaoconfig)
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

// 全局信号量
bool normal_stop = true;//正常播放结束
bool thread_pause = false;
bool thread_exit = false;
int to_stop = 0;
int videoIndex = -1;
int is_playing_fast = 0;
int is_playing_slowly = 0;
int is_playing_frame = 0;
int is_showpicture = 0;
double alltime;
CString time_display;
CString total_time;
CString curr_time;
int cur_hours, cur_mins, cur_secs, cur_pos;
double sec;
double last_sec = -1;
int m_dbFrameRate;
int m_dbFrameNum;
double m_video_dura;
int m_slider_pos;
double          seek_time;        //要移动的时间（秒）
int             seek_req;         //seek的标志 是否需要seek
int             seek_flags;       //seek的方式 AVSEEK_FLAG_FRAME等
int64_t         seek_pos;         //seek过后的时间
int clipindex = 0;
bool start_or_end = true;//true为开始时间，false为结束时间
bool con_add = true;//继续添加虚线
vector<int> timeclips;
int keyframe_index;
int timeclips_size;
CRect rect;
CWnd* pwnd;
DWORD result;
int nFrame = 0;//特征帧数

int CJiaohuDlg::SplitString(LPCTSTR lpszStr, LPCTSTR lpszSplit, CStringArray& rArrString, BOOL bAllowNullString)
{
	rArrString.RemoveAll();
	CString szStr = lpszStr;
	szStr.TrimLeft();
	szStr.TrimRight();
	if (szStr.GetLength() == 0)
	{
		return 0;
	}
	CString szSplit = lpszSplit;
	if (szSplit.GetLength() == 0)
	{
		rArrString.Add(szStr);
		return 1;
	}
	CString s;
	int n;
	do {
		n = szStr.Find(szSplit);
		if (n > 0)
		{
			rArrString.Add(szStr.Left(n));
			szStr = szStr.Right(szStr.GetLength() - n - szSplit.GetLength());
			szStr.TrimLeft();
		}
		else if (n == 0)
		{
			if (bAllowNullString)
				rArrString.Add(_T(""));
			szStr = szStr.Right(szStr.GetLength() - szSplit.GetLength());
			szStr.TrimLeft();
		}
		else
		{
			if ((szStr.GetLength() > 0) || bAllowNullString)
				rArrString.Add(szStr);
			break;
		}
	} while (1);
	return rArrString.GetSize();
}

int sfp_refresh_thread(void *opaque) {
	CJiaohuDlg* pDlg = (CJiaohuDlg*)opaque;
	while (!thread_exit) {
		if (!thread_pause) {
			SDL_Event event;
			event.type = SFM_REFRESH_EVENT;
			
			SDL_PushEvent(&event);
			//慢放
			if (is_playing_slowly) {
				SDL_Delay(m_video_dura * 1000);
			}
			//快放
			else if (is_playing_fast)
			{
				
				SDL_Delay(m_video_dura * 250);
				
			}
			//正常播放
			else {
			SDL_Delay(m_video_dura*500);
			}

		}
		//若为单帧播放
		 else if(is_playing_frame)
		{
			SDL_Event event;
			event.type = SFM_REFRESH_EVENT;
			SDL_PushEvent(&event);
			is_playing_frame = 0;

		}
		 else if (is_showpicture) {
			SDL_Event event;
			event.type = SFM_SHOWPICTURE_EVENT;
			SDL_PushEvent(&event);
			is_showpicture = 0;
		}

	}
	if (normal_stop) {
	SDL_Event event;
	event.type = SFM_BREAK_EVENT;
	SDL_PushEvent(&event);
	}
	thread_exit = false;
	thread_pause = false;
	normal_stop = true;
	return 0;
}
int CJiaohuDlg::get_allframes() {
	
		AVFormatContext* fepFmtCtx = NULL;
		AVCodecContext* fepCodecCtx = NULL;
		AVCodec* fepCodec = NULL;
	
		USES_CONVERSION;
		char* sourceFile = W2A(VideoFilepath);

		
		//注册库中含有的所有可用的文件格式和编码器，这样当打开一个文件时，它们才能够自动选择相应的文件格式和编码器。
		av_register_all();
	
		int ret;
		// 打开视频文件
		if ((ret = avformat_open_input(&fepFmtCtx, sourceFile, NULL, NULL)) != 0) {
			log_s("Can't open file when feature extracting");
			return -1;
		}
		// 取出文件流信息
		if (avformat_find_stream_info(fepFmtCtx, NULL) < 0) {
			log_s("Can't find suitable codec parameters when feature extracting");
			return -1;
		}
	
		//仅仅处理视频流
		//只简单处理我们发现的第一个视频流
		//  寻找第一个视频流
		int videoIndex = -1;
		for (int i = 0; i < fepFmtCtx->nb_streams; i++) {
			if (fepFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
				videoIndex = i;
				break;
			}
		}
		if (-1 == videoIndex) {
			log_s("Can't find video stream when feature extracting");
			return -1;
		}
		// 得到视频流编码上下文的指针
		fepCodecCtx = fepFmtCtx->streams[videoIndex]->codec;
		//  寻找视频流的解码器
		fepCodec = avcodec_find_decoder(fepCodecCtx->codec_id);
	
		if (NULL == fepCodec) {
			log_s("Can't decode when feature extracting");
			return -1;
		}
	
		// 通知解码器我们能够处理截断的bit流，bit流帧边界可以在包中
		//视频流中的数据是被分割放入包中的。因为每个视频帧的数据的大小是可变的，
		//那么两帧之间的边界就不一定刚好是包的边界。这里，我们告知解码器我们可以处理bit流。
		if (fepCodec->capabilities & AV_CODEC_CAP_TRUNCATED) {
			fepCodecCtx->flags |= AV_CODEC_CAP_TRUNCATED;
		}
	
	
		//打开解码器
		if (avcodec_open2(fepCodecCtx, fepCodec, NULL) != 0) {
			log_s("Decode end or Error when feature extracting.");
			return -1;
		}
		AVPacket *InPack;
		InPack = av_packet_alloc();
		int len = 0;

		int nComplete = 0;
		
		CString frame_num;
		while ((av_read_frame(fepFmtCtx, InPack) >= 0)) {
			
			if (InPack->stream_index == videoIndex) {
				if (avcodec_send_packet(fepCodecCtx, InPack) != 0) {//解码一帧压缩数据
					log_s("Decode end or Error.");
					break;
				}
				else {//处理解码数据
					AVFrame* OutFrame;
					OutFrame = av_frame_alloc();
					avcodec_receive_frame(fepCodecCtx, OutFrame);
					frames.push_back(OutFrame);
					frame_num.Format(_T("frame%d"), nFrame);
					m_listbox_frame.AddString(frame_num);
					nFrame++;
				}
			}

		}

		avcodec_close(fepCodecCtx);
		av_free(fepFmtCtx);

}
UINT videoplayer(LPVOID lpParam) {
	CJiaohuDlg* pDlg = (CJiaohuDlg*)lpParam;
	last_sec = -1;
	thread_exit = false;
	sec = 0;
	
	thread_pause = false;
	AVFormatContext* pFmtCtx = NULL;
	AVCodecContext* pCodecCtx = NULL;
	AVCodec* pCodec = NULL;
	AVStream* video_st;
	AVFrame* pFrame = NULL;
	AVFrame* pFrameYUV = NULL;
	uint8_t* outBuffer = NULL;
	AVPacket* pPacket = NULL;
	SwsContext* pSwsCtx = NULL;
	
	//SDL
	
	SDL_Window* screen = NULL;
	SDL_Renderer* sdlRenderer = NULL;
	SDL_Texture* sdlTexture = NULL;
	SDL_Rect sdlRect;
	SDL_Thread* video_tid = NULL;
	SDL_Event event;
	
	//======================== 添加 ========================

	USES_CONVERSION;
	char* filepath = W2A(pDlg->VideoFilepath);

	//1. 初始化
	av_register_all();
	avformat_network_init();
	//2. AVFormatContext获取
	pFmtCtx = avformat_alloc_context();
	//3. 打开文件
	if (avformat_open_input(&(pFmtCtx), filepath, NULL, NULL) != 0) {
		log_s("Couldn't open input stream.\n");
		return -1;
	}
	//4. 获取文件信息
	if (avformat_find_stream_info(pFmtCtx, NULL) < 0) {
		log_s("Couldn't find stream information.");
		return -1;
	}
	
	//5. 获取视频的index
	int i = 0;
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
	video_st = pFmtCtx->streams[videoIndex];
	alltime = (double)(pFmtCtx->duration) / AV_TIME_BASE;//时长
	pDlg->m_slider_seek.alltime = alltime;
	int secs = (int)alltime % 60;
	int mins = (int)alltime / 60 % 60;
	int hours = (int)alltime / 60 / 60;
	total_time.Format(_T("%.2d : %.2d : %.2d"), hours, mins, secs);
	pDlg->TimeLength.Format(_T("00 : 00 : 00/%s"), total_time); // format the output
	pDlg->m_timelength.SetWindowText(pDlg->TimeLength);
	pDlg->m_timelength.ShowWindow(SW_SHOW);

	pDlg->m_slider_seek.SetRange(0, alltime*10);
	pDlg->m_slider_seek.SetTicFreq(1);

	if (video_st->r_frame_rate.den > 0)
		m_dbFrameRate = av_q2d(video_st->r_frame_rate);//avStream->r_frame_rate.num / avStream->r_frame_rate.den;
	else if (video_st->codec->framerate.den > 0)
		m_dbFrameRate = av_q2d(video_st->codec->framerate);//m_pvCodecCtxOrg->framerate.num / m_pvCodecCtxOrg->framerate.den;

	m_dbFrameNum = video_st->nb_frames;
	if (m_dbFrameNum == 0)
	{
		m_dbFrameNum = alltime * m_dbFrameRate;
	}
	m_video_dura = alltime /m_dbFrameNum;

	CString szSplit = _T("\\");
	CStringArray szList;
	int Count = pDlg->SplitString(pDlg->VideoFilepath, szSplit, szList, FALSE);
	pDlg->VideoFilename = szList.GetAt(Count - 1);

	if (pDlg->newvideo){
		pDlg->m_listbox_videoclip.AddString(pDlg->VideoFilename);
	}

	CString szSplit2 = _T(".");
	CStringArray szList2;
	int Count2 = pDlg->SplitString(pDlg->VideoFilename, szSplit2, szList2, FALSE);
	pDlg->suffix = szList2.GetAt(Count2 - 1);
	int n = pDlg->VideoFilename.ReverseFind('.');
	pDlg->VideoFilename_nosuffix = pDlg->VideoFilename.Left(n);

	CString szSplit3 = _T("-");
	CStringArray szList3;
	int Count3 = pDlg->SplitString(pDlg->VideoFilename_nosuffix, szSplit3, szList3, FALSE);
	CString flag = szList3.GetAt(Count3 - 1);
	if (flag == _T("FD")) {
		pDlg->get_allframes();
	}
	else if (flag == _T("JC")) {
		pDlg->get_allframes();
	}

	//6. 获取解码器并打开
	pCodecCtx = avcodec_alloc_context3(NULL);
	
	if (avcodec_parameters_to_context(pCodecCtx, pFmtCtx->streams[videoIndex]->codecpar) < 0) {
		log_s("Didn't parameters to contex.");
		return -1;
	}
	pDlg->pixfmt = pCodecCtx->pix_fmt;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
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
	pSwsCtx = sws_getContext(pCodecCtx->width,pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width,pCodecCtx->height, AV_PIX_FMT_YUV420P, NULL, NULL, NULL, NULL);
	//----------------------------------------------------------------------------------------------------------------
	// 8. SDL相关初始化
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		log_s("Could not initialize SDL - ");
		log_s(SDL_GetError());
		return -1;
	}
	pDlg->screen_w = pCodecCtx->width;
	pDlg->screen_h = pCodecCtx->height;
	pDlg->isexcut_black = false;
	screen = SDL_CreateWindowFrom(pDlg->GetDlgItem(IDC_PICTURE_PLAY)->GetSafeHwnd());

	if (!screen) {
		return -1;
	}
	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,pCodecCtx->width,pCodecCtx->height);

	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = pDlg->screen_w;
	sdlRect.h = pDlg->screen_h;
	video_tid = SDL_CreateThread(sfp_refresh_thread,NULL, (void*)pDlg);

	//9.读取数据播放
	for (;;) {		
		if (to_stop) {
			to_stop = 0;
			pDlg->m_slider_seek.SetPos(0);
			//SDL_Quit();
			//pDlg->GetDlgItem(IDC_PICTURE_PLAY)->ShowWindow(SW_SHOWNORMAL);
			thread_exit = true;
			Sleep(100);
			break;
		}	
		//Wait
		SDL_WaitEvent(&event);
		if (event.type == SFM_REFRESH_EVENT) {

			if (seek_req)
			{
				int stream_index = -1;
				
				//转化成纳秒
				int64_t seek_target = seek_pos * 1000000;
				stream_index = videoIndex;
				if (stream_index >= 0)
				{
					AVRational time_base_q;
					time_base_q.num = 1;
					time_base_q.den = AV_TIME_BASE;
					//这里一定要注意：不单纯的是从秒转成毫秒，//seek_target = seek_target / 1000; 这样做是不对的
					seek_target = av_rescale_q(seek_target, time_base_q, pFmtCtx->streams[stream_index]->time_base);
				}
				int error = av_seek_frame(pFmtCtx, stream_index, seek_target, seek_flags);
				last_sec = -1;
				seek_req = 0;
			}

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
						int pts = pPacket->pts > 0 ? pPacket->pts : pPacket->dts;
						
						//当前帧显示时间
						sec = pts * av_q2d(video_st->time_base);
						if (sec > last_sec) {
							cur_pos = sec*10;
							cur_secs = sec;
							cur_mins = cur_secs / 60;
							cur_secs %= 60;
							cur_hours = cur_mins / 60;
							cur_mins %= 60;
							curr_time.Format(_T("%.2d : %.2d : %.2d"), cur_hours, cur_mins, cur_secs);
							pDlg->TimeLength.Format(_T("%s/%s"), curr_time, total_time);
							pDlg->m_timelength.SetWindowText(pDlg->TimeLength);
							pDlg->m_slider_seek.SetPos(cur_pos);
							last_sec = sec;
						}

						//SDL播放---------------------------
						SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
						SDL_RenderClear(sdlRenderer);
						//SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect);  
						SDL_RenderCopy(sdlRenderer, sdlTexture, &sdlRect, NULL);
						SDL_RenderPresent(sdlRenderer);
					
					}
				}
			}
			else {
				//退出线程
				//thread_exit = true;
				thread_pause = true;
				av_packet_unref(pPacket);
				//AfxMessageBox(_T("播放结束"));
			}
		}
		else if (event.type == SFM_SHOWPICTURE_EVENT) {
			
			pFrame = pDlg->frames[keyframe_index];
			if (sws_scale(pSwsCtx, (const uint8_t * const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
				pFrameYUV->data, pFrameYUV->linesize) == 0) {
				continue;
			}

			SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
			SDL_RenderClear(sdlRenderer);
			//SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect);  
			SDL_RenderCopy(sdlRenderer, sdlTexture, &sdlRect, NULL);
			SDL_RenderPresent(sdlRenderer);
		}
		else if (event.type == SDL_QUIT) {
			log_s("quit");
			thread_exit = true;
			break;
		}
		else if (event.type == SFM_BREAK_EVENT) {
			log_s("break");
			is_playing_fast = 0;
			is_playing_slowly = 0;
			is_playing_frame = 0;
			pDlg->play_thread = NULL;
			pDlg->isexcut_black = true;
			pDlg->GetDlgItem(IDC_BUTTON_GETTIME)->SetWindowText(_T("开始位置"));
			pDlg->GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("停止"));
			pDlg->GetDlgItem(IDC_BUTTON_GETTIME)->EnableWindow(0);
			pDlg->GetDlgItem(IDC_BUTTON_DELCLIP)->EnableWindow(0);
			pDlg->GetDlgItem(IDC_BUTTON_CUTVIDEO)->EnableWindow(0);
			pDlg->GetDlgItem(IDC_BUTTON_CLIPEXT)->EnableWindow(0);
			break;
		}
	}

	return 0;
}

//**************************************************************

// CJiaohuDlg 消息处理程序
void CJiaohuDlg::thread_stop() {
	to_stop = 1;
	normal_stop = false;
	is_playing_fast = 0;
	is_playing_slowly = 0;
	is_playing_frame = 0;
	isexcut_black = true;
}



void CJiaohuDlg::OnBnClickedButtonOpen()
{
	//CString TimeLength;

	// TODO: 在此添加控件通知处理程序代码
	// 设置过滤器   
	

	TCHAR szFilter[] = _T("All files(*.*)|*.*|AVI file(*.avi)|*.avi|wmv file(*.wmv)|*.wmv|asf file(*.asf)|*.asf|mpg file(*.mpg)|*.mpg||");

	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, NULL, NULL, 0, szFilter, this);

	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		strVideoFolderPath = fileDlg.GetFolderPath();
		VideoFilepath = fileDlg.GetPathName();

		if (fea_is_vid) {
			m_feature_folder_path.SetWindowText(strVideoFolderPath);
		}
		CGetFeatureDlg* pWnd = (CGetFeatureDlg*)AfxGetMainWnd();
		if (pWnd->m_piliangDlg.piplay_thread) {
			pWnd->m_piliangDlg.pithread_stop();
			MsgWaitForMultipleObjects(1, (HANDLE*)pWnd->m_piliangDlg.piplay_thread, FALSE, INFINITE, QS_ALLINPUT);
		}
		newvideo = true;
		if (play_thread) {
			thread_stop();	
			result = MsgWaitForMultipleObjects(1, (HANDLE*)play_thread, FALSE, INFINITE, QS_ALLINPUT);
		}
		
	
			Sleep(100);
			frames.swap(vector<AVFrame*>());
			timeclips.clear();
			m_slider_seek.timepos.clear();
			thread_pause = false;
			thread_exit = false;
			con_add = true;
			nFrame = 0;

			m_slider_seek.GetClientRect(&rect);
			play_thread = AfxBeginThread(videoplayer, this);
			m_listbox_videoclip.ResetContent();
			m_listbox_frame.ResetContent();
			GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText((CString)"停止");
			GetDlgItem(IDC_BUTTON_GETTIME)->SetWindowText(_T("开始位置"));
			GetDlgItem(IDC_BUTTON_FEATUREEXTRACT)->EnableWindow(1);

			m_describe.SetWindowText(_T("播放视频"));
			GetDlgItem(IDC_BUTTON_GETTIME)->EnableWindow(1);
			GetDlgItem(IDC_BUTTON_DELCLIP)->EnableWindow(0);
			GetDlgItem(IDC_BUTTON_CUTVIDEO)->EnableWindow(0);
			GetDlgItem(IDC_BUTTON_CLIPEXT)->EnableWindow(0);
			GetDlgItem(IDC_BUTTON_QUICK)->EnableWindow(1);
			GetDlgItem(IDC_BUTTON_JIAOCONFIG)->EnableWindow(1);
		
		
	}


}


void CJiaohuDlg::OnBnClickedButtonPlay()
{
	// TODO: 在此添加控件通知处理程序代码

	if (play_thread) {
		thread_pause = !thread_pause;
		
		if (thread_pause) {
			GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("播放"));
			
			m_describe.SetWindowText(_T("停止播放"));
		}
		else
		{
			GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("停止"));
			
			m_describe.SetWindowText(_T("播放视频"));
		}
	}
}

CString FicowGetDirectory()
{
	BROWSEINFO bi;
	char name[MAX_PATH];
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	//bi.pszDisplayName = name;
	bi.lpszTitle = _T("选择文件夹目录");
	bi.ulFlags = BIF_RETURNFSANCESTORS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (idl == NULL)
		return _T("");
	CString strDirectoryPath;
	SHGetPathFromIDList(idl, strDirectoryPath.GetBuffer(MAX_PATH));
	strDirectoryPath.ReleaseBuffer();
	if (strDirectoryPath.IsEmpty())
		return _T("");
	if (strDirectoryPath.Right(1) != "\\")
		strDirectoryPath += "\\";

	return strDirectoryPath;
}
void CJiaohuDlg::OnBnClickedButtonSetFolder()
{
	fea_is_vid = false;
	strFeatureFolderPath = FicowGetDirectory();
	m_feature_folder_path.SetWindowText(strFeatureFolderPath);
}


void CJiaohuDlg::OnBnClickedButtonOpenFolder()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(NULL, _T("open"), strFeatureFolderPath, NULL, NULL, SW_SHOWNORMAL);
}


void CJiaohuDlg::OnBnClickedButtonPlaySlowly()
{
	// TODO: 在此添加控件通知处理程序代码
	//thread_pause = 0;

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
	if (thread_pause)
	{
		//播放
		GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("停止"));
		thread_pause = !thread_pause;
	}
}


void CJiaohuDlg::OnBnClickedButtonPlayFast()
{
	// TODO: 在此添加控件通知处理程序代码
	

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
	if (thread_pause)
	{
		//播放
		GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("停止"));
		thread_pause = !thread_pause;
	}
}


void CJiaohuDlg::OnBnClickedButtonPlayFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	thread_pause = true;
	GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("播放"));
	GetDlgItem(IDC_BUTTON_PLAY_SLOWLY)->SetWindowText(_T("慢放"));
	GetDlgItem(IDC_BUTTON_PLAY_FAST)->SetWindowText(_T("快放"));
	is_playing_frame = 1;
}

void CJiaohuDlg::OnLbnSelchangeListFrames()
{
	// TODO: 在此添加控件通知处理程序代码

	int num = m_listbox_frame.GetCount();
	if (num != 0) {
		keyframe_index = m_listbox_frame.GetCurSel();
		if (keyframe_index != LB_ERR) {
			thread_pause = true;
			is_showpicture = 1;	
		}
	}
	
}
void CJiaohuDlg::OnBnClickedButtonDeleframe()
{
	// TODO: 在此添加控件通知处理程序代码
	jiaoneedsave = true;
	frames.erase(frames.begin()+keyframe_index);
	m_listbox_frame.DeleteString(keyframe_index);
	nFrame--;
	thread_pause = true;
	is_showpicture = 1;
}
BOOL CJiaohuDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	get_control_original_proportion();
	Gdiplus::GdiplusStartup(&m_pGdiToken, &m_pGdiplusStartupInput, NULL);
	GetDlgItem(IDC_BUTTON_GETTIME)->EnableWindow(0);
	GetDlgItem(IDC_BUTTON_DELCLIP)->EnableWindow(0);
	GetDlgItem(IDC_BUTTON_CUTVIDEO)->EnableWindow(0);
	GetDlgItem(IDC_BUTTON_CLIPEXT)->EnableWindow(0);
	GetDlgItem(IDC_BUTTON_FEATUREEXTRACT)->EnableWindow(0);
	GetDlgItem(IDC_PROGRESS)->ShowWindow(0);
	m_progress.SetRange(0, 100);
	m_combox_feature.SetCurSel(0);
	font.CreatePointFont(120, _T("新宋体"));
	m_listbox_videoclip.SetFont(&font);
	m_listbox_frame.SetFont(&font);
	m_timelength.SetFont(&font);
	m_describe.SetFont(&font);
	m_jradiao1.SetCheck(true);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CJiaohuDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_slider_pos = m_slider_seek.GetPos();
	if (pScrollBar->GetSafeHwnd() == m_slider_seek.GetSafeHwnd())        //如果是 文件播放进度条
	{
		if (play_thread!=NULL){
			seek_req = 1;
			seek_pos = m_slider_pos/10;
			seek_flags = AVSEEK_FLAG_FRAME;//m_streamstate->seek_time < 0 ? AVSEEK_FLAG_BACKWARD : 0;
			m_slider_pos = 0;
			Sleep(10);
			if (thread_pause) //如果正在暂停
				{
					//播放
					thread_pause = false;
				}
		}
		CRect rect;
		m_slider_seek.GetClientRect(&rect);
		InvalidateRect(rect);
		UpdateWindow();
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}



void CJiaohuDlg::OnLbnDblclkListVideoclip()
{
	// TODO: 在此添加控件通知处理程序代码

	
	//AfxMessageBox(_T("将要开启play线程"));
	int num = m_listbox_videoclip.GetCount();
	if (num != 0) {
		int nSel = m_listbox_videoclip.GetCurSel();
		if (nSel != LB_ERR) {
			newvideo = false;

			if (play_thread) {
				thread_stop();
				result = MsgWaitForMultipleObjects(1, (HANDLE*)play_thread, FALSE, INFINITE, QS_ALLINPUT);
			}
			Sleep(100);
			frames.swap(vector<AVFrame*>());
			timeclips.clear();
			m_slider_seek.timepos.clear();
			thread_pause = false;
			thread_exit = false;
			con_add = true;	
			nFrame = 0;
			m_slider_seek.GetClientRect(&rect);
			m_listbox_frame.ResetContent();
			CString s;
			m_listbox_videoclip.GetText(nSel, s);
			VideoFilepath = strVideoFolderPath + "\\" + s;
			play_thread = AfxBeginThread(videoplayer, this);
			GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText((CString)"停止");
			GetDlgItem(IDC_BUTTON_GETTIME)->SetWindowText(_T("开始位置"));
			GetDlgItem(IDC_BUTTON_GETTIME)->EnableWindow(1);
			GetDlgItem(IDC_BUTTON_DELCLIP)->EnableWindow(0);
			GetDlgItem(IDC_BUTTON_CUTVIDEO)->EnableWindow(0);
			GetDlgItem(IDC_BUTTON_CLIPEXT)->EnableWindow(0);
			m_describe.SetWindowText(_T("播放视频"));
		}
	}

}

void CJiaohuDlg::OnBnClickedButtonCutvideo()
{
	// TODO: 在此添加控件通知处理程序代码
	m_progress.ShowWindow(SW_SHOW);
	m_progress.RedrawWindow();
	m_progress.SetPos(100);
	if (!thread_pause) {
		thread_pause = !thread_pause;
		GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText((CString)"播放");
	}
	if (timeclips.size() % 2 == 1) {
		con_add = false;
		timeclips.pop_back();
		m_slider_seek.timepos.pop_back();

		CRect rect;
		//CWnd* pwnd = GetDlgItem(IDC_SLIDER_SEEK); // 取得控件的指针
		m_slider_seek.GetClientRect(&rect);
		m_slider_seek.InvalidateRect(rect);
		m_slider_seek.UpdateWindow();
	}
	timeclips_size = timeclips.size() / 2;
	CString desceibe;
	for (int i = 0; i < timeclips_size; i++) {
		clipindex++;
		CString str;
		str.Format(_T("%d"), clipindex);
		CString clipname = VideoFilename_nosuffix + "_" + str + "." + suffix;
		CString outfile = strVideoFolderPath + "//" + clipname;
		CString comm;
		int dura = timeclips[2 * i + 1] - timeclips[2 * i];
		if (dura <= 1) {
			AfxMessageBox(_T("视频无法切割"));
			continue;
		}

		comm.Format(_T("ffmpeg -ss %d -to %d  -accurate_seek -i %s -vcodec libx264 -acodec aac -avoid_negative_ts 1 %s -y"), timeclips[2 * i], timeclips[2 * i + 1], VideoFilepath, outfile);
		USES_CONVERSION;
		char* outcomm = W2A(comm);
		if (WinExec(outcomm, SW_HIDE)) {
			desceibe.Format(_T("%s号视频片段正在切割，请耐心等待！"),str);
			m_describe.SetWindowText(desceibe);
			m_listbox_videoclip.AddString(clipname);
			Sleep(600);
		}
			
	}	
	
	m_describe.SetWindowText(_T("视频切割完毕"));
	m_progress.ShowWindow(SW_HIDE);
	m_progress.RedrawWindow();
}


void CJiaohuDlg::OnBnClickedButtonGettime()
{
	// TODO: 在此添加控件通知处理程序代码
	
	if (!thread_pause) {
	thread_pause = !thread_pause;
	GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("播放"));
	}


	if (con_add) {
		timeclips.push_back((int)sec);
		m_slider_pos=m_slider_seek.GetPos();
		m_slider_seek.timepos.push_back(m_slider_pos);
		CRect rect;

		m_slider_seek.GetClientRect(&rect);
		m_slider_seek.InvalidateRect(rect);
		m_slider_seek.UpdateWindow();
	timeclips_size = timeclips.size();
	if (timeclips_size >= 2) {
		GetDlgItem(IDC_BUTTON_DELCLIP)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_CUTVIDEO)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_CLIPEXT)->EnableWindow(1);
	}


	if (start_or_end){
		GetDlgItem(IDC_BUTTON_GETTIME)->SetWindowText(_T("结束位置"));
		start_or_end = !start_or_end;
	}
	else {
		GetDlgItem(IDC_BUTTON_GETTIME)->SetWindowText(_T("开始位置"));
		start_or_end = !start_or_end;
	}
	
	
	}
}


void CJiaohuDlg::OnBnClickedButtonDelclip()
{
	// TODO: 在此添加控件通知处理程序代码
	if (timeclips.size() % 2 == 1) {
		timeclips.pop_back();
		m_slider_seek.timepos.pop_back();
	}

	if (timeclips.size() < 2) {
		GetDlgItem(IDC_BUTTON_DELCLIP)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_CUTVIDEO)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_CLIPEXT)->EnableWindow(0);
	}
	else
	{
		timeclips.pop_back();
		timeclips.pop_back();
		m_slider_seek.timepos.pop_back();
		m_slider_seek.timepos.pop_back();
	}
	if (timeclips.size() < 2) {
		GetDlgItem(IDC_BUTTON_DELCLIP)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_CUTVIDEO)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_CLIPEXT)->EnableWindow(0);
	}
	CRect rect;
	//CWnd* pwnd = GetDlgItem(IDC_SLIDER_SEEK); // 取得控件的指针
	m_slider_seek.GetClientRect(&rect);
	m_slider_seek.InvalidateRect(rect);
	m_slider_seek.UpdateWindow();
}
void CJiaohuDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	GdiplusShutdown(m_pGdiToken);
}


void CJiaohuDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (!GetSafeHwnd() == NULL) {
		CRect rect;// 获取当前窗口大小
		for (std::list<control*>::iterator it = m_con_list.begin(); it != m_con_list.end(); it++) {
			CWnd* pWnd = GetDlgItem((*it)->Id);//获取ID为woc的空间的句柄
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);//将控件大小转换为在对话框中的区域坐标
			rect.left = (*it)->scale[0] * cx;
			rect.right = (*it)->scale[1] * cx;
			rect.top = (*it)->scale[2] * cy;
			rect.bottom = (*it)->scale[3] * cy;
			pWnd->MoveWindow(rect);//设置控件大小
		}
		GetClientRect(&m_rect);//将变化后的对话框大小设为旧大小
	}
}
void CJiaohuDlg::get_control_original_proportion() {
	HWND hwndChild = ::GetWindow(m_hWnd, GW_CHILD);
	while (hwndChild)
	{
		CRect rect;//获取当前窗口的大小
		control* tempcon = new control;
		CWnd* pWnd = GetDlgItem(::GetDlgCtrlID(hwndChild));//获取ID为woc的空间的句柄
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);//将控件大小转换为在对话框中的区域坐标
		tempcon->Id = ::GetDlgCtrlID(hwndChild);//获得控件的ID;
		tempcon->scale[0] = (double)rect.left / m_rect.Width();//注意类型转换，不然保存成long型就直接为0了
		tempcon->scale[1] = (double)rect.right / m_rect.Width();
		tempcon->scale[2] = (double)rect.top / m_rect.Height();
		tempcon->scale[3] = (double)rect.bottom / m_rect.Height();
		m_con_list.push_back(tempcon);
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
}


void CJiaohuDlg::OnPaint()
{



	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	
	if (isexcut_black) {
		CRect rtTop;
		CWnd* pWnd = GetDlgItem(IDC_PICTURE_PLAY);
		CDC* cDc = pWnd->GetDC();
		pWnd->GetClientRect(&rtTop);
		cDc->FillSolidRect(rtTop.left, rtTop.top, rtTop.Width(), rtTop.Height(), RGB(0, 0, 0));
	}
	
}


#define STREAM_DURATION   10.0
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */
#define SCALE_FLAGS SWS_BICUBIC
#define AVFMT_RAWPICTURE   0x0020
typedef struct OutputStream { 
	AVStream* st;    /* pts of the next frame that will be generated */    
	int64_t next_pts;   
	int samples_count;   
	AVFrame* frame;  
	AVFrame* tmp_frame;   
	float t, tincr, tincr2;  
	struct SwsContext* sws_ctx;   
	//struct SwrContext* swr_ctx; 
} OutputStream;

static int write_frame(AVFormatContext* fmt_ctx, const AVRational* time_base, AVStream* st, AVPacket* pkt) {   
	/* rescale output packet timestamp values from codec to stream timebase */  
	av_packet_rescale_ts(pkt, *time_base, st->time_base);  
	pkt->stream_index = st->index;  
	/* Write the compressed frame to the media file. */    
	//log_packet(fmt_ctx, pkt);   
	return av_interleaved_write_frame(fmt_ctx, pkt); 
}
/* Add an output stream. */
static void add_stream(OutputStream* ost, AVFormatContext* oc, AVCodec** codec, enum AVCodecID codec_id,int keywidth,int keyheight) { 
	AVCodecContext* c;   
	int i;   
	/* find the encoder */  
	 *codec = avcodec_find_encoder(codec_id);  
	if (!(*codec)) { 
		fprintf(stderr, "Could not find encoder for '%s'\n", avcodec_get_name(codec_id));   
		exit(1);
	 }    
	ost->st = avformat_new_stream(oc, *codec);  
    if (!ost->st) { fprintf(stderr, "Could not allocate stream\n");  
      exit(1); 
	}  
	ost->st->id = videoIndex;   
    c = ost->st->codec;  
    switch ((*codec)->type) { 
		case AVMEDIA_TYPE_AUDIO:   
			c->sample_fmt = (*codec)->sample_fmts ? (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;    
			c->bit_rate = 64000;        
			c->sample_rate = 44100;        
			if ((*codec)->supported_samplerates) {
				c->sample_rate = (*codec)->supported_samplerates[0];
				for (i = 0; (*codec)->supported_samplerates[i]; i++) { 
					if ((*codec)->supported_samplerates[i] == 44100) 
						c->sample_rate = 44100; 
				} 
			}       
			c->channels = av_get_channel_layout_nb_channels(c->channel_layout);			
			c->channel_layout = AV_CH_LAYOUT_STEREO;        
			if ((*codec)->channel_layouts) {
				c->channel_layout = (*codec)->channel_layouts[0];
				for (i = 0; (*codec)->channel_layouts[i]; i++) { 
					if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
						c->channel_layout = AV_CH_LAYOUT_STEREO; 
				}
			}        
			c->channels = av_get_channel_layout_nb_channels(c->channel_layout);        
			ost->st->time_base = AVRational{ 1, c->sample_rate };
			break;    
		case AVMEDIA_TYPE_VIDEO:        
			c->codec_id = codec_id;        
			c->bit_rate = 400000;       
			/* Resolution must be a multiple of two. */       
			c->width = keywidth;      
			c->height = keyheight;     
			/* timebase: This is the fundamental unit of time (in seconds) in terms      
			 * of which frame timestamps are represented. For fixed-fps content,       
			 * timebase should be 1/framerate and timestamp increments should be       
			 * identical to 1. */     
			ost->st->time_base = AVRational{ 1,STREAM_FRAME_RATE };
			c->time_base = ost->st->time_base;  
			c->gop_size = 12; 
			/* emit one intra frame every twelve frames at most */      
			c->pix_fmt = STREAM_PIX_FMT;    
			if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {        
				/* just for testing, we also add B frames */   
				c->max_b_frames = 2; 
			}   
			if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {     
				/* Needed to avoid using macroblocks in which some coeffs overflow.    
				 * This does not happen with normal video, it just happens here as       
				 * the motion of the chroma plane does not match the luma plane. */     
				c->mb_decision = 2; 
			}    
			break;   
		default:    
			break; }   
	/* Some formats want stream headers to be separate. */   
	if (oc->oformat->flags & AVFMT_GLOBALHEADER)     
		c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

/**************************************************************/
/* video output */
static AVFrame* alloc_picture(enum AVPixelFormat pix_fmt, int width, int height) { 
	AVFrame* picture;   
	int ret;   
	picture = av_frame_alloc();  
	if (!picture)      
		return NULL;   
	picture->format = pix_fmt;  
	picture->width = width;  
	picture->height = height;  
	/* allocate the buffers for the frame data */   
	ret = av_frame_get_buffer(picture, 32);  
	if (ret < 0) {
		fprintf(stderr, "Could not allocate frame data.\n");     
		exit(1); 
	}   
	return picture; 
}
static void open_video(AVFormatContext* oc, AVCodec* codec, OutputStream* ost, AVDictionary* opt_arg) {
	int ret;   
	AVCodecContext* c = ost->st->codec;    
	AVDictionary* opt = NULL;    
	av_dict_copy(&opt, opt_arg, 0); 
	/* open the codec */   
	ret = avcodec_open2(c, codec, &opt);   
	av_dict_free(&opt);   
	if (ret < 0) { 
		AfxMessageBox(_T("123"));
		//fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));      
		exit(1); 
	}   
	/* allocate and init a re-usable frame */  
	ost->frame = alloc_picture(c->pix_fmt, c->width, c->height);   
	if (!ost->frame) {
		fprintf(stderr, "Could not allocate video frame\n");     
		exit(1); 
	}   
	/* If the output format is not YUV420P, then a temporary YUV420P   
	 * picture is needed too. It is then converted to the required    
	 * output format. */   
	ost->tmp_frame = NULL;  
	if (c->pix_fmt != AV_PIX_FMT_YUV420P) { 
		ost->tmp_frame = alloc_picture(AV_PIX_FMT_YUV420P, c->width, c->height);    
		if (!ost->tmp_frame) { 
			fprintf(stderr, "Could not allocate temporary picture\n");    
			exit(1);
		}
	} 
}
/* Prepare a dummy image. */
static void fill_yuv_image(AVFrame* pict, AVFrame* keyframe, int frame_index, int width, int height) {
	int x, y, i, ret; 
	/* when we pass a frame to the encoder, it may keep a reference to it   
	* internally;   
	* make sure we do not overwrite it here     */   
	ret = av_frame_make_writable(pict); 
	if (ret < 0)     
		exit(1);  
	i = frame_index; 
	pict->data[0] = keyframe->data[0];
	pict->data[1] = keyframe->data[1];
	pict->data[2] = keyframe->data[2];

}
static AVFrame* get_video_frame(OutputStream* ost, AVFrame* keyframe) {
	AVCodecContext* c = ost->st->codec;
	/* check if we want to generate more frames */  
	if (av_compare_ts(ost->next_pts, ost->st->codec->time_base, STREAM_DURATION, AVRational { 1, 1 }) >= 0)
		return NULL;    
	if (c->pix_fmt != AV_PIX_FMT_YUV420P) {     
		/* as we only generate a YUV420P picture, we must convert it      
		 * to the codec pixel format if needed */     
		if (!ost->sws_ctx) {
			ost->sws_ctx = sws_getContext(c->width, c->height, AV_PIX_FMT_YUV420P, c->width, c->height, c->pix_fmt, SCALE_FLAGS, NULL, NULL, NULL);    
			if (!ost->sws_ctx) {
				fprintf(stderr, "Could not initialize the conversion context\n");       
				exit(1); 
			} 
		}      
		fill_yuv_image(ost->tmp_frame, keyframe,ost->next_pts, c->width, c->height); 
		sws_scale(ost->sws_ctx, (const uint8_t * const*)ost->tmp_frame->data, ost->tmp_frame->linesize, 0, c->height, ost->frame->data, ost->frame->linesize);

		
	} else { 
		fill_yuv_image(ost->frame, keyframe,ost->next_pts, c->width, c->height);

	}   
	ost->frame->pts = ost->next_pts++;    
	return ost->frame; 
}


/* * encode one video frame and send it to the muxer 
 * return 1 when encoding is finished, 0 otherwise */
static int write_video_frame(AVFormatContext* oc, OutputStream* ost, AVFrame* keyframe) {
	int ret;    
	AVCodecContext* c; 
	AVFrame* frame;  
	int got_packet = 0; 
	c = ost->st->codec; 
	frame = get_video_frame(ost,keyframe);  

	if (oc->oformat->flags & AVFMT_RAWPICTURE) {   
		/* a hack to avoid data copy with some raw video muxers */  
		AVPacket pkt;   
		av_init_packet(&pkt);   
		if (!frame)   
			return 1;    
		pkt.flags |= AV_PKT_FLAG_KEY;     
		pkt.stream_index = ost->st->index;   
		pkt.data = (uint8_t*)frame;     
		pkt.size = sizeof(AVPicture);    
		pkt.pts = pkt.dts = frame->pts;    
		av_packet_rescale_ts(&pkt, c->time_base, ost->st->time_base);    
		ret = av_interleaved_write_frame(oc, &pkt); 
	} else {
		AVPacket pkt = { 0 };     
		av_init_packet(&pkt);   
		/* encode the image */   
		ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);   
		if (ret < 0) {
			//fprintf(stderr, "Error encoding video frame: %s\n", av_err2str(ret));   
			exit(1); 
		}     
		if (got_packet) {
			ret = write_frame(oc, &c->time_base, ost->st, &pkt); 
		} else { 
			ret = 0; 
		} 
	}    
	if (ret < 0) {
		//fprintf(stderr, "Error while writing video frame: %s\n", av_err2str(ret));  
		exit(1); 
	}  
	return (frame || got_packet) ? 0 : 1;
}
static void close_stream(AVFormatContext* oc, OutputStream* ost) { 
	avcodec_close(ost->st->codec);  
	av_frame_free(&ost->frame); 
	av_frame_free(&ost->tmp_frame);
	sws_freeContext(ost->sws_ctx);  
	//swr_free(&ost->swr_ctx); 
}

int CJiaohuDlg::JEeature_Extract(int kind1,int kind2) {
	//CJiaohuDlg* pDlg = (CJiaohuDlg*)lpParam;
	CString ckind1;
	CString ckind2;
	if (kind1 == 1) {
		ckind1 = "-X5";
		m_describe.SetWindowText(_T("正在提取X5特效..."));
		CRect rect;
		m_describe.GetClientRect(&rect);
		InvalidateRect(rect);
		UpdateWindow();
	}
	if (kind1 == 2) {
		ckind1 = "-YT";
		m_describe.SetWindowText(_T("正在提取YT特效..."));
		CRect rect;
		m_describe.GetClientRect(&rect);
		InvalidateRect(rect);
		UpdateWindow();
	}
	if (kind2 == 1) {
		ckind2 = "-FD";
	}
	if (kind2 == 2) {
		ckind2 = "-JC";
	}
	int ret;
	AVFormatContext* fepFmtCtx = NULL;
	AVCodecContext* fepCodecCtx = NULL;
	AVCodec* fepCodec = NULL;
	USES_CONVERSION;
	char* sourceFile = W2A(VideoFilepath);

	// 将提取特征按钮disable
	GetDlgItem(IDC_BUTTON_FEATUREEXTRACT)->EnableWindow(0);

	//注册库中含有的所有可用的文件格式和编码器，这样当打开一个文件时，它们才能够自动选择相应的文件格式和编码器。
	av_register_all();

	// 打开视频文件
	if ((ret = avformat_open_input(&fepFmtCtx, sourceFile, NULL, NULL)) != 0) {
		log_s("Can't open file when feature extracting");
		return -1;
	}
	// 取出文件流信息
	if (avformat_find_stream_info(fepFmtCtx, NULL) < 0) {
		log_s("Can't find suitable codec parameters when feature extracting");
		return -1;
	}

	//仅仅处理视频流
	//只简单处理我们发现的第一个视频流
	//  寻找第一个视频流
	int videoIndex = -1;
	for (int i = 0; i < fepFmtCtx->nb_streams; i++) {
		if (fepFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoIndex = i;
			break;
		}
	}
	if (-1 == videoIndex) {
		log_s("Can't find video stream when feature extracting");
		return -1;
	}
	// 得到视频流编码上下文的指针
	fepCodecCtx = fepFmtCtx->streams[videoIndex]->codec;

	//  寻找视频流的解码器
	fepCodec = avcodec_find_decoder(fepCodecCtx->codec_id);

	if (NULL == fepCodec) {
		log_s("Can't decode when feature extracting");
		return -1;
	}

	// 通知解码器我们能够处理截断的bit流，bit流帧边界可以在包中
	//视频流中的数据是被分割放入包中的。因为每个视频帧的数据的大小是可变的，
	//那么两帧之间的边界就不一定刚好是包的边界。这里，我们告知解码器我们可以处理bit流。
	if (fepCodec->capabilities & AV_CODEC_CAP_TRUNCATED) {
		fepCodecCtx->flags |= AV_CODEC_CAP_TRUNCATED;
	}


	//打开解码器 
	if (avcodec_open2(fepCodecCtx, fepCodec, NULL) != 0) {
		log_s("Decode end or Error when feature extracting.");
		return -1;
	}
	AVPacket InPack;
	int len = 0;
	AVFrame* OutFrame;
	OutFrame = av_frame_alloc();
	int nComplete = 0;


	OutputStream video_st = { 0 };
	const char* filename;
	const char* smpfilename;
	AVOutputFormat* fmt;
	AVFormatContext* oc;
	AVCodec* video_codec;
	int have_video = 0;
	int encode_video = 0;
	AVDictionary* opt = NULL;
	/* Initialize libavcodec, and register all codecs and formats. */
	CString newclipname = VideoFilename_nosuffix + "_x" + ".flv";
	CString	smpname = VideoFilename_nosuffix + "_x" + ".smp";
	CString outfile = strVideoFolderPath + "//" + newclipname;
	CString smpoutfile = strVideoFolderPath + "//" + smpname;
	
	filename = W2A(outfile);
	smpfilename = W2A(smpoutfile);

	/* allocate the output media context */
	avformat_alloc_output_context2(&oc, NULL, NULL, filename);
	if (!oc) {
		printf("Could not deduce output format from file extension: using MPEG.\n");
		avformat_alloc_output_context2(&oc, NULL, "mpeg", filename);
	}
	if (!oc) {
		
		return 1;
	}

		
	fmt = oc->oformat;
	/* Add the audio and video streams using the default format codecs
	 * and initialize the codecs. */
	if (fmt->video_codec != AV_CODEC_ID_NONE) {
		//add_stream(&video_st, oc, &video_codec, fmt->video_codec, key_width, key_height);
		add_stream(&video_st, oc, &video_codec, fmt->video_codec, screen_w, screen_h);
		have_video = 1;
		encode_video = 1;
	}
	/* Now that all the parameters are set, we can open the audio and
	 * video codecs and allocate the necessary encode buffers. */
	if (have_video)
		open_video(oc, video_codec, &video_st, opt);
	av_dump_format(oc, 0, filename, 1);
	/* open the output file, if needed */
	if (!(fmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE);
		if (ret < 0) {
			//fprintf(stderr, "Could not open '%s': %s\n", filename, av_err2str(ret));     
			return 1;
		}
	}
	/* Write the stream header, if any. */
	ret = avformat_write_header(oc, &opt);
	if (ret < 0) {
		//fprintf(stderr, "Error occurred when opening output file: %s\n", av_err2str(ret));    
		return 1;
	}
	int k = 0;
	smp ismp;
	while ((av_read_frame(fepFmtCtx, &InPack) >= 0)) {
		len = avcodec_decode_video2(fepCodecCtx, OutFrame, &nComplete, &InPack);

		//判断是否是关键帧
		if (nComplete > 0 && OutFrame->key_frame) {
			//解码一帧成功
			write_video_frame(oc, &video_st, OutFrame);

			//把数据写到smp文件中去
			ismp.setIndex(k);
			ismp.setGrade(100 - k); // just for test
			ismp.setCluster(k / 4);
			smp_data.push_back(ismp);

			k++;
		}
	}

	av_write_trailer(oc);
	/* Close each codec. */  
	if (have_video)   
		close_stream(oc, &video_st);    
	if (!(fmt->flags & AVFMT_NOFILE))    
		/* Close the output file. */     
		avio_closep(&oc->pb);


	//将smp写入文件
	int size = smp_data.size();
	vector<smp>::iterator it;
	ofstream ofile(smpfilename, ios::binary);
	ofile.write((const char*)&size, 4);
	for (it = smp_data.begin(); it != smp_data.end(); ++it)
	{
		ismp = *it;
		ofile.write((const char*)&ismp, sizeof(smp));
	}
	smp_data.clear(); // 清空smp_data vector
	ofile.close();


	/* free the stream */   
	avformat_free_context(oc); 
	m_listbox_videoclip.AddString(newclipname);
	KillTimer(1);
	GetDlgItem(IDC_BUTTON_GETTIME)->EnableWindow(1);
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(1);
	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow(1);
	GetDlgItem(IDC_BUTTON_PLAY_SLOWLY)->EnableWindow(1);
	GetDlgItem(IDC_BUTTON_PLAY_FAST)->EnableWindow(1);
	GetDlgItem(IDC_BUTTON_PLAY_FRAME)->EnableWindow(1);
	GetDlgItem(IDC_BUTTON_FEATUREEXTRACT)->EnableWindow(1);
	m_describe.SetWindowText(_T("提取成功"));
	m_progress.ShowWindow(SW_HIDE);
	return 0;
}


void CJiaohuDlg::OnBnClickedButtonFeatureextract()
{
	// TODO: 在此添加控件通知处理程序代码

	CKindDlg kk;
	int kind2;
	if (kk.DoModal() == IDOK) {
		if (!thread_pause) {
			thread_pause = !thread_pause;
			GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("播放"));
		}
		GetDlgItem(IDC_BUTTON_GETTIME)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_DELCLIP)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_CUTVIDEO)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_CLIPEXT)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_PLAY_SLOWLY)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_PLAY_FAST)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_PLAY_FRAME)->EnableWindow(0);
		m_progress.ShowWindow(SW_SHOW);
		m_progress.SetPos(0);
		SetTimer(1, 20, nullptr);
		if (m_jradiao1.GetCheck()) {
			kind2 = 1;
		}
		if (m_jradiao2.GetCheck()) {
			kind2 = 2;
		}
		if (kk.check1) {
			JEeature_Extract(1,kind2);
		}
		if (kk.check2) {
			JEeature_Extract(2,kind2);
		}
		
	}
}


void CJiaohuDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int nMax, nMin, nPos;
	m_progress.GetRange(nMin, nMax);
	nPos = m_progress.GetPos();
	++nPos;
	if (nPos > nMax)
	{
		nPos = 0;
	}
	m_progress.SetPos(nPos);

	CDialogEx::OnTimer(nIDEvent);
}

int CJiaohuDlg::save_newvideo() {

	int ret;
	OutputStream video_st = { 0 };
	const char* filename;
	AVOutputFormat* fmt;
	AVFormatContext* oc;
	AVCodec* video_codec;
	AVFrame* temp_frame;
	int have_video = 0;
	int encode_video = 0;
	AVDictionary* opt = NULL;
	CString outfile = VideoFilepath;
	USES_CONVERSION;
	filename = W2A(outfile);
	/* allocate the output media context */
	avformat_alloc_output_context2(&oc, NULL, NULL, filename);
	if (!oc) {
		printf("Could not deduce output format from file extension: using MPEG.\n");
		avformat_alloc_output_context2(&oc, NULL, "mpeg", filename);
	}
	
	if (!oc) {
		
		return 1;
	}
		
	fmt = oc->oformat;
	/* Add the audio and video streams using the default format codecs
	 * and initialize the codecs. */
	if (fmt->video_codec != AV_CODEC_ID_NONE) {
		add_stream(&video_st, oc, &video_codec, fmt->video_codec, screen_w, screen_h);
		have_video = 1;
		encode_video = 1;
		
	}
	/* Now that all the parameters are set, we can open the audio and
	 * video codecs and allocate the necessary encode buffers. */
	if (have_video) {
		open_video(oc, video_codec, &video_st, opt);	
	}
		
	
	av_dump_format(oc, 0, filename, 1);
	/* open the output file, if needed */
	if (!(fmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE);
		if (ret < 0) {
			
			//fprintf(stderr, "Could not open '%s': %s\n", filename, av_err2str(ret));     
			return 1;
		}
	}
	/* Write the stream header, if any. */
	ret = avformat_write_header(oc, &opt);
	if (ret < 0) {
		//fprintf(stderr, "Error occurred when opening output file: %s\n", av_err2str(ret));    
		return 1;
	}
	
		
	for (int i = 0; i<frames.size(); i++)
	{
		temp_frame = frames[i];
		write_video_frame(oc, &video_st, temp_frame);
	}
		
	av_write_trailer(oc);
	/* Close each codec. */
	if (have_video)
		close_stream(oc, &video_st);
	if (!(fmt->flags & AVFMT_NOFILE))
		/* Close the output file. */
		avio_closep(&oc->pb);
	/* free the stream */
	avformat_free_context(oc);
	
}

void CJiaohuDlg::OnBnClickedButtonSave()
{
	// TODO: 在此添加控件通知处理程序代码
	if (jiaoneedsave) {
		save_newvideo();
	}
	
}

void CJiaohuDlg::OnBnClickedButtonQuick()
{
	// TODO: 在此添加控件通知处理程序代码
	CGetFeatureDlg* pWnd = (CGetFeatureDlg*)AfxGetMainWnd();
	pWnd->change();
	pWnd->m_tezhengDlg.videoind = videoIndex;
	pWnd->m_tezhengDlg.screen_h = screen_h;
	pWnd->m_tezhengDlg.screen_w = screen_w;
	pWnd->m_tezhengDlg.tepixfmt = pixfmt;
	pWnd->m_tezhengDlg.VideoFilepath = VideoFilepath;
	pWnd->m_tezhengDlg.listbox_filepath.ResetContent();
	pWnd->m_tezhengDlg.tezhengframes.swap(vector<AVFrame*>());
	pWnd->m_tezhengDlg.tezhengframes.assign(frames.begin(), frames.end());
	pWnd->m_tezhengDlg.DrawThumbnails();
	pWnd->m_tezhengDlg.listbox_filepath.AddString(VideoFilepath);
	pWnd->m_tezhengDlg.SetHScroll();
	
}


void CJiaohuDlg::OnBnClickedButtonJiaoconfig()
{
	// TODO: 在此添加控件通知处理程序代码

	vector<frame_item> lists;
	// 读取 
	ifstream is(smpname, ios::binary);
	int size1;
	is.read((char*)& size1, 4);
	lists.resize(size1);
	is.read((char*)& lists[0], size1 * sizeof(list_item));
	//AfxMessageBox(lists[1].score);
}
