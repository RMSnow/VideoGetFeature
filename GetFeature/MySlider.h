#pragma once
#include <afxcmn.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include<Gdiplus.h>
using namespace Gdiplus;
using namespace std;
class MySlider :
	public CSliderCtrl
{


	DECLARE_MESSAGE_MAP()
public:
	vector<int> timepos;
	double alltime;
	int	timeclips_size;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
};

