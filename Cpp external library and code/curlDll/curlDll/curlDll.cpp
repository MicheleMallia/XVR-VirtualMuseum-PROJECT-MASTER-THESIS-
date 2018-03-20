// curlDll.cpp: definisce il punto di ingresso dell'applicazione console.
//
#include "stdafx.h"
#include <iostream>
#include <string>
#include <curl.h>
#include <fstream>
#include "curlDll.h"

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

char returnval[1256000];		


//store data into variable
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}


//main function
char* decripta(char* s) {
	

	//CURL object
	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	/*
	to see the output of body in .txt file

		std::ofstream outfile;
		std::ofstream ofs("output.txt", std::ios::out | std::ios::trunc);

	*/


	//init CURL
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	//if curl is OK
	if (curl) 
	{
		struct curl_slist *chunk = NULL;

		//add custom header
		chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.100 Safari/537.36");  
		chunk = curl_slist_append(chunk, "Accept: text/html");

		//encapsulate header into request
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		//store data into variable
		curl_easy_setopt(curl, CURLOPT_URL, s);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		
		//perform request
		res = curl_easy_perform(curl);

		//end request
		curl_easy_cleanup(curl);
	}

	/*
		outfile.open("output.txt", std::ios_base::app);
		outfile << readBuffer;
	*/

	//store data into variable
	const char *cstr = readBuffer.c_str();	
	int ii = strlen(cstr);					
	strcpy_s(returnval, cstr);					
	return  returnval;
}
