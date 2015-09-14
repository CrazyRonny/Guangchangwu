// DownLoadList.cpp : 实现文件
//

#include "stdafx.h"
#include "GuangChangWu.h"
#include "DownLoadList.h"
#include "afxdialogex.h"
#include "Base64.h"
#include <iostream>
#include "SaveDlg.h"
#include "Http.h"

#include <sstream>

// CDownLoadList 对话框

IMPLEMENT_DYNAMIC(CDownLoadList, CDialogEx)

	CDownLoadList::CDownLoadList(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDownLoadList::IDD, pParent)
{

}

CDownLoadList::~CDownLoadList()
{
}





void CDownLoadList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);

}


BEGIN_MESSAGE_MAP(CDownLoadList, CDialogEx)

	ON_BN_CLICKED(IDOK, &CDownLoadList::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CDownLoadList::OnBnClickedButton1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CDownLoadList::OnBnClickedCancel)
	ON_WM_RBUTTONDBLCLK()
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CDownLoadList::OnNMRClickList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CDownLoadList::OnNMRClickList1)
	ON_COMMAND(ID_PAUSE, &CDownLoadList::OnPause)
	ON_COMMAND(ID_CONTINUE, &CDownLoadList::OnContinue)
	ON_COMMAND(ID_REDOWN, &CDownLoadList::OnReStart)
	ON_COMMAND(ID_MP3, &CDownLoadList::OnMp3)
	ON_COMMAND(ID_OPENDIR, &CDownLoadList::OnOpendir)
END_MESSAGE_MAP()


// CDownLoadList 消息处理程序



BOOL CDownLoadList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	////初始化界面
	m_list.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	LONG lStyle;
	lStyle = GetWindowLong(m_list.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_list.m_hWnd, GWL_STYLE, lStyle);//设置style


	//添加列
	m_list.InsertColumn( 0, _T("文件名"),LVCFMT_CENTER, 150);//插入列
	m_list.InsertColumn(1,_T("url"),LVCFMT_CENTER,50);        
	m_list.InsertColumn(2,_T("进度"),LVCFMT_CENTER,100);    
	m_list.InsertColumn(3,_T("大小"),LVCFMT_CENTER, 100);
	m_list.InsertColumn(4,_T("状态"),LVCFMT_CENTER, 100);


	::SetTimer(m_hWnd,1,1000,NULL);//启动定时器1,定时时间是1秒

	return TRUE;  // return TRUE unless you set the focus to a control

}

void CDownLoadList::SetData(CHttp* pData,	CGuangChangWuDlg* pGcwDlg)
{		
	m_pData = pData;
	m_pGcwDlg = pGcwDlg;

}



void CDownLoadList::OnBnClickedOk()
{		

	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}



void CDownLoadList::Refresh()
{		
	vector<MyParams> result ;
	m_pData->GetFiles(result);


	for (int i=0 ;i<result.size(); ++i)
	{
		MyParams a_data = result[i];


		int nCount = m_list.GetItemCount();//获取当前已插入的行数

		if(i >= nCount)
		{
			m_list.InsertItem(i, _T("呵呵"));//插入一行
		}


		wstringstream oss, oss_size ;

		oss<<a_data.percent <<"%";
		oss_size<<a_data.size;
		//oss_state<<a_data.state;
		wstring wfile,wurl;
		CBase64::StringToWString(a_data.file,wfile);
		CBase64::StringToWString(a_data.ori_url,wurl);
		m_list.SetItemText(i, 0,wfile.c_str());//第一列
		m_list.SetItemText(i, 1, wurl.c_str());//第二列
		m_list.SetItemText(i, 2, oss.str().c_str());//第二列
		m_list.SetItemText(i, 3, oss_size.str().c_str());//第二列
		m_list.SetItemText(i, 4, CHttp::State2Str(a_data.state).GetBuffer());//第二列


	}

	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
}

// 
// void CDownLoadList::Video2Mp3(CString video_path,CString ffmpeg_path)
// {
// 
// 	//CString ffmpeg_path = m_bin_dir + TEXT("\\ffmpeg\\ffmpeg.exe");
// 
// 	CString InputFilePath = video_path;
// 	CString OutPutFilePath = video_path + TEXT(".mp3");	
// 
// 	CString cmd_line = ffmpeg_path + TEXT(" -i \"" + InputFilePath + "\" -acodec libmp3lame -ac 2 -ab 128k -vn -y \"" + OutPutFilePath + "\"");
// 
// 
// 	//AfxMessageBox(cmd_line);
// 
// 	STARTUPINFO   si;//创建进程所需的信息结构变量    
// 	GetStartupInfo(&si);    
// 	si.lpReserved=NULL;    
// 	si.lpDesktop=NULL;    
// 	si.lpTitle=NULL;    
// 	si.dwX=0;    
// 	si.dwY=0;    
// 	si.dwXSize=0;    
// 	si.dwYSize=0;    
// 	si.dwXCountChars=500;    
// 	si.dwYCountChars=500;    
// 	si.dwFlags=STARTF_USESHOWWINDOW;    
// 	si.wShowWindow=SW_HIDE;    
// 	//说明进程将以隐藏的方式在后台执行    
// 	si.cbReserved2=0;    
// 	si.lpReserved2=NULL;    
// 	si.hStdInput=stdin;    
// 	si.hStdOutput=stdout;    
// 	si.hStdError=stderr;    
// 
// 
// 
// 
// 	PROCESS_INFORMATION pi;
// 	ZeroMemory( &si, sizeof(si) );
// 	si.cb = sizeof(si);
// 	ZeroMemory( &pi, sizeof(pi) );
// 
// 	// Start the child process.
// 	if( !CreateProcess( NULL,   // No module name (use command line)
// 		cmd_line.GetBuffer(),                  // Command line
// 		NULL,                  // Process handle not inheritable
// 		NULL,                 // Thread handle not inheritable
// 		FALSE,                // Set handle inheritance to FALSE
// 		CREATE_NO_WINDOW,                      // No creation flags
// 		NULL,                 // Use parent's environment block
// 		NULL,                 // Use parent's starting directory
// 		&si,                    // Pointer to STARTUPINFO structure
// 		&pi )                  // Pointer to PROCESS_INFORMATION structure
// 		)
// 	{
// 		CString str_errror ;
// 		str_errror.Format(TEXT("CreateProcess failed (%d).\n"), GetLastError() );
// 
// 		AfxMessageBox(str_errror);
// 
// 		return;
// 	}
// 	// Wait until child process exits.
// 	WaitForSingleObject( pi.hProcess, 0 );
// 	// Close process and thread handles.
// 	CloseHandle( pi.hProcess );
// 	CloseHandle( pi.hThread );
// }




void CDownLoadList::PauseOne(int nIndex)
{		
	MyParams* a_data = m_pData->GetData(nIndex);
	if (a_data)
	{
		a_data->command = COMMAND_CANCEL; //设置为停止
	}
	

}


void CDownLoadList::Mp3One(int nIndex)
{		
	MyParams* a_data = m_pData->GetData(nIndex);
	if (a_data)
	{
		if (a_data->state == DSTATE_DOWN_FINISH) //下载完成才给转mp3
		{
			a_data->command = COMMAND_2MP3; //设置为转换mp3 命令
			a_data->state = DSTATE_MP3_WAITING;
		}
		else
		{
			AfxMessageBox(TEXT("该视频暂时不能提取mp3"));
		}
		
	}

}


void CDownLoadList::ContinueOne(int nIndex)
{		
	MyParams* a_data = m_pData->GetData(nIndex);
	if (a_data)
	{
		a_data->command = COMMAND_DOWN; //设置为停止
		m_pData->download_file(a_data);
	}


}

void CDownLoadList::RestartOne(int nIndex)
{		
	MyParams* a_data = m_pData->GetData(nIndex);
	if (a_data)
	{
		
		if (CBase64::IsFileExist(a_data->file.c_str()))
		{
			if(!::DeleteFileA(a_data->file.c_str()))
			{
				CString text = _T("文件被占用，无法重新下载"); 	//重新开始的话 需要先删除该文件 再启动
				AfxMessageBox(text);
				return;
			}		
		}
		

		a_data->command = COMMAND_DOWN; //设置为下载
		m_pData->download_file(a_data); //重新开一个线程
	}


}









void CDownLoadList::OnBnClickedButton1()
{



}


void CDownLoadList::OnTimer(UINT_PTR nIDEvent)
{
	Refresh();

	CDialogEx::OnTimer(nIDEvent);
}


void CDownLoadList::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CDownLoadList::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnRButtonDblClk(nFlags, point);
}





void CDownLoadList::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//防止在空白区点击弹出菜单
	if (m_list.GetSelectedCount() <= 0)
	{
		return;
	}
	//下面的这段代码, 不单单适应于ListCtrl
	CMenu menu, *pPopup;
	menu.LoadMenu(IDR_MENU_CARD_INFO);
	pPopup = menu.GetSubMenu(0);
	CPoint myPoint;
	ClientToScreen(&myPoint);
	GetCursorPos(&myPoint); //鼠标位置
	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, myPoint.x, myPoint.y,this);

}



void CDownLoadList::OnPause()
{

	if (m_list.GetSelectedCount() <= 0)
	{
		return;
	}

	POSITION pos = m_list.GetFirstSelectedItemPosition();

	while (pos)
	{
		int nItem = m_list.GetNextSelectedItem(pos);
		PauseOne(nItem); 
	}
}




void CDownLoadList::OnContinue()
{

	if (m_list.GetSelectedCount() <= 0)
	{
		return;
	}

	POSITION pos = m_list.GetFirstSelectedItemPosition();

	while (pos)
	{
		int nItem = m_list.GetNextSelectedItem(pos);
		ContinueOne(nItem); 
	}


}


void CDownLoadList::OnReStart()
{
	if (m_list.GetSelectedCount() <= 0)
	{
		return;
	}

	POSITION pos = m_list.GetFirstSelectedItemPosition();

	while (pos)
	{
		int nItem = m_list.GetNextSelectedItem(pos);
		PauseOne(nItem); //先暂停这个县城
		Sleep(1000);
		RestartOne(nItem); //再重启
	}
}


void CDownLoadList::OnMp3()
{
	m_pData->CreateMp3Theread();
	if (m_list.GetSelectedCount() <= 0)
	{
		return;
	}

	POSITION pos = m_list.GetFirstSelectedItemPosition();

	while (pos)
	{
		int nItem = m_list.GetNextSelectedItem(pos);
		Mp3One(nItem); 
	}
}


// void CDownLoadList::OnMp3()
// {
// 	if (m_list.GetSelectedCount() <= 0)
// 	{
// 		return;
// 	}
// 
// 	POSITION pos = m_list.GetFirstSelectedItemPosition();
// 
// 	//while (pos)//这里只打开第一个
// 	{
// 		int nItem = m_list.GetNextSelectedItem(pos);
// 
// 		MyParams* a_data = m_pData->GetData(nItem);
// 		if (a_data)
// 		{
// 
// 			if (a_data->percent <100 )
// 			{
// 
// 				CString msg_text = CString(TEXT("该视频还未下载完成，不能提取MP3.."));
// 
// 				AfxMessageBox(msg_text);
// 				return;
// 			}
// 
// 
// 			if (!CBase64::IsFileExist(a_data->file.c_str()))
// 			{
// 				CString msg_text = CString(TEXT("视频文件已丢失，无法提取"));
// 				AfxMessageBox(msg_text);
// 				return;
// 			}
// 
// 
// 			CString bin_dir = m_pGcwDlg->GetBinDir();
// 
// 			CString ffmpeg_path = bin_dir + TEXT("\\ffmpeg\\ffmpeg.exe");
// 
// 
// 			wstring wfile;
// 			CBase64::StringToWString(a_data->file,wfile);	
// 
// 			
// 			Video2Mp3(wfile.c_str(), ffmpeg_path);
// 
// 			CString msg_text = CString(TEXT("正在转换..")) + CString( wfile.c_str()) + CString(TEXT(".mp3"));
// 
// 			AfxMessageBox(msg_text);
// 		}
// 
// 
// 
// 
// 	}
// 
// }


	void CDownLoadList::OnOpendir()
	{

		if (m_list.GetSelectedCount() <= 0)
		{
			return;
		}

		POSITION pos = m_list.GetFirstSelectedItemPosition();

		//while (pos)//这里只打开第一个
		{
			int nItem = m_list.GetNextSelectedItem(pos);


			vector<MyParams> result ;
			m_pData->GetFiles(result);

			if (nItem >= result.size())
			{
				return;
			}


			MyParams a_data = result[nItem];

			string params = (string)"/e,/select," + a_data.file;

			// 打开文件所在目录
			ShellExecuteA(NULL,NULL,"explorer", params.c_str() ,NULL, SW_SHOW);
		}
	}


