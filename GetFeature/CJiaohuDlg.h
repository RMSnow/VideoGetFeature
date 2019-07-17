﻿#pragma once


// CJiaohuDlg 对话框

class CJiaohuDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CJiaohuDlg)

public:
	CJiaohuDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CJiaohuDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1_JIAOHU };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnStnClickedPicturePlay();
	CEdit m_feature_folder_path;
	afx_msg void OnBnClickedButtonPlay();

	CString m_video_file_path;
	afx_msg void OnBnClickedButtonSetFolder();
	afx_msg void OnBnClickedButtonOpenFolder();
};
