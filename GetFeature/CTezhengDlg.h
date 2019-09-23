#pragma once
#include <list>

#include <stdlib.h>
#include <Gdiplus.h>
#include "SaveBmp.h"
using namespace Gdiplus;
#define SDL_MAIN_HANDLED 
//Refresh Event
#define SFM_REFRESH_EVENT     (SDL_USEREVENT + 1)
#define SFM_BREAK_EVENT       (SDL_USEREVENT + 2)

extern "C" {
#include <libavformat/avformat.h>   
#include <libavcodec/avcodec.h>   
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <SDL.h>
#include <SDL_thread.h>
}


// CTezhengDlg 对话框

class CTezhengDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTezhengDlg)

public:
	CTezhengDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTezhengDlg();

	CString strFeatureFolderPath;//特征保存文件夹路径
	CString VideoFilepath;//视频文件的路径
	CString strVideoFolderPath;//视频文件所属文件夹的路径
	CString VideoFilename;//视频文件的名字
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void get_control_original_proportion();
	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	
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


// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3_TEZHENG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
};
