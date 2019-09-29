#include "pch.h"
#include "MyList.h"
#include <iostream>
#pragma comment(lib,"gdiplus.lib")

BEGIN_MESSAGE_MAP(MyList, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &MyList::OnNMCustomdraw)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

MyList::MyList()
{
	display_size = 120;
}

MyList::~MyList()
{

}


void MyList::OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	// Take the default processing unless we set this to something else below.
	*pResult = CDRF_DODEFAULT;
	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
	
		LVITEM rItem;
		int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec); //item number

		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		COLORREF crBkgnd;
		BOOL bListHasFocus;
		CRect rcItem;
		UINT uFormat;

		bListHasFocus = (this->GetSafeHwnd() == ::GetFocus());
		//get the image index and selected state of the item being draw
		ZeroMemory(&rItem, sizeof(LVITEM));
		rItem.mask = LVIF_IMAGE | LVIF_STATE|LVFIF_TEXT;
		rItem.iItem = nItem;
		rItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
		GetItem(&rItem);
		//draw the select background
		GetItemRect(nItem, &rcItem, LVIR_BOUNDS);
		int nBoundsWidth = rcItem.Width();

		
		//draw the icon ,delete the orginal blue background
		uFormat = ILD_TRANSPARENT;
		if ((rItem.state & LVIS_SELECTED) && bListHasFocus)
		{
			// uFormat|=ILD_FOCUS;
		}
		CImageList* pImageList = GetImageList(LVSIL_NORMAL);
		IMAGEINFO ii;
		if (pImageList)
		{
			pImageList->GetImageInfo(rItem.iImage, &ii);
			
			pImageList->Draw(pDC, rItem.iImage, CPoint(rcItem.left + (nBoundsWidth -
				(ii.rcImage.right - ii.rcImage.left)) / 2, rcItem.top + (rcItem.bottom-rcItem.top-ii.rcImage.bottom+ii.rcImage.top)/2), uFormat);
		}
		CString sText;
		sText = this->GetItemText(nItem, 0);
		if (sText != _T("")) {
			pDC->SetBkMode(OPAQUE);
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->DrawText(sText, CRect::CRect(rcItem.left + (rcItem.right - rcItem.left) / 2 - 10, rcItem.top + (rcItem.bottom - rcItem.top + ii.rcImage.bottom - ii.rcImage.top) / 2 - 20, rcItem.left + (rcItem.right - rcItem.left) / 2 + 30, rcItem.top + (rcItem.bottom - rcItem.top + ii.rcImage.bottom - ii.rcImage.top) / 2), DT_VCENTER);
		}
		

		//画框
		if (rItem.state & LVIS_SELECTED)
		{
			Graphics g(pDC->m_hDC);
			Pen pen(Color(0, 0, 255), 10);
			g.DrawRectangle(&pen, rcItem.left + (nBoundsWidth -(ii.rcImage.right - ii.rcImage.left)) / 2, rcItem.top + (rcItem.bottom - rcItem.top - ii.rcImage.bottom + ii.rcImage.top) / 2, display_size, display_size);
		}
		else
		{
			Graphics g(pDC->m_hDC);
			Pen pen(Color(221, 224, 231), 10);
			g.DrawRectangle(&pen, rcItem.left + (nBoundsWidth - (ii.rcImage.right - ii.rcImage.left)) / 2, rcItem.top + (rcItem.bottom - rcItem.top - ii.rcImage.bottom + ii.rcImage.top) / 2, display_size, display_size);
		}
	*pResult = CDRF_SKIPDEFAULT;
	}
}

void MyList::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//CListCtrl::OnLButtonDown(nFlags, point);
	LVHITTESTINFO lvinfo;
	lvinfo.pt = point;
	LVITEM rItem;
	int nItem = CListCtrl::HitTest(&lvinfo);
	if (nItem != -1) {

	rItem.iItem = lvinfo.iItem;
	GetItem(&rItem);
	if (CListCtrl::GetItemState(lvinfo.iItem, LVIS_SELECTED) == 0)
	{
		CListCtrl::SetItemState(lvinfo.iItem, LVIS_SELECTED, LVIS_SELECTED);
	}
	else
	{
		CListCtrl::SetItemState(lvinfo.iItem, 0, LVIS_SELECTED);
	}

	}
	
}
