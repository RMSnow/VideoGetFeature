// CJiaohuDlg.cpp: 实现文件
//

#include "pch.h"
#include "GetFeature.h"
#include "CJiaohuDlg.h"
#include "afxdialogex.h"


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
}


BEGIN_MESSAGE_MAP(CJiaohuDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CJiaohuDlg::OnBnClickedButtonOpen)
END_MESSAGE_MAP()


// CJiaohuDlg 消息处理程序

void CJiaohuDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码

	// 设置过滤器   
	TCHAR szFilter[] = _T("All files(*.*)|*.*|AVI file(*.avi)|*.avi|wmv file(*.wmv)|*.wmv|asf file(*.asf)|*.asf|mpg file(*.mpg)|*.mpg||");

	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, NULL, NULL, 0, szFilter, this);
	CString strFilePath;

	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		// 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
		strFilePath = fileDlg.GetPathName();
		//TODO: 改为显示目录名,而非文件名
		SetDlgItemText(IDC_EDIT_FEATURE, strFilePath);
	}
}