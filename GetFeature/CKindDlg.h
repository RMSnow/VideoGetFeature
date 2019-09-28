#pragma once


// CKindDlg 对话框

class CKindDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CKindDlg)

public:
	CKindDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CKindDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_KIND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	

	CButton m_kindcheck1;
	CButton m_kindcheck2;
	bool check1 = false;
	bool check2 = false;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
};
