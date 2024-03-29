﻿#pragma once
#include <list>

// CChachongDlg 对话框

class CChachongDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChachongDlg)

public:
	CChachongDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CChachongDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG4_CHACHONG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy); //当窗口大小调整时调用
	void get_control_original_proportion();
	CRect m_rect;
	typedef struct Rect {
	public:
		int Id;
		double scale[4];

		Rect() {
			Id = -2;
			scale[0] = 0;
			scale[1] = 0;
			scale[2] = 0;
			scale[3] = 0;
		}

		Rect(const Rect& c) {
			*this = c;
		}
	}control;
	std::list<control*> m_con_list;
	virtual BOOL OnInitDialog();
};
