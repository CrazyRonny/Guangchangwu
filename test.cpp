#include <Windows.h>
#include "curl/curl.h"
#include<string>
#include<iostream>
using namespace std;

#pragma comment(lib,"libcurl.lib")

size_t write_string(void *buffer, size_t size,size_t nmemb, void* userp)
{
	int segsize = size * nmemb;
	*(string*)userp += string((char*)buffer);
	return segsize;
}

int http_get(const char* url,  string & result);

int main(int argc, char* argv[])
{

	string tmp ;

	int ret = http_get("www.baidu.com",  tmp);


	if (ret != 0)
	{
		return ret;
	}

	printf(tmp.c_str());

	getchar();
}

int http_get(const char* url,  string & result)
{
	CURL *curl;
	CURLcode res;
	string tmp;

	curl = curl_easy_init();

	if (!curl)
	{
		return -1;
	}

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &tmp);


		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,write_string);

		res = curl_easy_perform(curl);

		if (res == CURLE_OK)
		{
			result = tmp;
		}
		else
		{
			char a[16] ={0};
			sprintf(a,"error:%d",res);
			result = a;
			
		}
		curl_easy_cleanup(curl);



	}

	return res;
}
