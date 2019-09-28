// CKindDlg.cpp: 实现文件
//

#include "pch.h"
#include "GetFeature.h"
#include "CKindDlg.h"
#include "afxdialogex.h"


// CKindDlg 对话框

IMPLEMENT_DYNAMIC(CKindDlg, CDialogEx)

CKindDlg::CKindDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_KIND, pParent)
{

}

CKindDlg::~CKindDlg()
{
}

void CKindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_kindcheck1);
	DDX_Control(pDX, IDC_CHECK2, m_kindcheck2);
}


BEGIN_MESSAGE_MAP(CKindDlg, CDialogEx)
	
	
	ON_BN_CLICKED(IDOK, &CKindDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK1, &CKindDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &CKindDlg::OnBnClickedCheck2)
END_MESSAGE_MAP()


// CKindDlg 消息处理程序








void CKindDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	
	if (m_kindcheck1.GetCheck()) {
		check1 = true;
	}
	else
	{
		check1 = false;
	}
	if (m_kindcheck2.GetCheck()) {
		check2 = true;
	}
	else
	{
		check2 = false;
	}
	CDialogEx::OnOK();
}


void CKindDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDOK)->EnableWindow(1);
}


void CKindDlg::OnBnClickedCheck2()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDOK)->EnableWindow(1);
}
