#pragma once
#include "afxcmn.h"


// CDownDlg �Ի���

class CDownDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDownDlg)

public:
	CDownDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDownDlg();

// �Ի�������
	enum { IDD = IDD_DOWN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
