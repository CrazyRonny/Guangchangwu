// SaveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GuangChangWu.h"
#include "SaveDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include <string>

#include "Base64.h"

using namespace std;


// CSaveDlg 对话框

IMPLEMENT_DYNAMIC(CSaveDlg, CDialogEx)

CSaveDlg::CSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSaveDlg::IDD, pParent)
{

}

CSaveDlg::~CSaveDlg()
{
}

void CSaveDlg::SetData(CGuangChangWuDlg* pGcwDlg)
{
	m_pGcwDlg  = pGcwDlg;

}


void CSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SAVEDIR, m_savePath);
	DDX_Control(pDX, IDC_EDIT1, m_fileName);
}


BEGIN_MESSAGE_MAP(CSaveDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSaveDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON2, &CSaveDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CSaveDlg 消息处理程序


void CSaveDlg::OnBnClickedOk()
{

	CString strFileName;
	m_fileName.GetWindowText(strFileName);

	if (strFileName.IsEmpty())
	{
		AfxMessageBox(TEXT("请输入文件名"));
		return;
	}


	if(!PathFileExists(m_pGcwDlg->GetSavePath()))
	{
		AfxMessageBox(TEXT("路径不存在，请选择保存文件夹"));
		return;
	}

	strFileName += TEXT(".mp4");

	CString file_path = m_pGcwDlg->GetSavePath() + TEXT("\\") + strFileName  ;

	string szFileName ;
	CBase64::WStringToString(file_path.GetBuffer(), szFileName);

	if (CBase64::IsFileExist(szFileName.c_str()))
	{
		AfxMessageBox(TEXT("文件已存在"));
		return;
	}


	m_pGcwDlg->SetSaveFileName(strFileName);


	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CSaveDlg::OnBnClickedButton2()
{
	m_pGcwDlg->NewSavePath();

	m_savePath.SetWindowText(m_pGcwDlg->GetSavePath());
	// TODO: 在此添加控件通知处理程序代码
}





string CSaveDlg::CreateFileName(const char* file_name)
{

	string result = file_name;

	if (CBase64::IsFileExist(file_name))
	{
		int find_index = result.rfind('.');
		if (string::npos != find_index)
		{
			result.insert(find_index,"_1");
		}else
		{
			result.append("_1");
		}

		return CreateFileName(result.c_str() );//"已经存在";

	}
	else{
		return result;
	}	


}

BOOL CSaveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	m_savePath.SetWindowText(m_pGcwDlg->GetSavePath());
	
	return TRUE;  // return TRUE unless you set the focus to a control


}
