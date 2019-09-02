﻿// CJiaohuDlg.cpp: 实现文件
//

#include "pch.h"
#include "GetFeature.h"
#include "CJiaohuDlg.h"
#include "afxdialogex.h"

//*************************** ffmpeg ***************************
#include <vector>
#include <iostream>
using namespace std;


#pragma comment(lib,"gdiplus.lib")
// LNK2019	无法解析的外部符号 SDL_main，该符号在函数 main_utf8 中被引用

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
	DDX_Control(pDX, IDC_LIST_VIDEOCLIP, m_listbox_videoclip);
	DDX_Control(pDX, IDC_TEXT_TIMELENGTH, m_timelength);
	DDX_Control(pDX, IDC_SLIDER_SEEK, m_slider_seek);
	DDX_Control(pDX, IDC_TEXT_DESCRIBE, m_describe);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
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

bool isexcut_black = true;
bool normal_stop = true;
bool thread_pause = false;
bool thread_exit = false;
CWinThread* play_thread = NULL;
int to_stop = 0;
int videoIndex = -1;
int is_playing_fast = 0;
int is_playing_slowly = 0;
int is_playing_frame = 0;
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
vector<int>::iterator it;
int timeclips_size;
CRect rect;
CWnd* pwnd;
int SplitString(LPCTSTR lpszStr, LPCTSTR lpszSplit, CStringArray& rArrString, BOOL bAllowNullString)
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
			//if (isexcut_line) {
			//	pDlg->InvalidateRect(rect);
			//	pDlg->UpdateWindow();
			//}
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

UINT videoplayer(LPVOID lpParam) {
	CJiaohuDlg* pDlg = (CJiaohuDlg*)lpParam;

	thread_exit = false;
	sec = 0;
	last_sec = -1;
	thread_pause = false;
	AVCodecContext* pCodecCtx = NULL;
	AVStream* video_st;
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
	

	
	//thread_pause = false;
	USES_CONVERSION;
	char* filepath = W2A(pDlg->VideoFilepath);

	//1. 初始化
	av_register_all();
	avformat_network_init();
	//2. AVFormatContext获取
	pDlg->pFmtCtx = avformat_alloc_context();
	//3. 打开文件
	if (avformat_open_input(&(pDlg->pFmtCtx), filepath, NULL, NULL) != 0) {
		log_s("Couldn't open input stream.\n");
		return -1;
	}
	//4. 获取文件信息
	if (avformat_find_stream_info(pDlg->pFmtCtx, NULL) < 0) {
		log_s("Couldn't find stream information.");
		return -1;
	}
	
	//5. 获取视频的index
	int i = 0;
	for (; i < pDlg->pFmtCtx->nb_streams; i++) {
		if (pDlg->pFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoIndex = i;
			break;
		}
	}

	if (videoIndex == -1) {
		log_s("Didn't find a video stream.");
		return -1;
	}
	video_st = pDlg->pFmtCtx->streams[videoIndex];
	alltime = (double)(pDlg->pFmtCtx->duration) / AV_TIME_BASE;//时长
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
	int Count = SplitString(pDlg->VideoFilepath, szSplit, szList, FALSE);
	pDlg->VideoFilename = szList.GetAt(Count - 1);

	if (pDlg->newvideo){
	pDlg->m_listbox_videoclip.AddString(pDlg->VideoFilename);
	}

	CString szSplit2 = _T(".");
	CStringArray szList2;
	int Count2 = SplitString(pDlg->VideoFilename, szSplit2, szList2, FALSE);
	pDlg->suffix = szList2.GetAt(Count2 - 1);
	int n = pDlg->VideoFilename.ReverseFind('.');
	pDlg->VideoFilename_nosuffix = pDlg->VideoFilename.Left(n);
	//6. 获取解码器并打开
	pCodecCtx = avcodec_alloc_context3(NULL);
	
	if (avcodec_parameters_to_context(pCodecCtx, pDlg->pFmtCtx->streams[videoIndex]->codecpar) < 0) {
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
	av_dump_format(pDlg->pFmtCtx, 0, filepath, 0);
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
	isexcut_black = false;
	screen = SDL_CreateWindowFrom(pDlg->GetDlgItem(IDC_PICTURE_PLAY)->GetSafeHwnd());
	
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
	video_tid = SDL_CreateThread(sfp_refresh_thread,NULL, (void*)pDlg);



	//9.读取数据播放
	for (;;) {		


		if (to_stop) {
			
			to_stop = 0;
			pDlg->m_slider_seek.SetPos(0);
			SDL_Quit();
			pDlg->GetDlgItem(IDC_PICTURE_PLAY)->ShowWindow(SW_SHOWNORMAL);
			thread_exit = true;
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
					seek_target = av_rescale_q(seek_target, time_base_q, pDlg->pFmtCtx->streams[stream_index]->time_base);
				}
				int error = av_seek_frame(pDlg->pFmtCtx, stream_index, seek_target, seek_flags);
				last_sec = -1;
				seek_req = 0;
			}

			if (av_read_frame(pDlg->pFmtCtx, pPacket) == 0) {
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
				thread_exit = true;
				av_packet_unref(pPacket);
				//AfxMessageBox(_T("播放结束"));
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
			thread_exit = true;
			break;
		}
		else if (event.type == SFM_BREAK_EVENT) {
			log_s("break");
			is_playing_fast = 0;
			is_playing_slowly = 0;
			is_playing_frame = 0;
			play_thread = NULL;
			isexcut_black = true;
			pDlg->GetDlgItem(IDC_BUTTON_GETTIME)->SetWindowText(_T("开始位置"));
			pDlg->GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("停止"));
			pDlg->GetDlgItem(IDC_BUTTON_GETTIME)->EnableWindow(0);
			pDlg->GetDlgItem(IDC_BUTTON_DELCLIP)->EnableWindow(0);
			pDlg->GetDlgItem(IDC_BUTTON_CUTVIDEO)->EnableWindow(0);
			pDlg->GetDlgItem(IDC_BUTTON_CLIPEXT)->EnableWindow(0);
			SDL_Quit();
			pDlg->GetDlgItem(IDC_PICTURE_PLAY)->ShowWindow(SW_SHOWNORMAL);
			//AfxMessageBox(_T("播放结束"));
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
		newvideo = true;
		if (play_thread) {
			thread_stop();
		}

		timeclips.clear();
		m_slider_seek.timepos.clear();
		thread_pause = false;
		thread_exit = false;
		con_add = true;
		//pwnd = GetDlgItem(IDC_SLIDER_SEEK); // 取得控件的指针
		m_slider_seek.GetClientRect(&rect);
		//ScreenToClient(rect);
		//m_slider_seek.SetPos(0);
		Sleep(300);
		play_thread = AfxBeginThread(videoplayer, this);
		m_listbox_videoclip.ResetContent();
		GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText((CString)"停止");
		GetDlgItem(IDC_BUTTON_GETTIME)->SetWindowText(_T("开始位置"));
		
		m_describe.SetWindowText(_T("播放视频"));
		GetDlgItem(IDC_BUTTON_GETTIME)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_DELCLIP)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_CUTVIDEO)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_CLIPEXT)->EnableWindow(0);
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
		SDL_PauseAudio(0);
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
		SDL_PauseAudio(0);
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
	GetDlgItem(IDC_PROGRESS)->ShowWindow(0);
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
		//CWnd* pwnd = GetDlgItem(IDC_SLIDER_SEEK); // 取得控件的指针
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
				//AfxMessageBox(_T("play线程正在运行"));
			}
			timeclips.clear();
			m_slider_seek.timepos.clear();
			thread_pause = false;
			thread_exit = false;
			con_add = true;		
			//pwnd = GetDlgItem(IDC_SLIDER_SEEK); // 取得控件的指针
			m_slider_seek.GetClientRect(&rect);
			//m_slider_seek.SetPos(0);
			Sleep(300);
			CString s;
			m_listbox_videoclip.GetText(nSel, s);
			VideoFilepath = strVideoFolderPath + "\\" + s;
			play_thread = AfxBeginThread(videoplayer, this);
			GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText((CString)"停止");
			GetDlgItem(IDC_BUTTON_GETTIME)->SetWindowText(_T("开始位置"));
			m_describe.SetWindowText(_T("播放视频"));
			GetDlgItem(IDC_BUTTON_GETTIME)->EnableWindow(1);
			GetDlgItem(IDC_BUTTON_DELCLIP)->EnableWindow(0);
			GetDlgItem(IDC_BUTTON_CUTVIDEO)->EnableWindow(0);
			GetDlgItem(IDC_BUTTON_CLIPEXT)->EnableWindow(0);
		}
	}

end:
	{
		//////////////////////////////////////////////////////////////////////////
		//这里可以做一些其他工作
		//////////////////////////////////////////////////////////////////////////
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






