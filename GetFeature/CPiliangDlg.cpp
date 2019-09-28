// CPiliangDlg.cpp: 实现文件
//

#include "pch.h"
#include "GetFeature.h"
#include "CPiliangDlg.h"
#include "afxdialogex.h"
#include "GetFeatureDlg.h"
#include "CKindDlg.h"

// CPiliangDlg 对话框

IMPLEMENT_DYNAMIC(CPiliangDlg, CDialogEx)

CPiliangDlg::CPiliangDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2_PILIANG, pParent)
{

}

CPiliangDlg::~CPiliangDlg()
{
}

void CPiliangDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTBOX_PIPATH, m_listpath);
	DDX_Control(pDX, IDC_LISTBOX_PIFEATUREFILE, m_listfeaturefile);
	DDX_Control(pDX, IDC_LISTBOX_PIINFO, m_listinfo);
	DDX_Control(pDX, IDC_LISTBOX_PIFRAMES, m_listframes);
	DDX_Control(pDX, IDC_RADIO_BUTTON1, m_pradio1);
	DDX_Control(pDX, IDC_RADIO_BUTTON2, m_pradio2);
}


BEGIN_MESSAGE_MAP(CPiliangDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_PIOPENFOLDER, &CPiliangDlg::OnBnClickedButtonPiopenfolder)
	ON_BN_CLICKED(IDC_BUTTON_PIDELPATH, &CPiliangDlg::OnBnClickedButtonPidelpath)
	ON_BN_CLICKED(IDC_BUTTON_PIEXTRACT, &CPiliangDlg::OnBnClickedButtonPiextract)
	
	ON_BN_CLICKED(IDC_BUTTON_PICLOSE, &CPiliangDlg::OnBnClickedButtonPiclose)
	ON_LBN_SELCHANGE(IDC_LISTBOX_PIFEATUREFILE, &CPiliangDlg::OnLbnSelchangeListboxPifeaturefile)
	ON_LBN_SELCHANGE(IDC_LISTBOX_PIFRAMES, &CPiliangDlg::OnLbnSelchangeListboxPiframes)
	ON_BN_CLICKED(IDC_BUTTON_PIDELFRAME, &CPiliangDlg::OnBnClickedButtonPidelframe)
	ON_BN_CLICKED(IDC_BUTTON_PISAVE, &CPiliangDlg::OnBnClickedButtonPisave)
	ON_BN_CLICKED(IDC_BUTTON_PIQUICK, &CPiliangDlg::OnBnClickedButtonPiquick)
END_MESSAGE_MAP()


// CPiliangDlg 消息处理程序
bool pithread_exit = false;
bool piis_showpicture = true;
bool display = true;
BOOL CPiliangDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	get_control_original_proportion();
	font.CreatePointFont(120, _T("新宋体"));
	m_listpath.SetFont(&font);
	m_pradio1.SetCheck(true);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CPiliangDlg::OnSize(UINT nType, int cx, int cy)
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


void CPiliangDlg::get_control_original_proportion() {
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

void CPiliangDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()

	if (isexcut_black) {
		CRect rtTop;
		CWnd* pWnd = GetDlgItem(IDC_PICTURESHOW);
		CDC* cDc = pWnd->GetDC();
		pWnd->GetClientRect(&rtTop);
		cDc->FillSolidRect(rtTop.left, rtTop.top, rtTop.Width(), rtTop.Height(), RGB(0, 0, 0));
	}
}
void CPiliangDlg::SetHScroll()
{
	CDC* dc = GetDC();//获取显示设备上下文句柄，以后可以使用该句柄在设备上下文环境中绘图
	SIZE s;
	int index;
	CString str;
	long temp;
	for (index = 0; index < m_listpath.GetCount(); index++)
	{
		m_listpath.GetText(index, str);
		s = dc->GetTextExtent(str, str.GetLength() + 1);   // 获取字符串的像素大小

		temp = (long)SendDlgItemMessage(IDC_LISTBOX_PIPATH, LB_GETHORIZONTALEXTENT, 0, 0); //temp得到滚动条的宽度
		
		if (s.cx > temp)
		{
			SendDlgItemMessage(IDC_LISTBOX_PIPATH, LB_SETHORIZONTALEXTENT, (WPARAM)s.cx, 0);
		}
	}
	ReleaseDC(dc);//释放句柄
}
void CPiliangDlg::SetHScroll2()
{
	CDC* dc = GetDC();//获取显示设备上下文句柄，以后可以使用该句柄在设备上下文环境中绘图
	SIZE s;
	int index;
	CString str;
	long temp;
	for (index = 0; index < m_listfeaturefile.GetCount(); index++)
	{
		m_listfeaturefile.GetText(index, str);
		s = dc->GetTextExtent(str, str.GetLength() + 1);   // 获取字符串的像素大小

		temp = (long)SendDlgItemMessage(IDC_LISTBOX_PIFEATUREFILE, LB_GETHORIZONTALEXTENT, 0, 0); //temp得到滚动条的宽度

		if (s.cx > temp)
		{
			SendDlgItemMessage(IDC_LISTBOX_PIFEATUREFILE, LB_SETHORIZONTALEXTENT, (WPARAM)s.cx, 0);
		}
	}
	ReleaseDC(dc);//释放句柄
}
void CPiliangDlg::SetHScroll3()
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

		temp = (long)SendDlgItemMessage(IDC_LISTBOX_PIINFO, LB_GETHORIZONTALEXTENT, 0, 0); //temp得到滚动条的宽度

		if (s.cx > temp)
		{
			SendDlgItemMessage(IDC_LISTBOX_PIINFO, LB_SETHORIZONTALEXTENT, (WPARAM)s.cx, 0);
		}
	}
	ReleaseDC(dc);//释放句柄
}

void CPiliangDlg::OnBnClickedButtonPiopenfolder()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilter[] = _T("All files(*.*)|*.*|AVI file(*.avi)|*.avi|wmv file(*.wmv)|*.wmv|asf file(*.asf)|*.asf|mpg file(*.mpg)|*.mpg||");
	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, NULL, NULL, 0, szFilter, this);

	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		FolderPath = fileDlg.GetFolderPath();
		CString fn = fileDlg.GetFileTitle();
		VideoFilepath = fileDlg.GetPathName();
		m_listpath.AddString(VideoFilepath);
		SetHScroll();
	}
}

void CPiliangDlg::OnBnClickedButtonPidelpath()
{
	// TODO: 在此添加控件通知处理程序代码
	int num = m_listpath.GetCount();
	if (num != 0) {
		int nSel = m_listpath.GetCurSel();
		if (nSel != CB_ERR) {
			m_listpath.DeleteString(nSel);
		}
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

static void add_stream(OutputStream* ost, AVFormatContext* oc, AVCodec** codec, enum AVCodecID codec_id, int keywidth, int keyheight, int videoIndex) {
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

int CPiliangDlg::PEeature_Extract(int kind1,int kind2) {
	CString ckind1;
	CString ckind2;
	CString pisuffix;
	if (kind1 == 1) {
		ckind1 = "-X5";
		pisuffix = ".flv";
	}
	if (kind1 == 2) {
		ckind1 = "-YT";
		pisuffix = ".mp4";
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

	// 明天优化这段代码
	char* sourceFile = W2A(VideoFilepath);
	CString szSplit = _T("\\");
	CStringArray szList;
	int Count = CJiaohuDlg::SplitString(VideoFilepath, szSplit, szList, FALSE);
	VideoFilename = szList.GetAt(Count - 1);
	int tet = VideoFilepath.Find(VideoFilename);
	strVideoFolderPath = VideoFilepath.Left(tet) ;
	CString info=_T("开始处理：")+ VideoFilename;
	info.Format(info);
	m_listinfo.AddString(info);
	CRect rect;
	m_listinfo.GetClientRect(&rect);
	InvalidateRect(rect);
	UpdateWindow();
	CString szSplit2 = _T(".");
	CStringArray szList2;
	int Count2 = CJiaohuDlg::SplitString(VideoFilename, szSplit2, szList2, FALSE);
	suffix = szList2.GetAt(Count2 - 1);
	int n = VideoFilename.ReverseFind('.');
	VideoFilename_nosuffix = VideoFilename.Left(n);
	
	//注册库中含有的所有可用的文件格式和编码器，这样当打开一个文件时，它们才能够自动选择相应的文件格式和编码器。
	av_register_all();

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
	
	for (int i = 0; i < fepFmtCtx->nb_streams; i++) {
		if (fepFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoIndex = i;
			break;
		}
	}
	if (-1 == videoIndex) {

		return -1;
	}
	// 得到视频流编码上下文的指针
	fepCodecCtx = fepFmtCtx->streams[videoIndex]->codec;
	screen_w = fepCodecCtx->width;
	screen_h = fepCodecCtx->height;
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
	AVPacket InPack;
	int len = 0;
	AVFrame* OutFrame;
	OutFrame = av_frame_alloc();
	int nComplete = 0;

	OutputStream video_st = { 0 };
	const char* filename;
	AVOutputFormat* fmt;
	AVFormatContext* oc;
	AVCodec* video_codec;
	int have_video = 0;
	int encode_video = 0;
	AVDictionary* opt = NULL;
	/* Initialize libavcodec, and register all codecs and formats. */
	feature_filename = VideoFilename_nosuffix + ckind1 + ckind2 + pisuffix;
	feature_filepath = strVideoFolderPath +feature_filename;
	smp_path = feature_filepath + _T(".smp");
	filename = W2A(feature_filepath);
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
		add_stream(&video_st, oc, &video_codec, fmt->video_codec, screen_w, screen_h, videoIndex);
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
			pismp.push_back(ismp);

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
	/* free the stream */
	avformat_free_context(oc);
	av_frame_free(&OutFrame);
	avcodec_close(fepCodecCtx);
	avformat_close_input(&fepFmtCtx);

	//将smp写入文件
	int size = pismp.size();
	vector<smp>::iterator it;
	ofstream ofile(smp_path, ios::binary);
	ofile.write((const char*)&size, 4);
	for (it = pismp.begin(); it != pismp.end(); ++it)
	{
		ismp = *it;
		ofile.write((const char*)&ismp, sizeof(smp));
	}
	pismp.clear(); // 清空smp_data vector
	ofile.close();

	return 0;
}

void CPiliangDlg::OnBnClickedButtonPiextract()
{
	// TODO: 在此添加控件通知处理程序代码
	int kind2;
	CKindDlg kk;
	int num = m_listpath.GetCount();
	if (num != 0) {
		if (kk.DoModal() == IDOK) {
			if (m_pradio1.GetCheck()) {
				kind2 = 1;
			}
			if (m_pradio2.GetCheck()) {
				kind2 = 2;
			}
			for (int i = 0; i < num; i++) {
				m_listpath.GetText(i, VideoFilepath);
				if (kk.check1) {
					PEeature_Extract(1,kind2);
					Sleep(500);
					m_listfeaturefile.AddString(smp_path);
					SetHScroll2();
					m_listinfo.AddString(_T("==提取成功=="));
					m_listinfo.AddString(_T("特征保存在视频所在目录："));
					m_listinfo.AddString(smp_path);
					m_listinfo.AddString(_T("----------------"));
					SetHScroll3();
				
				}
				if (kk.check2) {
					PEeature_Extract(2,kind2);
					Sleep(500);
					m_listfeaturefile.AddString(smp_path);
					SetHScroll2();
					m_listinfo.AddString(_T("==提取成功=="));
					m_listinfo.AddString(_T("特征保存在视频所在目录："));
					m_listinfo.AddString(smp_path);
					m_listinfo.AddString(_T("----------------"));
					SetHScroll3();

				}

	
			}
		}
	}
}


void CPiliangDlg::OnBnClickedButtonPiclose()
{
	// TODO: 在此添加控件通知处理程序代码
	if (display) {
		display = false;
		GetDlgItem(IDC_BUTTON_PICLOSE)->SetWindowText(_T("开显示区"));
	}
	else {
		display = true;
		GetDlgItem(IDC_BUTTON_PICLOSE)->SetWindowText(_T("关显示区"));
	
	}
}
int CPiliangDlg::get_allpiframes() {
	piframes.swap(vector<AVFrame*>());
	int nFrame = 0;
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
	for (int i = 0; i < fepFmtCtx->nb_streams; i++) {
		if (fepFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoIndex = i;
			break;
		}
	}
	if (-1 == videoIndex) {
		
		return -1;
	}
	// 得到视频流编码上下文的指针
	fepCodecCtx = fepFmtCtx->streams[videoIndex]->codec;
	//  寻找视频流的解码器
	fepCodec = avcodec_find_decoder(fepCodecCtx->codec_id);
	screen_w = fepCodecCtx->width;
	screen_h = fepCodecCtx->height;
	pipixfmt = fepCodecCtx->pix_fmt;
	if (NULL == fepCodec) {
		
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
		
		return -1;
	}
	AVPacket* InPack;
	InPack = av_packet_alloc();
	int len = 0;

	int nComplete = 0;

	CString frame_num;
	while ((av_read_frame(fepFmtCtx, InPack) >= 0)) {

		if (InPack->stream_index == videoIndex) {
			if (avcodec_send_packet(fepCodecCtx, InPack) != 0) {//解码一帧压缩数据
				
				break;
			}
			else {//处理解码数据
				AVFrame* OutFrame;
				OutFrame = av_frame_alloc();
				avcodec_receive_frame(fepCodecCtx, OutFrame);
				piframes.push_back(OutFrame);
				frame_num.Format(_T("frame%d"), nFrame);
				m_listframes.AddString(frame_num);
				nFrame++;
			}
		}
	}
	avcodec_close(fepCodecCtx);
	av_free(fepFmtCtx);

	//从文件加载smp信息
	smp ismp;
	int size;
	SmpFilepath = ""; pismp.clear(); //清空变量信息
	SmpFilepath = VideoFilepath + _T(".smp");

	ifstream ifile(SmpFilepath, ios::binary);

	ifile.read((char*)&size, 4); //读取关键帧数量
	for (int i = 0; i < size; i++)
	{
		ifile.read((char*)&ismp, sizeof(smp));
		pismp.push_back(ismp);
	}
}


int pisfp_refresh_thread(void* opaque) {

	while (!pithread_exit) {
		if(display){
		
		if (piis_showpicture) {
			SDL_Event event;
			event.type = SFM_SHOWPICTURE_EVENTPI;
			SDL_PushEvent(&event);
			piis_showpicture = false;
			}
		}


	}
	SDL_Event event;
	event.type = SFM_BREAK_EVENT;
	SDL_PushEvent(&event);
	pithread_exit = false;
	piis_showpicture = true;
	return 0;
}
UINT pivideoplayer(LPVOID lpParam) {
	
	CPiliangDlg* pDlg = (CPiliangDlg*)lpParam;

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
		return -1;
	}
	//4. 获取文件信息
	if (avformat_find_stream_info(pFmtCtx, NULL) < 0) {
		return -1;
	}

	//5. 获取视频的index
	int i = 0;
	for (; i < pFmtCtx->nb_streams; i++) {
		if (pFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			pDlg->videoIndex = i;
			break;
		}
	}

	if (pDlg->videoIndex == -1) {
		return -1;
	}
	
	video_st = pFmtCtx->streams[pDlg->videoIndex];
	
	//6. 获取解码器并打开
	pCodecCtx = avcodec_alloc_context3(NULL);

	if (avcodec_parameters_to_context(pCodecCtx, pFmtCtx->streams[pDlg->videoIndex]->codecpar) < 0) {
		return -1;
	}

	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {

		return -1;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {//打开解码器

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

	av_dump_format(pFmtCtx, 0, filepath, 0);

	//获取SwsContext
	pSwsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, NULL, NULL, NULL, NULL);
	//----------------------------------------------------------------------------------------------------------------
	// 8. SDL相关初始化
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {

		return -1;
	}

	pDlg->isexcut_black = false;
	screen = SDL_CreateWindowFrom(pDlg->GetDlgItem(IDC_PICTURESHOW)->GetSafeHwnd());

	if (!screen) {
		return -1;
	}
	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);

	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = pCodecCtx->width;
	sdlRect.h = pCodecCtx->height;
	video_tid = SDL_CreateThread(pisfp_refresh_thread, NULL, (void*)pDlg);

	//9.读取数据播放
	for (;;) {
		 
		//Wait
		SDL_WaitEvent(&event);
		
		 if (event.type == SFM_SHOWPICTURE_EVENTPI) {
			
			pFrame = pDlg->piframes[pDlg->pikeyframe_index];
			if (sws_scale(pSwsCtx, (const uint8_t * const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
				pFrameYUV->data, pFrameYUV->linesize) == 0) {
				continue;
			}

			SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
			SDL_RenderClear(sdlRenderer);  
			SDL_RenderCopy(sdlRenderer, sdlTexture, &sdlRect, NULL);
			SDL_RenderPresent(sdlRenderer);
		}
		 else if (event.type == SFM_BREAK_EVENT) {
			
			 break;
		 }

	}

	return 0;
}

void CPiliangDlg::pithread_stop() {
	pithread_exit = true;
	display = true;
}
void CPiliangDlg::OnLbnSelchangeListboxPifeaturefile()
{
	if (piplay_thread == NULL) {
		CGetFeatureDlg* pWnd = (CGetFeatureDlg*)AfxGetMainWnd();
		if (pWnd->m_jiaohuDlg.play_thread) {
			pWnd->m_jiaohuDlg.thread_stop();
			MsgWaitForMultipleObjects(1, (HANDLE*)pWnd->m_jiaohuDlg.play_thread, FALSE, INFINITE, QS_ALLINPUT);
			pWnd->m_jiaohuDlg.play_thread = NULL;
		}
		piplay_thread = AfxBeginThread(pivideoplayer, this);
	}
	// TODO: 在此添加控件通知处理程序代码
	int num = m_listfeaturefile.GetCount();
	if (num != 0) {
		int nSel = m_listfeaturefile.GetCurSel();
		if (nSel != CB_ERR) {
			m_listfeaturefile.GetText(nSel, smp_path);
			int n = smp_path.ReverseFind('.');
			VideoFilepath = smp_path.Left(n);
			piframes.swap(vector<AVFrame*>());
			m_listframes.ResetContent();
			get_allpiframes();
			piis_showpicture = true;
		}
	}
}


void CPiliangDlg::OnLbnSelchangeListboxPiframes()
{
	// TODO: 在此添加控件通知处理程序代码
	int num = m_listframes.GetCount();
	if (num != 0) {
		pikeyframe_index = m_listframes.GetCurSel();
		if (pikeyframe_index != LB_ERR) {
			piis_showpicture = true;
		}
	}
}


void CPiliangDlg::OnBnClickedButtonPidelframe()
{
	// TODO: 在此添加控件通知处理程序代码

	CString framename;
	piframes.erase(piframes.begin() + pikeyframe_index);
	pismp.erase(pismp.begin() + pikeyframe_index);

	CString	show;
	show.Format(_T("%d"), pikeyframe_index);
	AfxMessageBox(show);

	pineedsave = true;
	m_listframes.GetText(pikeyframe_index, framename);
	m_listframes.DeleteString(pikeyframe_index);
	m_listinfo.AddString(_T("删除")+framename);
	pikeyframe_index--;
	//nFrame--;
	piis_showpicture = 1;
}
int CPiliangDlg::pisave_newvideo() {

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
		add_stream(&video_st, oc, &video_codec, fmt->video_codec, screen_w, screen_h, videoIndex);
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


	for (int i = 0; i < piframes.size(); i++)
	{
		temp_frame = piframes[i];
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

void CPiliangDlg::OnBnClickedButtonPisave()
{
	// TODO: 在此添加控件通知处理程序代码
	if (pineedsave) {
		pineedsave = false;
		pisave_newvideo();
		m_listinfo.AddString(_T("保存文件：")+smp_path);
	}

}


void CPiliangDlg::OnBnClickedButtonPiquick()
{
	// TODO: 在此添加控件通知处理程序代码

	CGetFeatureDlg* pWnd = (CGetFeatureDlg*)AfxGetMainWnd();
	pWnd->change();
	pWnd->m_tezhengDlg.videoind = videoIndex;
	pWnd->m_tezhengDlg.screen_h = screen_h;
	pWnd->m_tezhengDlg.screen_w = screen_w;
	pWnd->m_tezhengDlg.tepixfmt = pipixfmt;
	pWnd->m_tezhengDlg.VideoFilepath = VideoFilepath;
	pWnd->m_tezhengDlg.listbox_filepath.ResetContent();
	pWnd->m_tezhengDlg.tezhengframes.swap(vector<AVFrame*>());
	pWnd->m_tezhengDlg.tezhengframes.assign(piframes.begin(), piframes.end());
	pWnd->m_tezhengDlg.DrawThumbnails(1);
	pWnd->m_tezhengDlg.listbox_filepath.AddString(VideoFilepath);
	pWnd->m_tezhengDlg.SetHScroll();
}
