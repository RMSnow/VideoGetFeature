#include "pch.h"
#include "MySlider.h"

#pragma comment(lib,"gdiplus.lib")
BEGIN_MESSAGE_MAP(MySlider, CSliderCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
END_MESSAGE_MAP()


void MySlider::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CSliderCtrl::OnLButtonDown(nFlags, point);
	CRect   rectClient, rectChannel;
	GetClientRect(rectClient);
	GetChannelRect(rectChannel);
	int nMax = 0;
	int nMin = 0;
	GetRange(nMin, nMax);
	int nPos =
		(nMax - nMin) * (point.x - rectClient.left - rectChannel.left) / (rectChannel.right - rectChannel.left);
	SetPos(nPos);
}


void MySlider::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用
	CSliderCtrl::OnPaint();
	
		CRect rect;
		this->GetClientRect(&rect);
		HWND hwnd = this->GetSafeHwnd();
		Graphics g(hwnd);
		Pen p(Color(255, 255, 0, 0));
		p.SetDashStyle(DashStyleDashDot);
		int time_num = timepos.size();
		if (time_num % 2 == 1) {
			int x_pos = timepos[time_num - 1] * (rect.right - 20) / (alltime * 10);
			g.DrawLine(&p, x_pos + 10, 0, x_pos + 10, 30);
		}
		timeclips_size = time_num / 2;
		for (int i = 0; i < timeclips_size; i++) {
			int x_pos1 = timepos[2 * i] * (rect.right - 20) / (alltime * 10);
			int x_pos2 = timepos[2 * i + 1] * (rect.right - 20) / (alltime * 10);
			g.DrawRectangle(&p, x_pos1 + 10, 0, x_pos2 - x_pos1, 30);

		}
	
}
