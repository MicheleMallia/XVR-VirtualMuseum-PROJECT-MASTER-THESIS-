// curlDll.cpp: definisce il punto di ingresso dell'applicazione console.
//
#include "stdafx.h"
#include <iostream>
#include <string>
#include <curl.h>
#include <fstream>
#include "curlWikimedia.h"
#include <vector>
#include <string>
#include <sstream>
#include <rapidjson.h>
#include <document.h>
using namespace rapidjson;

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


//write data into variable
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

	//store data in this variable
	std::string readBuffer;

	//init SSL
	curl_global_init(CURL_GLOBAL_ALL);
	
	//store all data in this variable
	std::string strings;

	//init CURL
	curl = curl_easy_init();

	//if curl OK
	if (curl)
	{	

		//add custom header
		struct curl_slist *chunk = NULL;
		chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.100 Safari/537.36");
		chunk = curl_slist_append(chunk, "Accept: text/html");

		//encapsulate header
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		//pass url to CURL
		curl_easy_setopt(curl, CURLOPT_URL, s);

		//store data into variable
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

		//perform request
		res = curl_easy_perform(curl);
		

		//create json object
		const char* json = readBuffer.c_str();
		Document document;
		document.Parse(json);

		//access to list of file
		const Value& a = document["query"]["categorymembers"];

		//get the filestring
		for (SizeType i = 0; i < a.Size(); i++) {
			const char* fileName = a[i]["title"].GetString();
			char *output = curl_easy_escape(curl, fileName, 0);
			strings = strings + output + ", ";
		}

		//end of request
		curl_easy_cleanup(curl);
	}
	
	//convert string to char
	const char *cstr = strings.c_str();
	int ii = strlen(cstr);
	strcpy_s(returnval, cstr);
	return  returnval;
}
