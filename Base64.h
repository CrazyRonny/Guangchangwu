#pragma once
#include <string>


class CBase64
{
public:
	CBase64(void);
	~CBase64(void);




	static std::string base64_encode(unsigned char const* , unsigned int len);
	static std::string base64_decode(std::string const& s);

	static BOOL WStringToString(const std::wstring &wstr,std::string &str);

	static BOOL StringToWString(const std::string &str,std::wstring &wstr);

	static bool IsFileExist(const char* file_name);


};

