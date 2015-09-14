// DownDlg.cpp : ʵ���ļ�
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
		pwnd->SetDlgItemText(IDC_STAT,TEXT("����У�����ص�ַ..."));
		fTargFile = netSession.OpenURL(szFile,1,INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);
		nDownloaded = 1;

		COleDateTime dlStart = COleDateTime::GetCurrentTime();
		int filesize = fTargFile->SeekToEnd();
		fTargFile->SeekToBegin();
		outfs = filesize / 1024;		// �����ļ���С��ǧ�ֽڣ�
		FileSize.Format(TEXT("%d"),outfs);	// ��KBΪ��λ��ʽ�ļ���С

		// �ڵ�ǰĿ¼�����µ�Ŀ���ļ�
		CFile fDestFile(fTargFile->GetFileName(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		int byteswrite;		// д���ļ����ֽ���
		int pos = 0;		// ��ǰ��������λ��
		int nperc,kbrecv;	// �������İٷֱ�,��ȡ�������ݴ�С��KbsΪ��λ��
		double secs,kbsec;	// ��¼����, �ٶȣ�KB/�룩

		// ����ļ���̫�������̴��ڵı��Ⲣ��״̬��ʾ
		NewName = fTargFile->GetFileName();					// ��ȡ���ļ���

		if(fTargFile->GetFileName().GetLength() >= 10)
		{
			NewName = fTargFile->GetFileName().Mid(0,7);	// �ָ��ļ�
			NewName = NewName + TEXT("...");
		}

		pwnd->SetDlgItemText(IDC_STAT,TEXT("��������..."));
		m_Prog->SetRange32(0,filesize);

		while (byteswrite = fTargFile->Read(filebuf, 512))	// ��ȡ�ļ�
		{
			if(nTerminate == 1)						// ������ȡ������
			{
				fDestFile.Close();					// �ر����ǵ�Ŀ���ļ�
				fTargFile->Close();					// �ر�Զ���ļ�
				delete fTargFile;					// ɾ��CStdioFile�����Է�ֹй©
				pwnd->SetDlgItemText(IDC_STAT,TEXT("����ʱ�ѱ��û�ȡ����")); // Set satus bar text
				AfxEndThread(0);					// ���������߳�
			}

			// ���ݿ�ʼʱ���뵱ǰʱ��Ƚϣ���ȡ����
			COleDateTimeSpan dlElapsed = COleDateTime::GetCurrentTime() - dlStart;
			secs = dlElapsed.GetTotalSeconds();
			pos = pos + byteswrite;					// �����µĽ�����λ��
			fDestFile.Write(filebuf, byteswrite);	// ��ʵ������д���ļ�
			m_Prog->SetPos(pos);

			nperc = pos * 100 / filesize;			// ���Ȱٷֱ�
			kbrecv = pos / 1024;					// ��ȡ�յ�������
			kbsec = kbrecv / secs;					// ��ȡÿ�����ض��٣�KB��

			Perc.Format(TEXT("%d"),nperc);				// ��ʽ�����Ȱٷֱ�
			KBin.Format(TEXT("%d"),kbrecv);				// ��ʽ�����������ݴ�С��KB��
			KBsec.Format(TEXT("%d"),(int)kbsec);			// ��ʽ�������ٶȣ�KB/�룩

			pwnd->SetDlgItemText(IDC_EDIT_FILESIZE,FileSize + TEXT("KB"));// Զ���ļ���С
			pwnd->SetDlgItemText(IDC_EDIT_SIZEOK,KBin + TEXT("KB"));		// �����ش�С
			pwnd->SetDlgItemText(IDC_EDIT2,KBsec + TEXT("KB/��"));		// �����ٶ�
			pwnd->SetDlgItemText(IDC_EDIT4,Perc + TEXT("%"));				// ���Ȱٷֱ�
		}
		// ������ɣ��ر��ļ�
		fDestFile.Close();
	}

	catch(CInternetException *IE)
	{
		CString strerror;
		TCHAR error[255];

		IE->GetErrorMessage(error,255); // ��ȡ������Ϣ
		strerror = error;

		pwnd->SetDlgItemText(IDC_STAT,strerror);
		pwnd->SetDlgItemText(IDB_BTN_STOP,TEXT("Exit"));
		nDownloaded = 0;
		delete fTargFile;
		IE->Delete();					// ɾ���쳣�����Է�ֹй©
	}
	// �ָ�Ĭ��
	pwnd->SetDlgItemText(IDC_EDIT2,TEXT("Kb/��"));
	//pwnd->SetDlgItemText(IDC_EDIT3,TEXT("Loading..."));
	pwnd->SetDlgItemText(IDC_EDIT4,TEXT("0%"));

	delete fTargFile;
	if(nDownloaded == 1)
	{
		pwnd->SetDlgItemText(IDC_STAT,TEXT("������ɣ�"));
		bStart->EnableWindow(TRUE);
	}
	return 0;
}




// CDownDlg �Ի���

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


// CDownDlg ��Ϣ�������



void CDownDlg::OnBtnStart() 
{
	// TODO: Add your control notification handler code here
	CButton* bStart = (CButton*)GetDlgItem(IDB_BTN_START);
	bStart->EnableWindow(FALSE);

	SetDlgItemText(IDB_BTN_STOP,TEXT("ȡ������"));
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

	if(btnlabel == TEXT("ȡ������"))
	{
		// Direct to our thread to terminate
		nTerminate = 1; 

		//SetDlgItemText(IDC_EDIT3,"loading...");
		SetDlgItemText(IDC_EDIT4,TEXT("0%"));
		SetDlgItemText(IDC_EDIT2,TEXT("0Kb/��"));
		m_Prog.SetPos(0);
		SetDlgItemText(IDB_BTN_STOP,TEXT("�˳�"));
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
