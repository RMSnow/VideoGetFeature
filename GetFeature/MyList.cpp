#include "pch.h"
#include "MyList.h"
#pragma comment(lib,"gdiplus.lib")
BEGIN_MESSAGE_MAP(MyList, CListCtrl)
	
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &MyList::OnNMCustomdraw)
END_MESSAGE_MAP()

MyList::MyList()
{

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
		int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		COLORREF crBkgnd;
		BOOL bListHasFocus;
		CRect rcItem;
		UINT uFormat;

		bListHasFocus = (this->GetSafeHwnd() == ::GetFocus());
		//get the image index and selected state of the item being draw
		ZeroMemory(&rItem, sizeof(LVITEM));
		rItem.mask = LVIF_IMAGE | LVIF_STATE;
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
				(ii.rcImage.right - ii.rcImage.left)) / 2, rcItem.top + 10), uFormat);

		}
		if (rItem.state & LVIS_SELECTED)//when selected draw the Frameline
		{
			Graphics g(pDC->m_hDC);
			Pen pen(Color(0, 0, 255), 10);
			g.DrawRectangle(&pen, rcItem.left + (nBoundsWidth -(ii.rcImage.right - ii.rcImage.left)) / 2, rcItem.top + 10, display_size, display_size);
		}
		else
		{
			Graphics g(pDC->m_hDC);
			Pen pen(Color(221, 224, 231), 10);
			g.DrawRectangle(&pen, rcItem.left + (nBoundsWidth - (ii.rcImage.right - ii.rcImage.left)) / 2, rcItem.top + 10, display_size, display_size);
		}
	*pResult = CDRF_SKIPDEFAULT;
	}
	
}



