// CTezhengDlg.cpp: 实现文件
//

#include "pch.h"
#include "GetFeature.h"
#include "CJiaohuDlg.h"
#include "GetFeatureDlg.h"
#include "CTezhengDlg.h"
#include "afxdialogex.h"
#pragma comment(lib,"gdiplus.lib")
#include "SaveBmp.h"

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
}


BEGIN_MESSAGE_MAP(CTezhengDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SELECTALL, &CTezhengDlg::OnBnClickedButtonSelectall)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_DENSE, &CTezhengDlg::OnCbnSelchangeComboDense)
END_MESSAGE_MAP()


// CTezhengDlg 消息处理程序

BOOL CTezhengDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	// TODO:  在此添加额外的初始化
	get_control_original_proportion();
	display_size = 120;

	m_Combobox.ResetContent();
	m_Combobox.AddString(_T("低"));
	m_Combobox.AddString(_T("中"));
	m_Combobox.AddString(_T("高"));
	m_Combobox.SetCurSel(1);

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
	m_listCtl.InsertItem(a, NULL, a);
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
	CGetFeatureDlg* pWnd = (CGetFeatureDlg*)AfxGetMainWnd();
	int nFrames = pWnd->m_jiaohuDlg.frames.size();
	for (int i = 0; i < nFrames; i++) {
		SaveAsBMP(pWnd->m_jiaohuDlg.frames[i], pWnd->m_jiaohuDlg.pixfmt, pWnd->m_jiaohuDlg.screen_w, pWnd->m_jiaohuDlg.screen_h, 24);
	}	
}

void CTezhengDlg::OnBnClickedButtonSelectall()
{
	// TODO: 在此添加控件通知处理程序代码
	
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
	AfxMessageBox(nCursel);
	switch (nCursel)
	{
	case 0:
		display_size = 200;
	case 1:
		display_size = 120;
	case 2:
		display_size = 40;
	default:
		display_size = 120;
		break;
	}
	DrawThumbnails();
}
