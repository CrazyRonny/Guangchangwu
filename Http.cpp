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


//�����������Ի���Ĵ��ھ������Ϊ����Ҫ��������ڷ���Ϣ������֪���䴰�ھ��
HWND	g_hDlgWnd = NULL;
//�ṩ��CURL���ؽ��Ȼص��ĺ��������ڱ������ص����ݵ��ļ�
static size_t	DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
//�ṩ��CURL���ؽ��Ȼص��ĺ��������ڼ������ؽ���֪ͨ����
static int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

//��������ص��̺߳�����Ϊ�˲��ѶԻ��������濨�����϶���Ҫ�Լ����߳������ص���
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
		if (http_code != 200) //�ɹ��� �п��ܷ���403 ʲô��
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

	//��ʼ��curl������Ǳ����
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, pm->video_real_url.c_str());
	//���ý������ݵĻص�
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DownloadCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)fp);
	//curl_easy_setopt(curl, CURLOPT_INFILESIZE, lFileSize);
	//curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	//curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
	//curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	// �����ض����������
	curl_easy_setopt(curl, CURLOPT_USERAGENT, HTTP_UA);
	//curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
	// ����301��302��ת����location
	//curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
	//���ý��Ȼص�����
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	curl_easy_setopt(curl,  CURLOPT_PROGRESSDATA, (LPVOID)pm);
	//curl_easy_getinfo(curl,  CURLINFO_CONTENT_LENGTH_DOWNLOAD, &lFileSize);
	//curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, g_hDlgWnd);
	//curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);

// 	long localFileLenth = getlength(pm->file); //��ȡ�����ļ���С
// 
// 	if(localFileLenth > 0)
// 	{
// 		curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, localFileLenth);
// 	}


	//��ʼִ������
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


	//����curl����ǰ��ĳ�ʼ��ƥ��
	curl_easy_cleanup(curl);

	fclose(fp);
	return retcCode;
}



/************************************************************************/
/* ��ȡҪ���ص�Զ���ļ��Ĵ�С 											*/
/************************************************************************/
long CHttp::getDownloadFileLenth(const char *url){
	long downloadFileLenth = 0;
	CURL *handle = curl_easy_init();
	curl_easy_setopt(handle, CURLOPT_URL, url);
	curl_easy_setopt(handle, CURLOPT_HEADER, 1);    //ֻ��Ҫheaderͷ
	curl_easy_setopt(handle, CURLOPT_NOBODY, 1);    //����Ҫbody
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
/*  ��ȡ�����ļ���С�ĺ���    											*/
/*  ʧ���򷵻�-1         ��i/oһ��           */
/************************************************************************/
long CHttp::getlength( string file)
{
	long len = -1;

	FILE* fp = NULL;
	fopen_s(&fp, file.c_str(), "ab+");

	if (fp)
	{
		long cur_pos;
		//ȡ�õ�ǰ�ļ����Ķ�ȡλ��
		cur_pos = ftell( fp );
		//���ļ����Ķ�ȡλ����Ϊ�ļ�ĩβ
		fseek( fp, 0, SEEK_END );
		//��ȡ�ļ�ĩβ�Ķ�ȡλ��,���ļ���С
		len = ftell( fp );
		//���ļ����Ķ�ȡλ�û�ԭΪԭ�ȵ�ֵ
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
	a_file.video_real_url = ""; //��ʼΪ�� ��Ҫ������һ�� ���ܻ��������ַ
	a_file.command = COMMAND_DOWN; //��ʼΪ����
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
	//�����ص���������׷�ӵķ�ʽд���ļ�(һ��Ҫ��a������ǰ��д������ݾͻᱻ������)

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
		return 1; //libcurl �з���1Ϊ��ֹ
	}

	pm->state = DSTATE_DOWN_ING;

	if ( dltotal > -0.1 && dltotal < 0.1 )
		return 0;
	int nPos = (int) ( (dlnow/dltotal)*100 );
	

	pm->percent = nPos;
	pm->size = dltotal;

	//֪ͨ�������������ؽ���
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
		int ret = 	CHttp::http_get(pm->ori_url.c_str(),result); //�Ȳ�ѯ�ӿ� �õ�������Ƶ��ַ

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
		return result; //�ļ�������";
	}
	else
	{
		fs.close();

		int find_index = result.rfind('.');
		if (string::npos != find_index)
		{
			result.insert(find_index,"_1");
		}

		return CreateFileName(result.c_str() );//"�Ѿ�����";
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

	STARTUPINFOA   si;//���������������Ϣ�ṹ����    
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
	//˵�����̽������صķ�ʽ�ں�ִ̨��    
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
// DSTATE_INIT,        //��ʼ״̬
// DSTATE_GETINFO_ING, //��ȡ��Ƶ��Ϣ ING
// DSTATE_GETINFO_END, //��ȡ��Ƶ��Ϣ END
// DSTATE_GETINFO_FAILED, //��ȡ��Ƶ��Ϣ FAILED
// DSTATE_DOWN_ING,	//����
// DSTATE_DOWN_FINISH, //���
// DSTATE_DOWN_FAILED, //����ʧ��
// DSTATE_DOWN_CANCEL, //ȡ��
// DSTATE_MP3_ING,		//ת��mp3 ing
// DSTATE_MP3_END,		//ת��mp3 ���
// DSTATE_MP3_FAILED_NOFILE,	//ת��mp3 ʧ��
// DSTATE_MP3_FAILED_CREATE,
// DSTATE_MP3_FAILED_TIMEOUT,
// DSTATE_COUNT,

	static TCHAR* pData[] = 
	{TEXT("��ʼ״̬"),
	TEXT("��ȡ��Ƶ��Ϣ��"),
	TEXT("��ȡ��Ƶ��Ϣ�ɹ�"),
	TEXT("��ȡ��Ƶ��Ϣʧ��"),
	TEXT("��������"),
	TEXT("�������"),
	TEXT("����ʧ��"),
	TEXT("�û�ȡ������"),
	TEXT("����ת��mp3"),
	TEXT("ת��mp3 ���"),
	TEXT("ת��mp3ʧ�ܣ��Ҳ����ļ�"),
	TEXT("ת��mp3ʧ��"),
	TEXT("���Ժ��Ѳ���mp3����"),
	TEXT("ת��mp3ʧ�ܣ�ץ����ʱ")

	};

	if (state >= DSTATE_COUNT)
	{
		return TEXT("");
	}

	return pData[state];
}
