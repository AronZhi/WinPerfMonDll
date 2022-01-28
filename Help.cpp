#include "pch.h"
#include "Help.h"

#include <locale>
#include <codecvt>

bool WsToS(const std::wstring& src, std::string& ret)
{
	try
	{
		ret.clear();
		std::wstring_convert<std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.to_bytes(src);
		return true;
	}
	catch (const std::exception& e)
	{
		throw e;
	}
	return false;
}

bool SToWs(const std::string& src, std::wstring& ret)
{
	try
	{
		ret.clear();
		std::wstring_convert<std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.from_bytes(src);
		return true;
	}
	catch (const std::exception& e)
	{
		throw e;
	}
	return false;
}

time_t LocalTimeStampNow()
{
	time_t timep;
	time(&timep);
	return timep;
}