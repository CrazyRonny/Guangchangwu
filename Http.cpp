#include "stdafx.h"
#include "Http.h"
#include "curl/curl.h"

#include <Windows.h>
#include <CommCtrl.h>
#include "resource.h"
#include <sstream>
#include <fstream>
#include "Base64.h"



#define HTTP_UA		"Mozilla"

#pragma comment(lib,"libcurl.lib")


//这个用来保存对话框的窗口句柄，因为后面要向这个窗口发消息，必须知道其窗口句柄
HWND	g_hDlgWnd = NULL;
//提供给CURL下载进度回调的函数，用于保存下载的数据到文件
static size_t	DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
//提供给CURL下载进度回调的函数，用于计算下载进度通知界面
static int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

//这个是下载的线程函数，为了不把对话框主界面卡死，肯定是要自己开线程来下载的了
DWORD WINAPI DownloadThread(LPVOID lpParam);




CHttp::CHttp(void)
{

	m_bMp3Flag =false;
	curl_global_init(CURL_GLOBAL_ALL);

}


CHttp::~CHttp(void)
{
		curl_global_cleanup();
}





struct MemoryStruct {
	char *memory;
	size_t size;
};

static size_t
	WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

int CHttp::http_get(const char* url,  string & result)
{
	result = "";
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;

	chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;    /* no data at this point */



	/* init the curl session */
	curl_handle = curl_easy_init();

	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);

	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

	/* some servers don't like requests that are made without a user-agent
	field, so we provide one */
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, HTTP_UA);

	/* get it! */
	res = curl_easy_perform(curl_handle);



	/* check for errors */
	if(res != CURLE_OK  ) {

		char a[128] ={0};
		sprintf_s(a,"error:%d ",res);
		result = a;

		//     fprintf(stderr, "curl_easy_perform() failed: %s\n",
		//             curl_easy_strerror(res));
	}
	else {

		long http_code = 0;
		curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
		if (http_code != 200) //成功了 有可能返回403 什么的
		{
			ostringstream oss;
			oss << "error_status_code_"<<http_code;

			result = oss.str();
			curl_easy_cleanup(curl_handle);
			return  -1;
		}

		result = string((char *)chunk.memory, chunk.size);

	}

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);

	free(chunk.memory);

	/* we're done with libcurl, so clean it up */
	//curl_global_cleanup();

	return res;
}



int CHttp::download(LPVOID lpParam)
{

	MyParams* pm = (MyParams*)lpParam;

	string url = pm->video_real_url;
	string file = pm->file;

	
	FILE* fp = NULL;
	fopen_s(&fp, file.c_str(), "ab+");

	//初始化curl，这个是必须的
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, pm->video_real_url.c_str());
	//设置接收数据的回调
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DownloadCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)fp);
	//curl_easy_setopt(curl, CURLOPT_INFILESIZE, lFileSize);
	//curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	//curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
	//curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	// 设置重定向的最大次数
	curl_easy_setopt(curl, CURLOPT_USERAGENT, HTTP_UA);
	//curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
	// 设置301、302跳转跟随location
	//curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
	//设置进度回调函数
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	curl_easy_setopt(curl,  CURLOPT_PROGRESSDATA, (LPVOID)pm);
	//curl_easy_getinfo(curl,  CURLINFO_CONTENT_LENGTH_DOWNLOAD, &lFileSize);
	//curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, g_hDlgWnd);
	//curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);

// 	long localFileLenth = getlength(pm->file); //获取本地文件大小
// 
// 	if(localFileLenth > 0)
// 	{
// 		curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, localFileLenth);
// 	}


	//开始执行请求
	CURLcode retcCode = curl_easy_perform(curl);

	long http_code = 0;
	curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

	pm->state = DSTATE_DOWN_FAILED;
	if (retcCode ==  CURLE_ABORTED_BY_CALLBACK)
	{
		pm->state = DSTATE_DOWN_CANCEL;
	}
	else if (retcCode ==  CURLE_OK)
	{
		if (http_code == 200)
		{
			pm->state = DSTATE_DOWN_FINISH;
		}
		else
		{
			retcCode = CURLE_OBSOLETE57;
		}
	}


	//清理curl，和前面的初始化匹配
	curl_easy_cleanup(curl);

	fclose(fp);
	return retcCode;
}



/************************************************************************/
/* 获取要下载的远程文件的大小 											*/
/************************************************************************/
long CHttp::getDownloadFileLenth(const char *url){
	long downloadFileLenth = 0;
	CURL *handle = curl_easy_init();
	curl_easy_setopt(handle, CURLOPT_URL, url);
	curl_easy_setopt(handle, CURLOPT_HEADER, 1);    //只需要header头
	curl_easy_setopt(handle, CURLOPT_NOBODY, 1);    //不需要body
	if (curl_easy_perform(handle) == CURLE_OK) 
	{
		curl_easy_getinfo(handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLenth);
	} 
	else 
	{
		downloadFileLenth = -1;
	}
	return downloadFileLenth;
}


/************************************************************************/
/*  获取本地文件大小的函数    											*/
/*  失败则返回-1         会i/o一次           */
/************************************************************************/
long CHttp::getlength( string file)
{
	long len = -1;

	FILE* fp = NULL;
	fopen_s(&fp, file.c_str(), "ab+");

	if (fp)
	{
		long cur_pos;
		//取得当前文件流的读取位置
		cur_pos = ftell( fp );
		//将文件流的读取位置设为文件末尾
		fseek( fp, 0, SEEK_END );
		//获取文件末尾的读取位置,即文件大小
		len = ftell( fp );
		//将文件流的读取位置还原为原先的值
		fseek( fp, cur_pos, SEEK_SET );

		fclose(fp);
	}
	
	return len;
}



int CHttp::download_file(const char* url,  const string  filename)
{
	
	ostringstream oss;

	MyParams a_file;
	a_file.file = filename;
	a_file.ori_url = url;
	a_file.video_real_url = ""; //初始为空 需要再请求一次 才能获得真正地址
	a_file.command = COMMAND_DOWN; //初始为下载
	a_file.down_size=0;
	a_file.percent =0;
	a_file.size = 0;
	a_file.state = DSTATE_INIT;
	a_file.mp3_state = DSTATE_INIT;
	

	files.push_back(a_file);
	
	return download_file((LPVOID)&files.back());

}

int CHttp::download_file(LPVOID p_a_file)
{

	HANDLE hThread = CreateThread(NULL, 0, DownloadThread, (LPVOID)p_a_file, 0, NULL);
	CloseHandle(hThread);
	return 1;
}



int CHttp::CheckUrl(const string  ori_url)
{
	for (int i=0;i<files.size(); i++)
	{
		MyParams mp = files[i];
		if (mp.ori_url == ori_url)
		{
			return 0;
		}
	}

	return 1;
}


void CHttp::GetFiles(vector<MyParams>& result)
{
	result = files;
}

MyParams* CHttp::GetData(int index)
{
	if (index >= files.size())
	{
		return  NULL;
	}
	
	return &files[index];
}



static size_t DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	//把下载到的数据以追加的方式写入文件(一定要有a，否则前面写入的内容就会被覆盖了)

	CString strTemp;
	strTemp.Format(TEXT("DownloadCallback %d,%d,%d"), (int)pBuffer,nSize,nMemByte);
	OutputDebugString(strTemp);

	FILE* fp = ((FILE*) pParam);


	size_t nWrite = 0;
	if (fp)
	{
		nWrite = fwrite(pBuffer, nSize, nMemByte, fp);
	}

	return nWrite;
}

static int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{

	CString strTemp;
	strTemp.Format(TEXT("ProgressCallback %d,%d,%d,%d"), dltotal,dlnow,ultotal, ulnow);
	OutputDebugString(strTemp);


	MyParams* pm  =(MyParams*) clientp ;

	if (pm->command == COMMAND_CANCEL)
	{
		return 1; //libcurl 中返回1为终止
	}

	pm->state = DSTATE_DOWN_ING;

	if ( dltotal > -0.1 && dltotal < 0.1 )
		return 0;
	int nPos = (int) ( (dlnow/dltotal)*100 );
	

	pm->percent = nPos;
	pm->size = dltotal;

	//通知进度条更新下载进度
	//::PostMessage(g_hDlgWnd, WM_USER + 110, nPos, 0);
	//::Sleep(10);
	return 0;
}

DWORD WINAPI DownloadThread(LPVOID lpParam)
{

	try
	{
		MyParams* pm = (MyParams*)lpParam;

		string result ;

		pm->state = DSTATE_GETINFO_ING;
		int ret = 	CHttp::http_get(pm->ori_url.c_str(),result); //先查询接口 得到真正视频地址

		if (ret == 0)
		{		
			pm->state = DSTATE_GETINFO_END;
			pm->video_real_url = result;

			CHttp::download(lpParam);
		}
		else
		{
			pm->state = DSTATE_GETINFO_FAILED;
		}


		return 0;
	}

	catch (CException* e)
	{
		return 0;
	}

}



string CreateFileName(const char* file_name)
{

	string result = file_name;
	fstream fs;
	fs.open(file_name, ios::in);
	if(!fs)
	{
		fs.close();
		return result; //文件不存在";
	}
	else
	{
		fs.close();

		int find_index = result.rfind('.');
		if (string::npos != find_index)
		{
			result.insert(find_index,"_1");
		}

		return CreateFileName(result.c_str() );//"已经存在";
	}

}




DState Video2Mp3(const char* video_path,const char* ffmpeg_path)
{

	//CString ffmpeg_path = m_bin_dir + TEXT("\\ffmpeg\\ffmpeg.exe");

	if (!CBase64::IsFileExist(video_path))
	{
		return DSTATE_MP3_FAILED_NOFILE;
	}
	

	string InputFilePath = video_path;
	string OutPutFilePath = InputFilePath + ".mp3";	

	string cmd_line = ffmpeg_path + (" -i \"" + InputFilePath + "\" -acodec libmp3lame -ac 2 -ab 128k -vn -y \"" + OutPutFilePath + "\"");


	//AfxMessageBox(cmd_line);

	STARTUPINFOA   si;//创建进程所需的信息结构变量    
	GetStartupInfoA(&si);    
	si.lpReserved=NULL;    
	si.lpDesktop=NULL;    
	si.lpTitle=NULL;    
	si.dwX=0;    
	si.dwY=0;    
	si.dwXSize=0;    
	si.dwYSize=0;    
	si.dwXCountChars=500;    
	si.dwYCountChars=500;    
	si.dwFlags=STARTF_USESHOWWINDOW;    
	si.wShowWindow=SW_HIDE;    
	//说明进程将以隐藏的方式在后台执行    
	si.cbReserved2=0;    
	si.lpReserved2=NULL;    
	si.hStdInput=stdin;    
	si.hStdOutput=stdout;    
	si.hStdError=stderr;    




	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	// Start the child process.
	if( !CreateProcessA( NULL,   // No module name (use command line)
		(char*)cmd_line.c_str(),                  // Command line
		NULL,                  // Process handle not inheritable
		NULL,                 // Thread handle not inheritable
		FALSE,                // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,                      // No creation flags
		NULL,                 // Use parent's environment block
		NULL,                 // Use parent's starting directory
		&si,                    // Pointer to STARTUPINFO structure
		&pi )                  // Pointer to PROCESS_INFORMATION structure
		)
	{
// 		CString str_errror ;
// 		str_errror.Format(TEXT("CreateProcess failed (%d).\n"), GetLastError() );
// 
// 		AfxMessageBox(str_errror);

		return DSTATE_MP3_FAILED_CREATE;
	}
	// Wait until child process exits.

	if (WAIT_TIMEOUT == WaitForSingleObject( pi.hProcess, 180*1000 ))
	{	// Close process and thread handles.
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );

		return DSTATE_MP3_FAILED_TIMEOUT;
	}

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	
	return DSTATE_MP3_END;
}

DWORD WINAPI Mp3ThereadCallback(LPVOID lpParam)
{

	vector<MyParams>* p_files  =(vector<MyParams>*)lpParam;

	while (true)
	{
		for (int i=0; i< p_files->size(); i++)
		{
			MyParams* a_data = &((*p_files)[i]);

			if (a_data->command == COMMAND_2MP3)
			{
				a_data->state = DSTATE_MP3_ING;
				a_data->state = Video2Mp3(a_data->file.c_str(), ".\\ffmpeg\\ffmpeg.exe");
				a_data->command = COMMAND_DOWN;
			}

		}

		Sleep(1000);
	}


	

}


int CHttp::CreateMp3Theread()
{
	if (!m_bMp3Flag)
	{
		HANDLE hThread = CreateThread(NULL, 0, Mp3ThereadCallback, (LPVOID)&files, 0, NULL);
		CloseHandle(hThread);
	}
	return 1;
}


CString CHttp::State2Str(DState state)
{
// DSTATE_INIT,        //初始状态
// DSTATE_GETINFO_ING, //获取视频信息 ING
// DSTATE_GETINFO_END, //获取视频信息 END
// DSTATE_GETINFO_FAILED, //获取视频信息 FAILED
// DSTATE_DOWN_ING,	//下载
// DSTATE_DOWN_FINISH, //完成
// DSTATE_DOWN_FAILED, //下载失败
// DSTATE_DOWN_CANCEL, //取消
// DSTATE_MP3_ING,		//转换mp3 ing
// DSTATE_MP3_END,		//转换mp3 完成
// DSTATE_MP3_FAILED_NOFILE,	//转换mp3 失败
// DSTATE_MP3_FAILED_CREATE,
// DSTATE_MP3_FAILED_TIMEOUT,
// DSTATE_COUNT,

	static TCHAR* pData[] = 
	{TEXT("初始状态"),
	TEXT("获取视频信息中"),
	TEXT("获取视频信息成功"),
	TEXT("获取视频信息失败"),
	TEXT("正在下载"),
	TEXT("下载完成"),
	TEXT("下载失败"),
	TEXT("用户取消下载"),
	TEXT("正在转换mp3"),
	TEXT("转换mp3 完成"),
	TEXT("转换mp3失败，找不到文件"),
	TEXT("转换mp3失败"),
	TEXT("请稍候，已插入mp3队列"),
	TEXT("转换mp3失败，抓换超时")

	};

	if (state >= DSTATE_COUNT)
	{
		return TEXT("");
	}

	return pData[state];
}
