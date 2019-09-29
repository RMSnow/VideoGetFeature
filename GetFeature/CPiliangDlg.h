#pragma once
#include <list>
#include "CJiaohuDlg.h"
#include "smpfile.h"
// CPiliangDlg 对话框
#define SFM_SHOWPICTURE_EVENTPI       (SDL_USEREVENT + 4)
extern "C" {
#include <libavformat/avformat.h>   
#include <libavcodec/avcodec.h>   
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <SDL.h>
#include <SDL_thread.h>
}
class CPiliangDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPiliangDlg)

public:
	CPiliangDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPiliangDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2_PILIANG };
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

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonPiopenfolder();
	afx_msg void OnBnClickedButtonPidelpath();
	afx_msg void OnBnClickedButtonPiextract();
	afx_msg void OnBnClickedButtonPiclose();
	afx_msg void OnLbnSelchangeListboxPifeaturefile();
	afx_msg void OnLbnSelchangeListboxPiframes();
	afx_msg void OnBnClickedButtonPidelframe();
	afx_msg void OnBnClickedButtonPisave();

	void get_control_original_proportion();
	void SetHScroll();
	void SetHScroll2();
	void SetHScroll3();
	int PEeature_Extract(int kind1,int kind2);
	int get_allpiframes();
	void pithread_stop();
	int pisave_newvideo();

	bool isexcut_black = true;
	CString FolderPath;//文件路径
	CString VideoFilepath;//视频文件的路径
	CString SmpFilepath; //smp 文件路径
	CString VideoFilename;
	CString VideoFilename_nosuffix;
	CString strVideoFolderPath;
	CString suffix;
	CString feature_filename;
	CString feature_filepath;
	CString smp_path;
	CListBox m_listpath;
	CListBox m_listfeaturefile;
	CListBox m_listinfo;
	CListBox m_listframes;
	CFont font;
	int screen_w,screen_h;
	int videoIndex = -1;
	AVPixelFormat pipixfmt;
	vector<AVFrame*> piframes;
	vector<smp> pismp;
	int pikeyframe_index = 0;
	CWinThread* piplay_thread = NULL;
	CButton m_pradio1;
	CButton m_pradio2;
	bool pineedsave = false;

	afx_msg void OnBnClickedButtonPiquick();
};
