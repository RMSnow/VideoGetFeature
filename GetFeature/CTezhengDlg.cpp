// CTezhengDlg.cpp: 实现文件
//

#include "pch.h"
#include "GetFeature.h"
#include "CJiaohuDlg.h"
#include "GetFeatureDlg.h"
#include "CTezhengDlg.h"
#include "afxdialogex.h"
#include "afxcmn.h"
#include "SaveBmp.h"
#include "MyList.h"


#include <vector>
#include <iostream>
using namespace std;


#pragma comment(lib,"gdiplus.lib")

// CTezhengDlg 对话框

IMPLEMENT_DYNAMIC(CTezhengDlg, CDialogEx)

CTezhengDlg::CTezhengDlg(CWnd* pParent /*=nullptr*/): CDialogEx(IDD_DIALOG3_TEZHENG, pParent)
{

}

CTezhengDlg::~CTezhengDlg()
{
}

void CTezhengDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_LIST2, m_listCtl);
	DDX_Control(pDX, IDC_LIST2, m_listCtl);
	DDX_Control(pDX, IDC_COMBO_DENSE, m_Combobox);
	DDX_Control(pDX, IDC_LISTBOX_FILEPATH, listbox_filepath);
	DDX_Control(pDX, IDC_LISTBOX_INFO, m_listinfo);
}


BEGIN_MESSAGE_MAP(CTezhengDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SELECTALL, &CTezhengDlg::OnBnClickedButtonSelectall)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_DENSE, &CTezhengDlg::OnCbnSelchangeComboDense)
	ON_BN_CLICKED(IDC_BUTTON_INVERT, &CTezhengDlg::OnBnClickedButtonInvert)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CTezhengDlg::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_SAVE2, &CTezhengDlg::OnBnClickedButtonSave2)
	ON_BN_CLICKED(IDC_BUTTON_TEOPEN, &CTezhengDlg::OnBnClickedButtonTeopen)
	ON_LBN_SELCHANGE(IDC_LISTBOX_FILEPATH, &CTezhengDlg::OnLbnSelchangeListboxFilepath)
	ON_BN_CLICKED(IDC_BUTTON_DELFILE, &CTezhengDlg::OnBnClickedButtonDelfile)
END_MESSAGE_MAP()


// CTezhengDlg 消息处理程序
BOOL CTezhengDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	// TODO:  在此添加额外的初始化
	((CButton*)GetDlgItem(IDC_RADIO_BUTTONMO))->SetCheck(TRUE);
	get_control_original_proportion();
	display_size = 120;
	m_Combobox.ResetContent();
	m_Combobox.AddString(_T("低"));
	m_Combobox.AddString(_T("中"));
	m_Combobox.AddString(_T("高"));
	m_Combobox.SetCurSel(1);
	font.CreatePointFont(120, _T("新宋体"));
	listbox_filepath.SetFont(&font);
	m_listinfo.SetFont(&font);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CTezhengDlg::OnSize(UINT nType, int cx, int cy)
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

void CTezhengDlg::get_control_original_proportion() {
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
void CTezhengDlg::SaveAsBMP(AVFrame* pFrameRGB, AVPixelFormat pixfmt, int width, int height, int bpp)
{
	AVPicture pPictureRGB;//RGB图片
	static struct SwsContext* img_convert_ctx;
	img_convert_ctx = sws_getContext(width, height, pixfmt, width, height, \
		AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
	// 确认所需缓冲区大小并且分配缓冲区空间
	avpicture_alloc(&pPictureRGB, AV_PIX_FMT_BGR24, width, height);
	sws_scale(img_convert_ctx, pFrameRGB->data, pFrameRGB->linesize, \
		0, height, pPictureRGB.data, pPictureRGB.linesize);

	int lineBytes = pPictureRGB.linesize[0], i = 0;
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;

	bmpheader.bfType = MAKEWORD(66, 77);
	bmpheader.bfSize = lineBytes * height;
	bmpheader.bfReserved1 = 0;
	bmpheader.bfReserved2 = 0;
	bmpheader.bfOffBits = 54;

	bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.biWidth = width;
	bmpinfo.biHeight = -height;
	bmpinfo.biPlanes = 1;
	bmpinfo.biBitCount = bpp;
	bmpinfo.biCompression = BI_RGB;
	bmpinfo.biSizeImage = (width * bpp + 31) / 32 * 4 * height;
	bmpinfo.biXPelsPerMeter = 100;
	bmpinfo.biYPelsPerMeter = 100;
	bmpinfo.biClrUsed = 0;
	bmpinfo.biClrImportant = 0;

	DWORD dwInfoSize = width * height * bpp / 8;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, sizeof(bmpheader) + sizeof(bmpinfo) + dwInfoSize);
	BYTE* pvData = (BYTE*)GlobalLock(hGlobal);
	memcpy(pvData, &bmpheader, sizeof(bmpheader));
	memcpy(pvData + sizeof(bmpheader), &bmpinfo, sizeof(bmpinfo));
	memcpy(pvData + sizeof(bmpheader) + sizeof(bmpinfo), pPictureRGB.data[0], dwInfoSize);
	GlobalUnlock(hGlobal);

	IStream* pStream = NULL;
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
	Bitmap bmp(pStream);                         
	Bitmap* pThumbnail = (Bitmap*)bmp.GetThumbnailImage(display_size, display_size, NULL, NULL); //设定缩略图的大小
	HBITMAP hBmp;
	pThumbnail->GetHBITMAP(Color(255, 255, 255), &hBmp);
	CBitmap* pImage = CBitmap::FromHandle(hBmp);         //转换成CBitmap格式位图
	COLORREF crMask = RGB(255, 0, 0);//透明色
	int a = m_imgList->Add(pImage, RGB(255, 255, 255));
	//m_listCtl.InsertItem(LVIF_IMAGE | LVIF_STATE, 0, NULL, 0, LVIS_SELECTED, 0, 0);
	m_listCtl.InsertItem(a , _T("12"), a);
	avpicture_free(&pPictureRGB);
	GlobalFree(hGlobal); // 使用Bitmap完后，需要释放资源，以免造成内存泄漏。
}

void CTezhengDlg::DrawThumbnails() {
	if (m_imgList != NULL) {
		m_listCtl.DeleteAllItems();
		delete(m_imgList);
	}

	CGetFeatureDlg* pWnd = (CGetFeatureDlg*)AfxGetMainWnd();

	m_imgList = new CImageList();
	m_imgList->Create(display_size, display_size, ILC_COLOR32 | ILC_MASK, 50, 2);
	m_listCtl.SetImageList(m_imgList, LVSIL_NORMAL);

	int nFrames = tezhengframes.size();
	for (int i = 0; i < nFrames; i++) {
		SaveAsBMP(tezhengframes[i], tepixfmt, screen_w, screen_h, 24);
	}	
}

void CTezhengDlg::OnBnClickedButtonSelectall()
{
	// TODO: 在此添加控件通知处理程序代码
	m_listCtl.SetRedraw(FALSE);
	for (int i = 0; i < m_listCtl.GetItemCount(); i++)
	{
		m_listCtl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
	m_listCtl.SetRedraw(TRUE);
}

void CTezhengDlg::OnBnClickedButtonInvert()
{
	// TODO: 在此添加控件通知处理程序代码
	m_listCtl.SetRedraw(FALSE);
	for (int i = 0; i < m_listCtl.GetItemCount(); i++)
	{
		if (m_listCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			m_listCtl.SetItemState(i, 0, LVIS_SELECTED);
		}
		else
		{
			m_listCtl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		}
	}
	m_listCtl.SetRedraw(TRUE);
}


void CTezhengDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	GdiplusShutdown(m_gdiplusToken);
	// TODO: 在此处添加消息处理程序代码
}


void CTezhengDlg::OnCbnSelchangeComboDense()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCursel = -1;
	nCursel = m_Combobox.GetCurSel();
	switch (nCursel)
	{
	case 0:
		display_size = 200;
		m_listCtl.display_size = 200;
		break;
	case 1:
		display_size = 120;
		m_listCtl.display_size = 120;
		break;
	case 2:
		display_size = 80;
		m_listCtl.display_size = 80;
		break;
	default:
		display_size = 120;
		m_listCtl.display_size = 120;
		break;
	}
	DrawThumbnails();
}

void CTezhengDlg::OnBnClickedButtonDel()
{
	// TODO: 在此添加控件通知处理程序代码
	CGetFeatureDlg* pWnd = (CGetFeatureDlg*)AfxGetMainWnd();
	m_listCtl.SetRedraw(FALSE);
	for (int i = 0; i < m_listCtl.GetItemCount(); i++)
	{
		if (m_listCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED) 
		{
			tezhengframes.erase(tezhengframes.begin()+i);
			needsave = true;
		}
	}
	DrawThumbnails();
	m_listCtl.SetRedraw(TRUE);
}

void CTezhengDlg::SetHScroll()
{
	CDC* dc = GetDC();//获取显示设备上下文句柄，以后可以使用该句柄在设备上下文环境中绘图
	SIZE s;
	int index;
	CString str;
	long temp;
	for (index = 0; index < listbox_filepath.GetCount(); index++)
	{
		listbox_filepath.GetText(index, str);
		s = dc->GetTextExtent(str, str.GetLength() + 1);   // 获取字符串的像素大小

		temp = (long)SendDlgItemMessage(IDC_LISTBOX_FILEPATH, LB_GETHORIZONTALEXTENT, 0, 0); //temp得到滚动条的宽度
		//temp = m_listbox.GetHorizontalExtent();//使用该函数也可获取水平滚动条宽度（像素数）
		if (s.cx > temp)
		{
			SendDlgItemMessage(IDC_LISTBOX_FILEPATH, LB_SETHORIZONTALEXTENT, (WPARAM)s.cx, 0);
			//m_listbox.SetHoriaontalExtent(s.cx)//使用该函数也可设置水平滚动条宽度（像素数）
		}
	}
	ReleaseDC(dc);//释放句柄
}
void CTezhengDlg::SetHScroll2()
{
	CDC* dc = GetDC();//获取显示设备上下文句柄，以后可以使用该句柄在设备上下文环境中绘图
	SIZE s;
	int index;
	CString str;
	long temp;
	for (index = 0; index < m_listinfo.GetCount(); index++)
	{
		m_listinfo.GetText(index, str);
		s = dc->GetTextExtent(str, str.GetLength() + 1);   // 获取字符串的像素大小

		temp = (long)SendDlgItemMessage(IDC_LISTBOX_INFO, LB_GETHORIZONTALEXTENT, 0, 0); //temp得到滚动条的宽度
		if (s.cx > temp)
		{
			SendDlgItemMessage(IDC_LISTBOX_INFO, LB_SETHORIZONTALEXTENT, (WPARAM)s.cx, 0);
		}
	}
	ReleaseDC(dc);//释放句柄
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

 static void add_stream(OutputStream* ost, AVFormatContext* oc, AVCodec** codec, enum AVCodecID codec_id, int keywidth, int keyheight,int videoIndex) {
	AVCodecContext* c;
	int i;
	/* find the encoder */
	*codec = avcodec_find_encoder(codec_id);
	if (!(*codec)) {
		fprintf(stderr, "Could not find encoder for '%s'\n", avcodec_get_name(codec_id));
		exit(1);
	}
	ost->st = avformat_new_stream(oc, *codec);
	if (!ost->st) {
		fprintf(stderr, "Could not allocate stream\n");
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
		break;
	}
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
	if (av_compare_ts(ost->next_pts, ost->st->codec->time_base, STREAM_DURATION, AVRational{ 1, 1 }) >= 0)
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
		fill_yuv_image(ost->tmp_frame, keyframe, ost->next_pts, c->width, c->height);
		sws_scale(ost->sws_ctx, (const uint8_t * const*)ost->tmp_frame->data, ost->tmp_frame->linesize, 0, c->height, ost->frame->data, ost->frame->linesize);


	}
	else {
		fill_yuv_image(ost->frame, keyframe, ost->next_pts, c->width, c->height);

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
	frame = get_video_frame(ost, keyframe);

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
	}
	else {
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
		}
		else {
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
int CTezhengDlg::tesave_newvideo() {

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
	/* Initialize libavcodec, and register all codecs and formats. */
	//CString newclipname = VideoFilename_nosuffix + "-FD" + ".flv";
	//CString outfile = strVideoFolderPath + "//" + newclipname;
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
		add_stream(&video_st, oc, &video_codec, fmt->video_codec, screen_w, screen_h,videoind);
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


	for (int i = 0; i < tezhengframes.size(); i++)
	{
		temp_frame = tezhengframes[i];
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

void CTezhengDlg::OnBnClickedButtonSave2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (needsave) {
		tesave_newvideo();
		CGetFeatureDlg* pWnd = (CGetFeatureDlg*)AfxGetMainWnd();
		pWnd->m_jiaohuDlg.m_listbox_frame.ResetContent();
		CString info1 = VideoFilepath +_T( ":特征文件保存完毕！");
		m_listinfo.AddString(info1);
		CString info2 = _T("保存文件:") + VideoFilepath  ;
		m_listinfo.AddString(info2);
		SetHScroll2();
		needsave = false;
	}

}
int CTezhengDlg::get_allteframes() {

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
		return -1;
	}
	// 取出文件流信息
	if (avformat_find_stream_info(fepFmtCtx, NULL) < 0) {
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
		return -1;
	}
	videoind = videoIndex;
	// 得到视频流编码上下文的指针
	fepCodecCtx = fepFmtCtx->streams[videoIndex]->codec;
	screen_w = fepCodecCtx->width;
	screen_h = fepCodecCtx->height;
	tepixfmt = fepCodecCtx->pix_fmt;
	//  寻找视频流的解码器
	fepCodec = avcodec_find_decoder(fepCodecCtx->codec_id);

	if (NULL == fepCodec) {
		return -1;
	}
	if (fepCodec->capabilities & AV_CODEC_CAP_TRUNCATED) {
		fepCodecCtx->flags |= AV_CODEC_CAP_TRUNCATED;
	}
	//打开解码器
	if (avcodec_open2(fepCodecCtx, fepCodec, NULL) != 0) {

		return -1;
	}
	AVPacket* InPack;
	InPack = av_packet_alloc();
	int len = 0;

	int nComplete = 0;


	while ((av_read_frame(fepFmtCtx, InPack) >= 0)) {
		if (InPack->stream_index == videoIndex) {
			if (avcodec_send_packet(fepCodecCtx, InPack) != 0) {//解码一帧压缩数据

				break;
			}
			else {//处理解码数据
				AVFrame* OutFrame;
				OutFrame = av_frame_alloc();
				avcodec_receive_frame(fepCodecCtx, OutFrame);
				tezhengframes.push_back(OutFrame);
			}
		}

	}
	avcodec_close(fepCodecCtx);
	av_free(fepFmtCtx);

}
void CTezhengDlg::OnBnClickedButtonTeopen()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilter[] = _T("All files(*.*)|*.*|AVI file(*.avi)|*.avi|wmv file(*.wmv)|*.wmv|asf file(*.asf)|*.asf|mpg file(*.mpg)|*.mpg||");
	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, NULL, NULL, 0, szFilter, this);

	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		VideoFilepath = fileDlg.GetPathName();
		listbox_filepath.AddString(VideoFilepath);
		SetHScroll();
		tezhengframes.swap(vector<AVFrame*>());
		get_allteframes();
		DrawThumbnails();
	}
}


void CTezhengDlg::OnLbnSelchangeListboxFilepath()
{
	// TODO: 在此添加控件通知处理程序代码
	int num = listbox_filepath.GetCount();
	if (num != 0) {
		int nSel = listbox_filepath.GetCurSel();
		if (nSel != CB_ERR) {
			listbox_filepath.GetText(nSel,VideoFilepath);
			tezhengframes.swap(vector<AVFrame*>());
			get_allteframes();
			DrawThumbnails();	
		}
	}
}


void CTezhengDlg::OnBnClickedButtonDelfile()
{
	// TODO: 在此添加控件通知处理程序代码
	int num = listbox_filepath.GetCount();
	if (num != 0) {
		int nSel = listbox_filepath.GetCurSel();
		if (nSel != CB_ERR) {
			listbox_filepath.DeleteString(nSel);
		}
	}
}
