#pragma once

#include<string>
#include <vector>


using namespace std;


enum DownLoadCommand
{
	COMMAND_DOWN, //����
	COMMAND_CANCEL, //ȡ��
	COMMAND_2MP3, //ת��MP3

};

//״̬
enum DState
{
	DSTATE_INIT,        //��ʼ״̬
	DSTATE_GETINFO_ING, //��ȡ��Ƶ��Ϣ ING
	DSTATE_GETINFO_END, //��ȡ��Ƶ��Ϣ END
	DSTATE_GETINFO_FAILED, //��ȡ��Ƶ��Ϣ FAILED
	DSTATE_DOWN_ING,	//����
	DSTATE_DOWN_FINISH, //���
	DSTATE_DOWN_FAILED, //����ʧ��
	DSTATE_DOWN_CANCEL, //ȡ��
	DSTATE_MP3_ING,		//ת��mp3 ing
	DSTATE_MP3_END,		//ת��mp3 ���
	DSTATE_MP3_FAILED_NOFILE,	//ת��mp3 ʧ��
	DSTATE_MP3_FAILED_CREATE,
	DSTATE_MP3_WAITING,//�Ѳ�����У����Ժ�
	DSTATE_MP3_FAILED_TIMEOUT,
	DSTATE_COUNT,

};

struct MyParams
{
	string file;	//�ļ�·��
	string video_real_url;		//��ԭʼ��Ƶ��ַ����ӿں�õ����µ�ַ
	string ori_url; //ԭʼ��Ƶ��ַ
	int size ;		//�ļ���С
	int down_size;	//�����ļ���С
	int percent;	//���ذٷֱ�
	DownLoadCommand command;	//��һ������
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

