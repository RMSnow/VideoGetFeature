#pragma once
#include <list>
#include<Gdiplus.h>
#include "MyList.h"
#include "smpfile.h"

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
	
	CImageList* m_imgList = NULL;
	ULONG_PTR m_gdiplusToken;
	CComboBox m_Combobox;
	int display_size;
	MyList m_listCtl;
	CListBox listbox_filepath;
	CListBox m_listinfo;
	bool needsave = false;
	CString VideoFilepath;//视频文件的路径
	CString SmpFilepath;//视频文件的路径
	CString FolderPath;

	int screen_w, screen_h;
	AVPixelFormat tepixfmt;
	int videoind = -1;
	vector<AVFrame*> tezhengframes;
	vector<smp> smp_read_data;
	CFont font;


	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonInvert();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonSave2();
	afx_msg void OnBnClickedButtonTeopen();
	afx_msg void OnLbnSelchangeListboxFilepath();
	afx_msg void OnBnClickedButtonSelectall();
	afx_msg void OnDestroy();
	afx_msg void OnCbnSelchangeComboDense();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void get_control_original_proportion();
	void  DrawThumbnails();
	void SaveAsBMP(AVFrame* pFrameRGB, AVPixelFormat pixfmt, int width, int height, int bpp);
	int get_allteframes();
	int tesave_newvideo();
	void SetHScroll();
	void SetHScroll2();
	void GetSMPFile();
	afx_msg void OnBnClickedButtonDelfile();
};
