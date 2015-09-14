#pragma once
#include "GuangChangWuDlg.h"
#include "afxwin.h"


// CSaveDlg �Ի���

class CSaveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSaveDlg)

public:
	CSaveDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSaveDlg();

// �Ի�������
	enum { IDD = IDD_SAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton2();

public:
	void SetData(CGuangChangWuDlg* pGcwDlg);
	string CSaveDlg::CreateFileName(const char* file_name);


private:
		CGuangChangWuDlg* m_pGcwDlg;
public:
	CEdit m_savePath;
	virtual BOOL OnInitDialog();
	CEdit m_fileName;
};
