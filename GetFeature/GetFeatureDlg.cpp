﻿
// GetFeatureDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "GetFeature.h"
#include "GetFeatureDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnDestroy();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_DESTROY()

END_MESSAGE_MAP()


// CGetFeatureDlg 对话框



CGetFeatureDlg::CGetFeatureDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GETFEATURE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGetFeatureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}

BEGIN_MESSAGE_MAP(CGetFeatureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CGetFeatureDlg::OnTcnSelchangeTab1)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CGetFeatureDlg 消息处理程序

BOOL CGetFeatureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//全屏显示
	ShowWindow(SW_MAXIMIZE);
	
	//控制台调试Test
	//printInConsole("Test");

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	

	m_tab.InsertItem(0, _T("交互提取"));         
	m_tab.InsertItem(1, _T("批量提取"));
	m_tab.InsertItem(2, _T("特征编辑"));
	m_tab.InsertItem(3, _T("查重校验"));

	m_jiaohuDlg.Create(IDD_DIALOG1_JIAOHU, &m_tab);
	m_piliangDlg.Create(IDD_DIALOG2_PILIANG, &m_tab);
	m_tezhengDlg.Create(IDD_DIALOG3_TEZHENG, &m_tab);
	m_chachongDlg.Create(IDD_DIALOG4_CHACHONG, &m_tab);

	m_tab.GetClientRect(&tabRect);    // 获取标签控件客户区Rect   

	// 调整tabRect，使其覆盖范围适合放置标签页   
	//tabRect.left += 1;
	//tabRect.right -= 1;
	tabRect.top += 25;
	//tabRect.bottom -= 1;

	m_jiaohuDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
	m_piliangDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
	m_tezhengDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
	m_chachongDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGetFeatureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGetFeatureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CGetFeatureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGetFeatureDlg::teexect_reslut(int result) {
	switch (result)
	{
	case IDYES:
	{
		//点击YES按钮的具体实现功能
		m_tezhengDlg.tesave_newvideo();
		m_jiaohuDlg.m_listbox_frame.ResetContent();
		CString info1 = m_tezhengDlg.VideoFilepath + _T(":特征文件保存完毕！");
		m_tezhengDlg.m_listinfo.AddString(info1);
		CString info2 = _T("保存文件:") + m_tezhengDlg.VideoFilepath;
		m_tezhengDlg.m_listinfo.AddString(info2);
		m_tezhengDlg.SetHScroll2();
		m_tezhengDlg.needsave = false;
		break;
	}
	case IDNO:
		m_tezhengDlg.tezhengframes.swap(vector<AVFrame*>());
		m_tezhengDlg.DrawThumbnails(1);
		m_tezhengDlg.needsave = false;
		break;
	}

}
void CGetFeatureDlg::piexect_reslut(int result) {
	switch (result)
	{
	case IDYES:
	{
		//点击YES按钮的具体实现功能
		m_piliangDlg.pisave_newvideo();
		m_piliangDlg.m_listinfo.AddString(_T("保存文件：") + m_piliangDlg.VideoFilepath);
		m_piliangDlg.pineedsave = false;
		break;
	}
	case IDNO:
		m_piliangDlg.piframes.swap(vector<AVFrame*>());
		m_piliangDlg.m_listframes.ResetContent();
		m_piliangDlg.pineedsave = false;
		break;
	}

}
void CGetFeatureDlg::jiaoexect_reslut(int result) {
	switch (result)
	{
	case IDYES:
	{
		//点击YES按钮的具体实现功能
		m_jiaohuDlg.save_newvideo();
		m_jiaohuDlg.jiaoneedsave = false;
		break;
	}
	case IDNO:
		m_jiaohuDlg.frames.swap(vector<AVFrame*>());
		m_jiaohuDlg.m_listbox_frame.ResetContent();
		m_jiaohuDlg.jiaoneedsave = false;
		break;
	}

}

void CGetFeatureDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int result;
	//CRect tabRect;    // 标签控件客户区的Rect   

	// 获取标签控件客户区Rect，并对其调整，以适合放置标签页   
	m_tab.GetClientRect(&tabRect);

	//tabRect.left += 1;
	//tabRect.right -= 0;
	tabRect.top += 25;
	//tabRect.bottom -= 0;

	switch (m_tab.GetCurSel())
	{
	case 0:
		if (m_tezhengDlg.needsave) {
			result = MessageBox(TEXT("是否保存当前修改？"),NULL, MB_YESNO);
			teexect_reslut(result);
		}
		if (m_piliangDlg.pineedsave) {
			result = MessageBox(TEXT("是否保存当前修改？"), NULL, MB_YESNO);
			piexect_reslut(result);
		}
		m_jiaohuDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
		m_piliangDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		m_tezhengDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		m_chachongDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		m_jiaohuDlg.isexcut_black = true;
		m_jiaohuDlg.m_listbox_frame.ResetContent();
		break;
	case 1:
		if (m_tezhengDlg.needsave) {
			result = MessageBox(TEXT("是否保存当前修改？"), NULL, MB_YESNO);
			teexect_reslut(result);
		}
		if (m_jiaohuDlg.jiaoneedsave) {
			result = MessageBox(TEXT("是否保存当前修改？"), NULL, MB_YESNO);
			jiaoexect_reslut(result);
		}
		m_jiaohuDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		m_piliangDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
		m_tezhengDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		m_chachongDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		m_piliangDlg.isexcut_black = true;
		m_piliangDlg.m_listframes.ResetContent();
		break;
	case 2:
		if (m_piliangDlg.pineedsave) {
			result = MessageBox(TEXT("是否保存当前修改？"), NULL, MB_YESNO);
			piexect_reslut(result);
		}
		if (m_jiaohuDlg.jiaoneedsave) {
			result = MessageBox(TEXT("是否保存当前修改？"), NULL, MB_YESNO);
			jiaoexect_reslut(result);
		}
		m_jiaohuDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		m_piliangDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		m_tezhengDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
		m_chachongDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		break;
	case 3:
		if (m_tezhengDlg.needsave) {
			result = MessageBox(TEXT("是否保存当前修改？"), NULL, MB_YESNO);
			teexect_reslut(result);
		}
		if (m_piliangDlg.pineedsave) {
			result = MessageBox(TEXT("是否保存当前修改？"), NULL, MB_YESNO);
			piexect_reslut(result);
		}
		if (m_jiaohuDlg.jiaoneedsave) {
			result = MessageBox(TEXT("是否保存当前修改？"), NULL, MB_YESNO);
			jiaoexect_reslut(result);
		}
		m_jiaohuDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		m_piliangDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		m_tezhengDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		m_chachongDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);

		break;
	default:
		break;
	}

}


void CGetFeatureDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	//AfxMessageBox(_T("GETF 调用OnSize"));
	if (nType != SIZE_MINIMIZED)  //判断窗口是不是最小化了，因为窗口最小化之后 ，
			//窗口的长和宽会变成0，当前一次变化的时就会出现除以0的错误操作
	{
		m_jiaohuDlg.OnSize(nType, cx, cy);
		m_piliangDlg.OnSize(nType, cx, cy);
		m_tezhengDlg.OnSize(nType, cx, cy);
		m_chachongDlg.OnSize(nType, cx, cy);
	}
}
void CGetFeatureDlg::change() {
	m_tab.SetCurSel(2);
	m_tab.GetClientRect(&tabRect);
	tabRect.top += 25;
	m_jiaohuDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
	m_piliangDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
	m_tezhengDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
	m_chachongDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
}

