#pragma once
#include <list>
#include<Gdiplus.h>
using namespace Gdiplus;

// CTezhengDlg 对话框
extern "C" {
#include <libavformat/avformat.h>   
#include <libavcodec/avcodec.h>   
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <SDL.h>
#include <SDL_thread.h>
}
class CTezhengDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTezhengDlg)

public:
	CTezhengDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTezhengDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3_TEZHENG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void get_control_original_proportion();
	void  DrawThumbnails();
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
	CImageList* m_imgList = NULL;
	CListCtrl m_listCtl;
	ULONG_PTR m_gdiplusToken;
	afx_msg void OnBnClickedButtonSelectall();
	afx_msg void OnDestroy();
	void SaveAsBMP(AVFrame* pFrameRGB, AVPixelFormat pixfmt,int width, int height, int bpp);
};
