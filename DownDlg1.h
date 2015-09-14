#pragma once
#include "afxcmn.h"


// CDownDlg 对话框

class CDownDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDownDlg)

public:
	CDownDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDownDlg();

// 对话框数据
	enum { IDD = IDD_DOWN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBtnStart();
	afx_msg void OnBtnStop();
	CProgressCtrl m_Prog;
	virtual BOOL OnInitDialog();

	void SetFile(CString file);

private:
	CString m_file;
};
