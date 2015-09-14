// DownDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GuangChangWu.h"
#include "DownDlg.h"
#include "afxdialogex.h"
#include <afxinet.h>



int nTerminate  = 0;
int nDownloaded = 0;

struct MyParams
{
	CString file;
	CWnd * pWnd;
};

UINT DownloadFile(LPVOID pParam)
{

	MyParams* params = (MyParams*)pParam;
	CWnd*			pwnd =params->pWnd;
	CProgressCtrl*	m_Prog = (CProgressCtrl*)pwnd->GetDlgItem(IDC_PROGRESS1);
	CButton*		bStart = (CButton*)pwnd->GetDlgItem(IDB_BTN_START);

	char				filebuf[512];
	CInternetSession	netSession;
	CStdioFile			*fTargFile;
	int					outfs;
	CString				szFile,FileSize,KBin,KBsec,NewName,Perc;

	try
	{
		pwnd->GetDlgItemText(IDC_FILE,szFile);
		pwnd->SetDlgItemText(IDC_STAT,TEXT("正在校验下载地址..."));
		fTargFile = netSession.OpenURL(szFile,1,INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);
		nDownloaded = 1;

		COleDateTime dlStart = COleDateTime::GetCurrentTime();
		int filesize = fTargFile->SeekToEnd();
		fTargFile->SeekToBegin();
		outfs = filesize / 1024;		// 计算文件大小（千字节）
		FileSize.Format(TEXT("%d"),outfs);	// 以KB为单位格式文件大小

		// 在当前目录创建新的目标文件
		CFile fDestFile(fTargFile->GetFileName(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		int byteswrite;		// 写入文件的字节数
		int pos = 0;		// 当前进度条的位置
		int nperc,kbrecv;	// 进度条的百分比,获取到的数据大小（Kbs为单位）
		double secs,kbsec;	// 记录秒数, 速度（KB/秒）

		// 如果文件名太长，缩短窗口的标题并在状态显示
		NewName = fTargFile->GetFileName();					// 获取新文件名

		if(fTargFile->GetFileName().GetLength() >= 10)
		{
			NewName = fTargFile->GetFileName().Mid(0,7);	// 分割文件
			NewName = NewName + TEXT("...");
		}

		pwnd->SetDlgItemText(IDC_STAT,TEXT("正在下载..."));
		m_Prog->SetRange32(0,filesize);

		while (byteswrite = fTargFile->Read(filebuf, 512))	// 读取文件
		{
			if(nTerminate == 1)						// 如果点击取消下载
			{
				fDestFile.Close();					// 关闭我们的目标文件
				fTargFile->Close();					// 关闭远程文件
				delete fTargFile;					// 删除CStdioFile对象，以防止泄漏
				pwnd->SetDlgItemText(IDC_STAT,TEXT("下载时已被用户取消！")); // Set satus bar text
				AfxEndThread(0);					// 结束下载线程
			}

			// 根据开始时间与当前时间比较，获取秒数
			COleDateTimeSpan dlElapsed = COleDateTime::GetCurrentTime() - dlStart;
			secs = dlElapsed.GetTotalSeconds();
			pos = pos + byteswrite;					// 设置新的进度条位置
			fDestFile.Write(filebuf, byteswrite);	// 将实际数据写入文件
			m_Prog->SetPos(pos);

			nperc = pos * 100 / filesize;			// 进度百分比
			kbrecv = pos / 1024;					// 获取收到的数据
			kbsec = kbrecv / secs;					// 获取每秒下载多少（KB）

			Perc.Format(TEXT("%d"),nperc);				// 格式化进度百分比
			KBin.Format(TEXT("%d"),kbrecv);				// 格式化已下载数据大小（KB）
			KBsec.Format(TEXT("%d"),(int)kbsec);			// 格式化下载速度（KB/秒）

			pwnd->SetDlgItemText(IDC_EDIT_FILESIZE,FileSize + TEXT("KB"));// 远程文件大小
			pwnd->SetDlgItemText(IDC_EDIT_SIZEOK,KBin + TEXT("KB"));		// 已下载大小
			pwnd->SetDlgItemText(IDC_EDIT2,KBsec + TEXT("KB/秒"));		// 下载速度
			pwnd->SetDlgItemText(IDC_EDIT4,Perc + TEXT("%"));				// 进度百分比
		}
		// 下载完成，关闭文件
		fDestFile.Close();
	}

	catch(CInternetException *IE)
	{
		CString strerror;
		TCHAR error[255];

		IE->GetErrorMessage(error,255); // 获取错误消息
		strerror = error;

		pwnd->SetDlgItemText(IDC_STAT,strerror);
		pwnd->SetDlgItemText(IDB_BTN_STOP,TEXT("Exit"));
		nDownloaded = 0;
		delete fTargFile;
		IE->Delete();					// 删除异常对象，以防止泄漏
	}
	// 恢复默认
	pwnd->SetDlgItemText(IDC_EDIT2,TEXT("Kb/秒"));
	//pwnd->SetDlgItemText(IDC_EDIT3,TEXT("Loading..."));
	pwnd->SetDlgItemText(IDC_EDIT4,TEXT("0%"));

	delete fTargFile;
	if(nDownloaded == 1)
	{
		pwnd->SetDlgItemText(IDC_STAT,TEXT("下载完成！"));
		bStart->EnableWindow(TRUE);
	}
	return 0;
}




// CDownDlg 对话框

IMPLEMENT_DYNAMIC(CDownDlg, CDialogEx)

CDownDlg::CDownDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDownDlg::IDD, pParent)
{

}

CDownDlg::~CDownDlg()
{
}

void CDownDlg::SetFile(CString file)
{
	m_file = file;
}

void CDownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Prog);
}


BEGIN_MESSAGE_MAP(CDownDlg, CDialogEx)
	ON_BN_CLICKED(IDB_BTN_START, &CDownDlg::OnBtnStart)
	ON_BN_CLICKED(IDB_BTN_STOP, &CDownDlg::OnBtnStop)
END_MESSAGE_MAP()


// CDownDlg 消息处理程序



void CDownDlg::OnBtnStart() 
{
	// TODO: Add your control notification handler code here
	CButton* bStart = (CButton*)GetDlgItem(IDB_BTN_START);
	bStart->EnableWindow(FALSE);

	SetDlgItemText(IDB_BTN_STOP,TEXT("取消下载"));
	nTerminate = 0; // Clear the Terminate value to 0 (starting a new download)

	// Begin our download thread
	MyParams* params = new MyParams;
	params->file = m_file;
	params->pWnd = (CWnd *)this;
	CWinThread* pThread = AfxBeginThread(DownloadFile,(LPVOID)params);
}

void CDownDlg::OnBtnStop() 
{
	// TODO: Add your control notification handler code here

	CButton* bGetFile = (CButton*)GetDlgItem(IDB_BTN_START);
	CString btnlabel;
	GetDlgItemText(IDB_BTN_STOP,btnlabel);

	if(btnlabel == TEXT("取消下载"))
	{
		// Direct to our thread to terminate
		nTerminate = 1; 

		//SetDlgItemText(IDC_EDIT3,"loading...");
		SetDlgItemText(IDC_EDIT4,TEXT("0%"));
		SetDlgItemText(IDC_EDIT2,TEXT("0Kb/秒"));
		m_Prog.SetPos(0);
		SetDlgItemText(IDB_BTN_STOP,TEXT("退出"));
		bGetFile->EnableWindow(TRUE);
	}
	else
	{
		CDialog::OnOK();
	}
}



BOOL CDownDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	SetDlgItemText(IDC_FILE,m_file);

	return TRUE;  // return TRUE unless you set the focus to a control

}
