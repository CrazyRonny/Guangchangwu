#pragma once

#include<string>
#include <vector>


using namespace std;


enum DownLoadCommand
{
	COMMAND_DOWN, //下载
	COMMAND_CANCEL, //取消
	COMMAND_2MP3, //转换MP3

};

//状态
enum DState
{
	DSTATE_INIT,        //初始状态
	DSTATE_GETINFO_ING, //获取视频信息 ING
	DSTATE_GETINFO_END, //获取视频信息 END
	DSTATE_GETINFO_FAILED, //获取视频信息 FAILED
	DSTATE_DOWN_ING,	//下载
	DSTATE_DOWN_FINISH, //完成
	DSTATE_DOWN_FAILED, //下载失败
	DSTATE_DOWN_CANCEL, //取消
	DSTATE_MP3_ING,		//转换mp3 ing
	DSTATE_MP3_END,		//转换mp3 完成
	DSTATE_MP3_FAILED_NOFILE,	//转换mp3 失败
	DSTATE_MP3_FAILED_CREATE,
	DSTATE_MP3_WAITING,//已插入队列，请稍候
	DSTATE_MP3_FAILED_TIMEOUT,
	DSTATE_COUNT,

};

struct MyParams
{
	string file;	//文件路径
	string video_real_url;		//用原始视频地址请求接口后得到的新地址
	string ori_url; //原始视频地址
	int size ;		//文件大小
	int down_size;	//下载文件大小
	int percent;	//下载百分比
	DownLoadCommand command;	//下一个命令
	DState state;
	DState mp3_state;
	FILE * pFile;
};




class CHttp
{



public:
	CHttp(void);
	~CHttp(void);


public:


	static int http_get(const char* url,  string & result);
	static int download(LPVOID lpParam);

	static long getDownloadFileLenth(const char *url);


	static long CHttp::getlength( string file);

	static CString State2Str(DState state);

	int download_file(const char* url,  const string  filename);

	int download_file(LPVOID p_a_file);
	int CreateMp3Theread();

	int CheckUrl(const string  ori_url);

	void GetFiles(vector<MyParams>& result);

	MyParams* GetData(int index);

private:
	vector<MyParams> files;
	bool m_bMp3Flag;

};

