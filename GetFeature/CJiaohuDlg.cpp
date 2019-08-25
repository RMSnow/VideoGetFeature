// CJiaohuDlg.cpp: 实现文件
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
bool normal_stop = true;
int thread_pause = 0;
CWinThread* play_thread = NULL;
int to_stop = 0;
int videoIndex = -1;
int is_playing_fast = 0;
int is_playing_slowly = 0;
int is_playing_frame = 0;
CString time_display;
CString total_time;
CString curr_time;
int cur_hours, cur_mins, cur_secs, cur_pos;
double sec;
double last_sec = 0;
int m_dbFrameRate;
int m_dbFrameNum;
double m_video_dura;
double video_time_base;
int m_slider_pos;
double          seek_time;        //要移动的时间（秒）
int             seek_req;         //seek的标志 是否需要seek
int             seek_flags;       //seek的方式 AVSEEK_FLAG_FRAME等
int64_t         seek_pos;         //seek过后的时间
int clipindex = 0;
vector<vector<int>> timeclips;

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
	//double dura = *((double*)opaque);
	while (!pDlg->thread_exit) {
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
			if (sec > last_sec) {
				cur_pos = sec;
				cur_secs = sec;
				cur_mins = cur_secs / 60;
				cur_secs %= 60;
				cur_hours = cur_mins / 60;
				cur_mins %= 60;
				curr_time.Format(_T("%.2d : %.2d : %.2d"), cur_hours, cur_mins, cur_secs);

						pDlg->TimeLength.Format(_T("%s/%s"), curr_time, total_time);
						pDlg->m_timelength.SetWindowText(pDlg->TimeLength);
						pDlg->m_slider_seek.SetPos(cur_pos);
				
				
			}
			last_sec = sec;
		}
		//若为单帧播放
		else if(is_playing_frame)
		{
			SDL_Event event;
			event.type = SFM_REFRESH_EVENT;
			SDL_PushEvent(&event);
			is_playing_frame = 0;
			if (sec > last_sec) {
				cur_pos = sec;
				cur_secs = sec;
				cur_mins = cur_secs / 60;
				cur_secs %= 60;
				cur_hours = cur_mins / 60;
				cur_mins %= 60;
				curr_time.Format(_T("%.2d : %.2d : %.2d"), cur_hours, cur_mins, cur_secs);
				pDlg->TimeLength.Format(_T("%s/%s"), curr_time, total_time);
				pDlg->m_timelength.SetWindowText(pDlg->TimeLength);
				pDlg->m_slider_seek.SetPos(cur_pos);
				
			}
			last_sec = sec;
		}

	}
	if (normal_stop) {
	SDL_Event event;
	event.type = SFM_BREAK_EVENT;
	SDL_PushEvent(&event);
	}
	
	pDlg->thread_exit = 0;
	thread_pause = 0;
	normal_stop = true;
	return 0;
}

UINT videoplayer(LPVOID lpParam) {
	
	sec = 0;
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
	CJiaohuDlg* pDlg = (CJiaohuDlg*)lpParam;
	pDlg->thread_exit = 0;
	USES_CONVERSION;
	char* filepath = W2A(pDlg->VideoFilepath);
	//printInConsole(filepath);

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
	double time = (double)(pDlg->pFmtCtx->duration) / AV_TIME_BASE;//时长
	int secs = (int)time % 60;
	int mins = (int)time / 60 % 60;
	int hours = (int)time / 60 / 60;
	total_time.Format(_T("%.2d : %.2d : %.2d"), hours, mins, secs);
	pDlg->TimeLength.Format(_T("00 : 00 : 00/%s"), total_time); // format the output
	pDlg->m_timelength.SetWindowText(pDlg->TimeLength);
	pDlg->m_timelength.ShowWindow(SW_SHOW);
	pDlg->m_slider_seek.SetRange(0, time, 0);
	last_sec = 0;
	if (video_st->r_frame_rate.den > 0)
		m_dbFrameRate = av_q2d(video_st->r_frame_rate);//avStream->r_frame_rate.num / avStream->r_frame_rate.den;
	else if (video_st->codec->framerate.den > 0)
		m_dbFrameRate = av_q2d(video_st->codec->framerate);//m_pvCodecCtxOrg->framerate.num / m_pvCodecCtxOrg->framerate.den;

	m_dbFrameNum = video_st->nb_frames;
	if (m_dbFrameNum == 0)
	{
		m_dbFrameNum = time * m_dbFrameRate;
	}
	m_video_dura = time/m_dbFrameNum;

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
	Sleep(100);
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

	//======================== 添加 ========================
	/*screen = SDL_CreateWindow("WS ffmpeg player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		pCodecCtx->width / 2, pCodecCtx->height / 2, SDL_WINDOW_OPENGL);
	*/
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
	//----------------------------------------------------------------------------------------------------------------
	//printInConsole();


	int count = 0;

	//9.读取数据播放
	for (;;) {

		if (to_stop) {
			pDlg->thread_exit = 1;
			to_stop = 0;
			break;
		}

		//Wait
		SDL_WaitEvent(&event);
		if (event.type == SFM_REFRESH_EVENT) {
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
				pDlg->thread_exit = 1;
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
			pDlg->thread_exit = 1;
			break;
		}
		else if (event.type == SFM_BREAK_EVENT) {
			log_s("break");
			is_playing_fast = 0;
			is_playing_slowly = 0;
			is_playing_frame = 0;
			play_thread = NULL;
			//AfxMessageBox(_T("播放结束"));
			break;
		}
	}

	//当前视频播放结束后，结束线程
	//play_thread = NULL;
	//pDlg->pFmtCtx = NULL;

	//sdl退出
	//SDL_Quit();

	////回收
	//if (pSwsCtx != NULL) {
	//	sws_freeContext(pSwsCtx);
	//	pSwsCtx = NULL;
	//}
	//if (outBuffer != NULL) {
	//	av_free(outBuffer);
	//	outBuffer = NULL;
	//}
	//if (pFrameYUV != NULL) {
	//	av_frame_free(&pFrameYUV);
	//	pFrameYUV = NULL;
	//}
	//if (pFrame != NULL) {
	//	av_frame_free(&pFrame);
	//	pFrame = NULL;
	//}
	//if (pCodecCtx != NULL) {
	//	avcodec_close(pCodecCtx);
	//	pCodecCtx = NULL;
	//}
	//if (dlg->pFmtCtx != NULL) {
	//	avformat_close_input(&dlg->pFmtCtx);
	//	dlg->pFmtCtx = NULL;
	//}
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
			//AfxMessageBox(_T("play线程正在运行"));
		}
		//AfxMessageBox(_T("将要开启play线程"));
		m_slider_seek.SetPos(0);
		play_thread = AfxBeginThread(videoplayer, this);
		//Sleep(1000);
		m_listbox_videoclip.ResetContent();
		//GetDlgItem(IDC_TEXT_TIMELENGTH)->SetWindowText(TimeLength);
		//GetDlgItem(IDC_TEXT_TIMELENGTH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText((CString)"停止");

	}


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
	thread_pause = 1;
	GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("播放"));
	GetDlgItem(IDC_BUTTON_PLAY_SLOWLY)->SetWindowText(_T("慢放"));
	GetDlgItem(IDC_BUTTON_PLAY_FAST)->SetWindowText(_T("快放"));

	is_playing_frame = 1;
}


BOOL CJiaohuDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	Gdiplus::GdiplusStartup(&m_pGdiToken, &m_pGdiplusStartupInput, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CJiaohuDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_slider_pos = m_slider_seek.GetPos();
	if (pScrollBar->GetSafeHwnd() == m_slider_seek.GetSafeHwnd())        //如果是 文件播放进度条
	{
		//?? seek后的文件结束时间可以优化用video->clock代表文件当前播放的时间 和文件结束时间不匹配
		//首先暂停
		//////////////////////////////////////////////////////////////////////////
		if (!thread_pause) //如果正在播放
		{
			//暂停
			thread_pause = !thread_pause;
		}
		//////////////////////////////////////////////////////////////////////////

		//if (nSBCode == SB_ENDSCROLL) //结束滚动  
		//{
			seek_time = m_slider_pos - (int)(sec);  //获取改变的时常 可能是整数也可能是负数
			seek_req = 1;
			seek_pos = m_slider_pos;
			seek_flags = AVSEEK_FLAG_FRAME;//m_streamstate->seek_time < 0 ? AVSEEK_FLAG_BACKWARD : 0;
			m_slider_pos = 0;
			//////////////////////////////////////////////////////////////////////////
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
				//seek成功
				//if (error >= 0)
				//{
				//	//last_sec = 0;
				//}
				seek_req = 0;
				seek_time = 0;

			}
			//恢复播放
			if (thread_pause) //如果正在暂停
			{
				//播放
				
				thread_pause = !thread_pause;
			}
			//////////////////////////////////////////////////////////////////////////

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
			m_slider_seek.SetPos(0);
			CString s;
			m_listbox_videoclip.GetText(nSel, s);
			VideoFilepath = strVideoFolderPath + "\\" + s;
			play_thread = AfxBeginThread(videoplayer, this);
			//Sleep(1000);
			//GetDlgItem(IDC_TEXT_TIMELENGTH)->SetWindowText(TimeLength);
			//GetDlgItem(IDC_TEXT_TIMELENGTH)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText((CString)"停止");
		}


		
	}

end:
	{
		//////////////////////////////////////////////////////////////////////////
		//这里可以做一些其他工作
		//////////////////////////////////////////////////////////////////////////
	}
}

int CJiaohuDlg::SplitVideo(int startTime, int endTime, const char* pDst)
{
	
	AVFormatContext* pOutAVFmtCtx = NULL;
	AVOutputFormat* pOutAVFmt = NULL;
	AVPacket pkt;
	int ret;
	int i;
	int isOpen = 0;

	/* 打开输出文件 */
	avformat_alloc_output_context2(&pOutAVFmtCtx, NULL, NULL, pDst);
	if (!pOutAVFmtCtx)
	{
		printf("avformat_alloc_output_context2 error!\n");
		ret = AVERROR_UNKNOWN;
		goto end;
	}

	pOutAVFmt = pOutAVFmtCtx->oformat;

	/* 为输出多媒体文件创建流并且拷贝流参数 */
	for (i = 0; i < pFmtCtx->nb_streams; i++)
	{
		AVStream* pInStream = pFmtCtx->streams[i];
		AVStream* pOutStream = avformat_new_stream(pOutAVFmtCtx, pInStream->codec->codec);;
		if (!pOutStream)
		{
			printf("avformat_new_stream error!\n");
			ret = AVERROR_UNKNOWN;
			goto end;
		}

		/* 拷贝参数 */
		ret = avcodec_copy_context(pOutStream->codec, pInStream->codec);
		if (ret < 0)
		{
			printf("avcodec_copy_context error!\n");
			goto end;
		}

		/* 拷贝参数 */
		ret = avcodec_parameters_copy(pOutStream->codecpar, pInStream->codecpar);
		if (ret < 0)
		{
			printf("avcodec_parameters_copy error!\n");
			return -1;
		}

		pOutStream->codec->codec_tag = 0;

	}

	av_dump_format(pOutAVFmtCtx, 0, pDst, 1);

	/* 打开输出多媒体文件，准备写数据 */
	ret = avio_open(&pOutAVFmtCtx->pb, pDst, AVIO_FLAG_WRITE);
	if (ret < 0)
	{
		printf("avio_open error!\n");
		goto end;
	}
	isOpen = 1;

	/* 写多媒体文件头 */
	ret = avformat_write_header(pOutAVFmtCtx, NULL);
	if (ret < 0)
	{
		printf("avformat_write_header error!\n");
		goto end;
	}

	/* 移动到相应的时间点 */
	ret = av_seek_frame(pFmtCtx, -1, startTime * AV_TIME_BASE, AVSEEK_FLAG_ANY);
	if (ret < 0)
	{
		printf("av_seek_frame error!\n");
		goto end;
	}

	int64_t* dtsStartTime = (int64_t*)malloc(sizeof(int64_t) * pFmtCtx->nb_streams);
	memset(dtsStartTime, 0, sizeof(int64_t) * pFmtCtx->nb_streams);
	int64_t* ptsStartTime = (int64_t*)malloc(sizeof(int64_t) * pFmtCtx->nb_streams);
	memset(ptsStartTime, 0, sizeof(int64_t) * pFmtCtx->nb_streams);

	while (1)
	{
		AVStream* pInStream, * pOutStream;

		ret = av_read_frame(pFmtCtx, &pkt);
		if (ret < 0)
			break;

		pInStream = pFmtCtx->streams[pkt.stream_index];
		pOutStream = pOutAVFmtCtx->streams[pkt.stream_index];

		if (av_q2d(pInStream->time_base) * pkt.pts > endTime)
		{
			av_free_packet(&pkt);
			break;
		}

		if (dtsStartTime[pkt.stream_index] == 0)
			dtsStartTime[pkt.stream_index] = pkt.dts;

		if (ptsStartTime[pkt.stream_index] == 0)
			ptsStartTime[pkt.stream_index] = pkt.pts;

		/* 转化时间基 */
		pkt.pts = av_rescale_q_rnd(pkt.pts - ptsStartTime[pkt.stream_index], pInStream->time_base, pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts - dtsStartTime[pkt.stream_index], pInStream->time_base, pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
		if (pkt.pts < 0)
			pkt.pts = 0;

		if (pkt.dts < 0)
			pkt.dts = 0;

		pkt.duration = (int)av_rescale_q((int64_t)pkt.duration, pInStream->time_base, pOutStream->time_base);
		pkt.pos = -1;

		/* 写数据 */
		ret = av_interleaved_write_frame(pOutAVFmtCtx, &pkt);
		if (ret < 0)
		{
			printf("av_interleaved_write_frame error!\n");
			break;
		}

		av_free_packet(&pkt);
	}

	free(dtsStartTime);
	free(ptsStartTime);

	av_write_trailer(pOutAVFmtCtx);

end:
	if (isOpen)
		avio_closep(&pOutAVFmtCtx->pb);

	if (pOutAVFmtCtx)
		avformat_free_context(pOutAVFmtCtx);

	return ret;
}


void CJiaohuDlg::OnBnClickedButtonCutvideo()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!thread_pause) {
		thread_pause = !thread_pause;
		GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText((CString)"播放");
	}
	for (int i = 0; i < 3; i++) {
		clipindex++;
		CString str;
		str.Format(_T("%d"), clipindex);
	CString outfile = strVideoFolderPath + "//"+VideoFilename_nosuffix+"_"+str+"."+suffix;
	USES_CONVERSION;
	char* outfilepath = W2A(outfile);
	SplitVideo(5, 10, outfilepath);
	}

	
}


void CJiaohuDlg::OnBnClickedButtonGettime()
{
	// TODO: 在此添加控件通知处理程序代码
	Graphics g(this->m_hWnd);
	Pen p(Color(255, 0, 0, 255));
	g.DrawLine(&p, 100, 260, 500, 260);
	

}


void CJiaohuDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	GdiplusShutdown(m_pGdiToken);
	// TODO: 在此处添加消息处理程序代码
}
