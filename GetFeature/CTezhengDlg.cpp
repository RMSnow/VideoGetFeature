// CTezhengDlg.cpp: 实现文件
//

#include "pch.h"
#include "GetFeature.h"
#include "CTezhengDlg.h"
#include "afxdialogex.h"
#include "SaveBmp.h"

#include <vector>
#include <iostream>
using namespace std;


#pragma comment(lib,"gdiplus.lib")
// CTezhengDlg 对话框

IMPLEMENT_DYNAMIC(CTezhengDlg, CDialogEx)

CTezhengDlg::CTezhengDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3_TEZHENG, pParent)
{

}

CTezhengDlg::~CTezhengDlg()
{
}

void CTezhengDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTezhengDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CTezhengDlg::OnBnClickedButtonOpen)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CTezhengDlg::OnCustomDrawList)
END_MESSAGE_MAP()


// CTezhengDlg 消息处理程序

BOOL CTezhengDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	get_control_original_proportion();

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


UINT getFrame(LPVOID lpParam) {
	CTezhengDlg* pDlg = (CTezhengDlg*)lpParam;
	AVFormatContext* fepFmtCtx = NULL;
	AVCodecContext* fepCodecCtx = NULL;
	AVCodec* fepCodec = NULL;

	USES_CONVERSION;
	char* sourceFile = W2A(pDlg->VideoFilepath);

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
	// 得到视频流编码上下文的指针
	fepCodecCtx = fepFmtCtx->streams[videoIndex]->codec;
	//  寻找视频流的解码器
	fepCodec = avcodec_find_decoder(fepCodecCtx->codec_id);

	if (NULL == fepCodec) {
		return -1;
	}

	// 通知解码器我们能够处理截断的bit流，bit流帧边界可以在包中，这里，我们告知解码器我们可以处理bit流。
	if (fepCodec->capabilities & AV_CODEC_CAP_TRUNCATED) {
		fepCodecCtx->flags |= AV_CODEC_CAP_TRUNCATED;
	}


	//打开解码器 
	if (avcodec_open2(fepCodecCtx, fepCodec, NULL) != 0) {
		return -1;
	}
	int videoHeight;
	int videoWidth;
	videoWidth = fepCodecCtx->width;
	videoHeight = fepCodecCtx->height;

	AVPacket InPack;
	int len = 0;
	AVFrame* OutFrame;
	OutFrame = av_frame_alloc();
	int nComplete = 0;

	int nFrame = 0;
	AVRational avRation = fepCodecCtx->time_base;
	float frameRate = (float)avRation.den / avRation.num;
	//av_seek_frame(pInputFormatContext,0);
	while ((av_read_frame(fepFmtCtx, &InPack) >= 0)) {
		len = avcodec_decode_video2(fepCodecCtx, OutFrame, &nComplete, &InPack);

		//解码一帧成功
		SaveBmp(fepCodecCtx, OutFrame, videoWidth, videoHeight, nFrame);
		nFrame++;
	}

	avcodec_close(fepCodecCtx);
	av_free(fepFmtCtx);

	// 删除中间保存的bitmap
	//CString bmppath_del = pDlg->strVideoFolderPath + "\\*.bmp";
	//CString comm_del;
	//comm_del.Format(_T("cmd.exe /k del %s"), bmppath_del);
	//char* outcomm_del = W2A(comm_del);
	//WinExec(outcomm_del, SW_HIDE);
}

void CTezhengDlg::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	
}

void CTezhengDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码	

	//设置过滤器
	TCHAR szFilter[] = _T("All files(*.*)|*.*|AVI file(*.avi)|*.avi|wmv file(*.wmv)|*.wmv|asf file(*.asf)|*.asf|mpg file(*.mpg)|*.mpg||");

	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, NULL, NULL, 0, szFilter, this);

	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		strVideoFolderPath = fileDlg.GetFolderPath();
		VideoFilepath = fileDlg.GetPathName();

		Sleep(300);
		AfxBeginThread(getFrame, this);
	}
}