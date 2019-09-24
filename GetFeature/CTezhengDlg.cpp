// CTezhengDlg.cpp: 实现文件
//

#include "pch.h"
#include "GetFeature.h"
#include "CTezhengDlg.h"
#include "afxdialogex.h"
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

	DDX_Control(pDX, IDC_LIST2, m_listCtl);
}


BEGIN_MESSAGE_MAP(CTezhengDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SELECTALL, &CTezhengDlg::OnBnClickedButtonSelectall)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTezhengDlg 消息处理程序

BOOL CTezhengDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
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

void CTezhengDlg::DrawThumbnails() {

	m_imgList.Create(100, 100, ILC_COLOR32 | ILC_MASK, 50, 2);
	m_listCtl.SetImageList(&m_imgList, LVSIL_NORMAL);
	Bitmap bmp(_T("E:\\0520.bmp"));
	int sourceWidth = 120;                                           //获得图片宽度,这个120和创建的120保持相同。
	int sourceHeight = bmp.GetHeight();                 //获得图片宽度                                   
	if (sourceHeight > 120)             
	{
		sourceHeight = 120;
	}
	else
	{
		sourceHeight = bmp.GetHeight();
	}
	Bitmap* pThumbnail = (Bitmap*)bmp.GetThumbnailImage(sourceWidth, sourceHeight, NULL, NULL); //设定缩略图的大小
	HBITMAP hBmp;
	pThumbnail->GetHBITMAP(Color(255, 255, 255), &hBmp);
	CBitmap* pImage = CBitmap::FromHandle(hBmp);         //转换成CBitmap格式位图
	int a = m_imgList.Add(pImage, RGB(255, 255, 255));
	m_listCtl.InsertItem(LVIF_TEXT | LVIF_STATE, 0, NULL,
		(0 % 2) == 0 ? LVIS_SELECTED : 0, LVIS_SELECTED, 0, 0);



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
