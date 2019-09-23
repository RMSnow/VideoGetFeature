#pragma once
#include "MySlider.h"
#include <stdlib.h>
#include <list>
#include<Gdiplus.h>
using namespace Gdiplus;
// CJiaohuDlg 对话框
#define SDL_MAIN_HANDLED 
//Refresh Event
#define SFM_REFRESH_EVENT     (SDL_USEREVENT + 1)
#define SFM_BREAK_EVENT       (SDL_USEREVENT + 2)
#define SFM_SHOWPICTURE_EVENT       (SDL_USEREVENT + 3)
extern "C" {
#include <libavformat/avformat.h>   
#include <libavcodec/avcodec.h>   
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <SDL.h>
#include <SDL_thread.h>
}
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

	CString strFeatureFolderPath;//特征保存文件夹路径
	CString VideoFilepath;//视频文件的路径
	CString strVideoFolderPath;//视频文件所属文件夹的路径
	CString VideoFilename;//视频文件的名字
	CString suffix;
	CString VideoFilename_nosuffix;
	bool newvideo = true; //判断是选择新文件还是从listbox里打开 以此来决定是否加入将文件名listbox
	bool fea_is_vid = true;//feature的目录和video的目录是否为同一个
	int m_video_duration;
	CEdit m_feature_folder_path;
	CString TimeLength;
	CListBox m_listbox_videoclip;
	CStatic m_timelength;
	CStatic m_describe;
	MySlider m_slider_seek;
	CProgressCtrl m_progress;


	GdiplusStartupInput m_pGdiplusStartupInput;
	ULONG_PTR m_pGdiToken;

	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonSetFolder();
	afx_msg void OnBnClickedButtonOpenFolder();
	afx_msg void OnBnClickedButtonPlaySlowly();
	afx_msg void OnBnClickedButtonPlayFast();
	afx_msg void OnBnClickedButtonPlayFrame();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLbnDblclkListVideoclip();
	virtual BOOL OnInitDialog();
	//int SplitVideo(int startTime, int endTime, const char* pDst);
	afx_msg void OnBnClickedButtonCutvideo();
	afx_msg void OnBnClickedButtonGettime();
	void thread_stop();
	int save_newvideo();
	int get_allframes();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void get_control_original_proportion();
	void SaveBmp(AVCodecContext* CodecContex, AVFrame* Picture, int width, int height, int num);
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

	afx_msg void OnBnClickedButtonDelclip();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonFeatureextract();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CComboBox m_combox_feature;
	CListBox m_listbox_frame;
	afx_msg void OnLbnSelchangeListFrames();
	afx_msg void OnBnClickedButtonDeleframe();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonQuick();
};
int sfp_refresh_thread(void* opaque);