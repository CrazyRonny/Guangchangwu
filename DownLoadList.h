#pragma once
#include "afxcmn.h"
#include "Http.h"
#include "GuangChangWuDlg.h"
#include "afxwin.h"


// CDownLoadList 对话框

class CDownLoadList : public CDialogEx
{
	DECLARE_DYNAMIC(CDownLoadList)

public:
	CDownLoadList(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDownLoadList();

// 对话框数据
	enum { IDD = IDD_DOWNLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list;

	virtual BOOL OnInitDialog();


	afx_msg void OnBnClickedOk();
	void SetData(CHttp* pData,	CGuangChangWuDlg* pGcwDlg);

	void Refresh();
	void Video2Mp3(CString video_path,CString ffmpeg_path);
	void PauseOne(int nIndex);
	void Mp3One(int nIndex);
	void ContinueOne(int nIndex);
	void RestartOne(int nIndex);


private:
	CHttp* m_pData;
	CGuangChangWuDlg* m_pGcwDlg;



public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);

	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPause();
	afx_msg void OnContinue();
	afx_msg void OnReStart();
	afx_msg void OnMp3();
	afx_msg void OnOpendir();
};
