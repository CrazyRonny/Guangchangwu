
// GuangChangWuDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GuangChangWu.h"
#include "GuangChangWuDlg.h"
#include "afxdialogex.h"
#include "Base64.h"
#include "Http.h"
#include "DownLoadList.h"
#include <string>
#include "SaveDlg.h"

#include "IniReader.h"
#include "IniWriter.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)

END_MESSAGE_MAP()


// CGuangChangWuDlg �Ի���



CGuangChangWuDlg::CGuangChangWuDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGuangChangWuDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGuangChangWuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_web);
	DDX_Control(pDX, IDC_BTN_DMOVIE, m_bmpBtn);
	DDX_Control(pDX, IDC_EDIT_URL, m_inputUrl);
}

BEGIN_MESSAGE_MAP(CGuangChangWuDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_DMOVIE, &CGuangChangWuDlg::OnBnClickedBtnDmovie)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CGuangChangWuDlg::OnNMClickSyslink1)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CGuangChangWuDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_SETTING, &CGuangChangWuDlg::OnBnClickedBtnSetting)
END_MESSAGE_MAP()


// CGuangChangWuDlg ��Ϣ�������

BOOL CGuangChangWuDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������




// 	m_bmpBtn.LoadBitmaps(IDB_BITMAP1,IDB_BITMAP2);
// 	m_bmpBtn.SizeToContent();  //ʹ��ť��ӦͼƬ��С  




	TCHAR exeFullPath[MAX_PATH]; // MAX_PATH
	GetModuleFileName(NULL,exeFullPath,MAX_PATH);//�õ�����ģ�����ƣ�ȫ·��

	m_bin_dir = exeFullPath;

	int find_index = m_bin_dir.ReverseFind(TCHAR('\\'));

	if (-1 != find_index )
	{
		m_bin_dir = m_bin_dir.Left(find_index);
	}


	

	CString  iniPath = m_bin_dir + TEXT("\\cfg.ini");

	CIniReader iniReader(iniPath.GetBuffer());

	LPTSTR szName = iniReader.ReadString(TEXT("save"), TEXT("path"), TEXT("d:\\"));   


	m_save_dir = szName;


	COleVariant vaUrl=_T("http://www.92scj.com/");

	m_web.put_Silent(TRUE);//��ֹ�ű�������ʾ  
 	m_web.Navigate2(vaUrl,NULL,NULL,NULL,NULL);


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CGuangChangWuDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGuangChangWuDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CRect rect;
		GetClientRect(&rect);
		CRect anchor_rect;

		GetDlgItem(IDC_ANCHOR)->GetWindowRect(&anchor_rect);

		ScreenToClient(anchor_rect);


		CClientDC dc(this);        //ClientDC ��ʽ����
  
		dc.MoveTo(CPoint(rect.left,anchor_rect.bottom));       
		dc.LineTo(CPoint(rect.right,anchor_rect.bottom));

		dc.MoveTo(CPoint(anchor_rect.right,anchor_rect.bottom));       
		dc.LineTo(CPoint(anchor_rect.right,rect.bottom));



		m_web.MoveWindow(anchor_rect.right +3,anchor_rect.bottom +3,rect.Width() -anchor_rect.right-3,rect.Height()-anchor_rect.bottom-3);
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CGuangChangWuDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CGuangChangWuDlg::OnBnClickedBtnDmovie()
{

	
	CString got_input_url;

	m_inputUrl.GetWindowText(got_input_url);

	if(got_input_url.IsEmpty())
	{

		AfxMessageBox(TEXT("ҳ�����ڴ򿪣����Ժ�"));
		return;
	}


	CString new_url  = CString(TEXT("www.9igcw.com/analysis/pc.php?url=")) + m_video_path;

	bool can_down = false;

	if (-1 != got_input_url.Find(TEXT("56.com")))
	{

		CString str_tmp = m_video_path;


		int find_index = str_tmp.ReverseFind(TCHAR("/"));
		if(-1 != find_index)
		{

			str_tmp	= 	str_tmp.Right(str_tmp.GetLength() - find_index -1);
		}


		str_tmp.TrimRight(TEXT(".html")).Append(TEXT(".swf"));



		new_url  = CString(TEXT("www.9igcw.com/analysis/pc.php?url=http://player.56.com/")) + str_tmp;


		can_down = true;

	}
	if (-1 != got_input_url.Find(TEXT("tangdou.com")))
	{
			can_down = true;

	}
	if (-1 != got_input_url.Find(TEXT("tudou.com")))
	{
			can_down = true;
	}
	if (-1 != got_input_url.Find(TEXT("youku.com")))
	{
			can_down = true;
	}
	if 	(-1 != got_input_url.Find(TEXT("www.92scj.com"))&&-1 != got_input_url.Find(TEXT("video")))
	{
		//new_url = m_video_path;
		can_down = true;
	}

	if (!can_down)
	{
		AfxMessageBox(TEXT("��ҳ�治��������"));
		return;
	}

	if (-1 != m_video_path.Find(TEXT("www.56.com")))
	{

		CString str_tmp = m_video_path;


		int find_index = str_tmp.ReverseFind(TCHAR('/'));
		if(-1 != find_index)
		{

			str_tmp	= 	str_tmp.Right(str_tmp.GetLength() - find_index -1);
		}


		str_tmp.TrimRight(TEXT(".html")).Append(TEXT(".swf"));



		new_url  = CString(TEXT("www.9igcw.com/analysis/pc.php?url=http://player.56.com/")) + str_tmp;


	}

	string org_str;
	CBase64::WStringToString(new_url.GetBuffer(),org_str);


	if ( 1 != myhttp.CheckUrl(org_str))
	{
		AfxMessageBox(TEXT("����Ƶ��������"));
		return;
	}


	
	CSaveDlg saveDlg;
	saveDlg.SetData(this);
	if (IDOK == saveDlg.DoModal())
	{
		CString file_name = GetSavePath() +TEXT("\\") +GetSaveFileName();

		string s_file_name;
		CBase64::WStringToString(file_name.GetBuffer(),s_file_name);


		myhttp.download_file(org_str.c_str(),s_file_name.c_str());
	}


// 
// 
// 	string result ;
// 	int ret = 	CHttp::http_get(org_str.c_str(),result);
// 
// 	if (ret == 0)
// 	{
// 
// 		wstring tmp ,save_path;
// 		CBase64::StringToWString(result, tmp);
// 
// 		string save_dir;
// 
// 		CBase64::WStringToString(m_save_dir.GetBuffer(), save_dir);
// 
// 		Sleep(5000);
// 		myhttp.download_file(result.c_str(),save_dir, org_str);
// 
// 	}



}


void CGuangChangWuDlg::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}
BEGIN_EVENTSINK_MAP(CGuangChangWuDlg, CDialogEx)
	ON_EVENT(CGuangChangWuDlg, IDC_EXPLORER1, 259, CGuangChangWuDlg::DocumentCompleteExplorer1, VTS_DISPATCH VTS_PVARIANT)
	
	ON_EVENT(CGuangChangWuDlg, IDC_EXPLORER1, 273, CGuangChangWuDlg::NewWindow3Explorer1, VTS_PDISPATCH VTS_PBOOL VTS_UI4 VTS_BSTR VTS_BSTR)
	ON_EVENT(CGuangChangWuDlg, IDC_EXPLORER1, 250, CGuangChangWuDlg::BeforeNavigate2Explorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()


void CGuangChangWuDlg::DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL)
{


	HRESULT   hr;
	LPUNKNOWN lpUnknown;
	LPUNKNOWN lpUnknownWB = NULL;
	LPUNKNOWN lpUnknownDC = NULL;
	IHTMLElementCollection *objAllElement=NULL;
	IHTMLDocument2 *objDocument=NULL;
	CString strUrl,strTemp;

	lpUnknown = m_web.GetControlUnknown();
	ASSERT(lpUnknown);

	if (lpUnknown)
	{
		hr = lpUnknown->QueryInterface(IID_IUnknown,(LPVOID*)&lpUnknownWB);
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			return;

		hr = pDisp->QueryInterface(IID_IUnknown, (LPVOID*)&lpUnknownDC);
		ASSERT(SUCCEEDED(hr));
		if (SUCCEEDED(hr) && lpUnknownWB == lpUnknownDC)
		{
			// The document has finished loading.
			strUrl=m_web.get_LocationURL();

			m_inputUrl.SetWindowText(strUrl);

			
			if (strUrl.IsEmpty())
			{
				return ;
			}

			
			m_video_path = strUrl;

			if (-1 != strUrl.Find(TEXT("www.92scj.com"))&&	-1 != strUrl.Find(TEXT("video")))
			{


				objDocument=(IHTMLDocument2*)m_web.get_Document();
				objDocument->get_all(&objAllElement);
		
				CComPtr<IDispatch>pDisp;
				objAllElement->item(COleVariant(_T("resourceurl")),COleVariant((long)0),&pDisp); 
				CComQIPtr<IHTMLElement, &IID_IHTMLElement>pElement;
				if(pDisp==NULL)
				{
					return;
				}
				else
				{

					//BSTR    strText;
					pElement=pDisp;
		

					_variant_t href ;
					pElement->getAttribute(CString(TEXT("value")).AllocSysString(), 0,&href);  
					CString cstrHref;  
					if (href.vt != VT_NULL)  
					{  
						cstrHref = href.bstrVal;  
					}  

					string org_str;
					CBase64::WStringToString(cstrHref.GetBuffer(),org_str);

					string str_out = CBase64::base64_decode(org_str);

					wstring tmp ;
					CBase64::StringToWString(str_out, tmp);

					m_video_path = tmp.c_str();

					//AfxMessageBox(tmp.c_str());

				}

			}
		}
	}	

	if (lpUnknownWB)
		lpUnknownWB->Release();

	if (lpUnknownDC)
		lpUnknownDC->Release();

}






void CGuangChangWuDlg::NewWindow3Explorer1(LPDISPATCH* ppDisp, BOOL* Cancel, unsigned long dwFlags, LPCTSTR bstrUrlContext, LPCTSTR bstrUrl)
{

	// TODO: �ڴ˴������Ϣ����������  
	* Cancel = TRUE;  
	VARIANT vInfo;  
	m_inputUrl.SetWindowText(CString(bstrUrl));
	m_web.Navigate(bstrUrl, &vInfo, &vInfo, &vInfo, &vInfo);  
	// TODO: �ڴ˴������Ϣ����������
}


void CGuangChangWuDlg::OnBnClickedBtnLogin()
{
	CString got_input_url;

	m_inputUrl.GetWindowText(got_input_url);

	m_inputUrl.SetWindowText(TEXT(""));
	
	if(got_input_url.IsEmpty())
	{
		return;
	}


	COleVariant vaUrl= got_input_url;

	m_web.Navigate2(vaUrl,NULL,NULL,NULL,NULL);


}


void CGuangChangWuDlg::OnBnClickedBtnSetting()
{


 	CDownLoadList downDlg;

	downDlg.SetData(&myhttp,this);

	downDlg.DoModal();
// 	
// 	saveDlg.SetData();
// 	saveDlg.DoModal();
	
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CAboutDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}


void CGuangChangWuDlg::BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{

	//m_inputUrl.SetWindowText(CString(URL->bstrVal));
	// TODO: �ڴ˴������Ϣ����������
}





CString CGuangChangWuDlg::NewSavePath()
{

	CString result;
	BROWSEINFO  bi;
	bi.hwndOwner=NULL;
	bi.pidlRoot=NULL;
	bi.pszDisplayName=NULL;
	bi.lpszTitle=NULL;
	bi.ulFlags=0x0040 | BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
	bi.lpfn =NULL;
	bi.iImage =0;
	LPCITEMIDLIST pidl=SHBrowseForFolder(&bi);
	if(pidl)	
	{
		TCHAR  szDisplayName[255];
		SHGetPathFromIDList(pidl,szDisplayName);
		m_save_dir = CString(szDisplayName);


		CString  iniPath = m_bin_dir + TEXT("\\cfg.ini");

		CIniReader iniReader(iniPath.GetBuffer());

		LPTSTR szName = iniReader.ReadString(TEXT("save"), TEXT("path"), TEXT(""));   



		CIniWriter iniWriter(iniPath.GetBuffer());  
		iniWriter.WriteString(TEXT("save"), TEXT("path"), m_save_dir.GetBuffer());   

	}

	return result;
}

CString CGuangChangWuDlg::GetSavePath()
{
	return m_save_dir.TrimRight(TEXT("\\"));
}


void CGuangChangWuDlg::SetSaveFileName(CString name)
{	m_save_file_name =name;

}


CString CGuangChangWuDlg::GetSaveFileName()
{
	return m_save_file_name;
}


CString CGuangChangWuDlg::GetBinDir()
{
	return m_bin_dir;
}







