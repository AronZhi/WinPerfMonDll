#pragma once
#include <string>
#include <time.h>

bool WsToS(const std::wstring& src, std::string& ret);
bool SToWs(const std::string& src, std::wstring& ret);
time_t LocalTimeStampNow();