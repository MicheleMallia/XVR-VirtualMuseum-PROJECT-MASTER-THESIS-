// ConsoleApplication3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <curl.h>
#include <fstream>
#include <sys/stat.h>
#include <rapidjson.h>
#include <istreamwrapper.h>
#include <document.h>
#include <sstream>
#include "curlEuropeana.h"
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

//function to save the data into a variable
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

	//variable to store the data
	std::string readBuffer;

	//initialization
	curl_global_init(CURL_GLOBAL_ALL);
	std::string strings;

	//curl start
	curl = curl_easy_init();

	//if curl is OK
	if (curl)
	{
		struct curl_slist *chunk = NULL;

		//add custom header
		chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.100 Safari/537.36");
		chunk = curl_slist_append(chunk, "Accept: application/json");

		//encapsule the header into request
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		
		//store json object into variable
		curl_easy_setopt(curl, CURLOPT_URL, s);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		
		//perform request
		res = curl_easy_perform(curl);

		//initialization of json object
		const char* json = readBuffer.c_str();
		Document document;
		document.Parse(json);


		//access to main node
		const Value& items = document["items"];
		assert(items.IsArray());
		for (SizeType i = 0; i < items.Size(); i++) 
		{
			//store any data if element is an image
			if (items[i]["type"] == "IMAGE") {
				std::string url = items[i]["link"].GetString();
				strings = strings + url + "$";
			}

		}

		//stop request
		curl_easy_cleanup(curl);
	}

	//store data into variable
	const char *cstr = strings.c_str();
	int ii = strlen(cstr);
	strcpy_s(returnval, cstr);
	return  returnval;
}

