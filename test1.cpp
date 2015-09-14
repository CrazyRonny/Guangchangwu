#include <Windows.h>
#include <CommCtrl.h>
#include "resource.h"
#include "curl\curl.h"

#pragma comment(lib, "libcurl")



//这个用来保存对话框的窗口句柄，因为后面要向这个窗口发消息，必须知道其窗口句柄
HWND	g_hDlgWnd = NULL;
//提供给CURL下载进度回调的函数，用于保存下载的数据到文件
static size_t	DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
//提供给CURL下载进度回调的函数，用于计算下载进度通知界面
static int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
//这是对话框的消息循环，在控制台程序里面创建GUI，仅仅是为了更好地展现下载回调这个功能
INT_PTR CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//这个是下载的线程函数，为了不把对话框主界面卡死，肯定是要自己开线程来下载的了
DWORD WINAPI DownloadThread(LPVOID lpParam);

int main(int argc, wchar_t* argv[])
{
	//弹出对话框，知道对话框关闭才会执行退出
	DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc );
	return 0;
}

static size_t DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	//把下载到的数据以追加的方式写入文件(一定要有a，否则前面写入的内容就会被覆盖了)
	FILE* fp = NULL;
	fopen_s(&fp, "d:\\test.apk", "ab+");
	size_t nWrite = fwrite(pBuffer, nSize, nMemByte, fp);
	fclose(fp);
	return nWrite;
}

static int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	if ( dltotal > -0.1 && dltotal < 0.1 )
		return 0;
	int nPos = (int) ( (dlnow/dltotal)*100 );
	//通知进度条更新下载进度
	::PostMessage(g_hDlgWnd, WM_USER + 110, nPos, 0);
	//::Sleep(10);
	return 0;
}

INT_PTR CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			g_hDlgWnd = hWnd;
			HWND hProgress = GetDlgItem(hWnd, IDC_PROGRESS1);
			SendMessage(hProgress, PBM_SETRANGE32, (WPARAM) 0, (LPARAM) 100);
			//对话框初始化时创建下载线程
			HANDLE hThread = CreateThread(NULL, 0, DownloadThread, 0, 0, NULL);
			CloseHandle(hThread);
			::SetWindowText(hWnd, L"使用CURL下载文件示例：");
			return TRUE;
		}
	case WM_COMMAND:
		{
			WORD  msg = HIWORD(wParam);
			WORD  id  = LOWORD(wParam);
			if ( id == IDOK || id == IDCANCEL )
				EndDialog(hWnd, id);
			break;
		}
	case WM_ERASEBKGND:
		return TRUE;
	case WM_CTLCOLORSTATIC:
		return (INT_PTR)(HBRUSH)::GetStockObject(WHITE_BRUSH);
	case WM_USER + 110:
		{//接收到设置进度的消息
			HWND	hProgress	= GetDlgItem(hWnd, IDC_PROGRESS1);
			HWND	hStatus		= GetDlgItem(hWnd, IDC_STATUS);
			if ( hProgress )
				SendMessage(hProgress, PBM_SETPOS, wParam, 0L);
			if ( hStatus )
			{
				WCHAR szBuffer[100] = {0};
				if ( wParam<100 )
					swprintf(szBuffer, L"正在下载文件，进度：%d%%", wParam);
				else
					swprintf(szBuffer, L"文件下载完毕！");
				::SetWindowText(hStatus, szBuffer);
			}
			return 0;
		}
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI DownloadThread(LPVOID lpParam)
{
	//初始化curl，这个是必须的
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, "http://android.shoujids.com/software/download?id=154103");
	//设置接收数据的回调
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DownloadCallback);
	//curl_easy_setopt(curl, CURLOPT_INFILESIZE, lFileSize);
	//curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	//curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
	//curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	// 设置重定向的最大次数
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "123as");
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
	// 设置301、302跳转跟随location
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
	//设置进度回调函数
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	//curl_easy_getinfo(curl,  CURLINFO_CONTENT_LENGTH_DOWNLOAD, &lFileSize);
	//curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, g_hDlgWnd);
	//开始执行请求
	CURLcode retcCode = curl_easy_perform(curl);
	//查看是否有出错信息
	const char* pError = curl_easy_strerror(retcCode);
	//清理curl，和前面的初始化匹配
	curl_easy_cleanup(curl);
	return 0;
}