
// GuangChangWuDlg.h : 头文件
//

#pragma once
#include "cwebbrowser2.h"
#include "afxext.h"
#include <Mshtmlc.h>
#include "afxwin.h"
#include "Http.h"
#include <map>

// CGuangChangWuDlg 对话框
class CGuangChangWuDlg : public CDialogEx
{
// 构造
public:
	CGuangChangWuDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GUANGCHANGWU_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CWebBrowser2 m_web;
	CBitmapButton m_bmpBtn;
	CString NewSavePath();
	CString GetSavePath();
	CString GetSaveFileName();
	CString GetBinDir();
	void SetSaveFileName(CString name);	
	void Video2Mp3(CString video_path);



	afx_msg void OnBnClickedBtnDmovie();
	afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_EVENTSINK_MAP()
	void DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL);

	afx_msg void OnBnClickedBtnLogin();

	void NewWindow3Explorer1(LPDISPATCH* ppDisp, BOOL* Cancel, unsigned long dwFlags, LPCTSTR bstrUrlContext, LPCTSTR bstrUrl);
	CEdit m_inputUrl;
	afx_msg void OnBnClickedBtnSetting();
	void BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);



private:
	CString m_video_path;
	CString m_save_dir;
	CString m_save_file_name;
	CString m_bin_dir;
	CHttp myhttp;

	//map<CString,CString> m_map_url_vedio; // 存储url 以及真正视频存储的地址
};
