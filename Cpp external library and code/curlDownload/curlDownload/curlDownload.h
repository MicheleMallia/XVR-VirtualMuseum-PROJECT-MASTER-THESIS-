#pragma once


//make function readable by XVR
extern "C" __declspec(dllexport) int curlDownload(char* url, char* path);