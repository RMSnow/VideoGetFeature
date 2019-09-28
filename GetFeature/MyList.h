#pragma once
#include <afxcmn.h>
#include<Gdiplus.h>
using namespace Gdiplus;
class MyList :
	public CListCtrl
{

DECLARE_MESSAGE_MAP()
public:
	MyList();
	virtual ~MyList();
	afx_msg void OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	int display_size;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

