// curlDll.cpp: definisce il punto di ingresso dell'applicazione console.
//
#include "stdafx.h"
#include <iostream>
#include <string>
#include <curl.h>
#include <fstream>
#include "curlGetWikimediaUrl.h"
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <rapidjson.h>
#include <document.h>
#include <codecvt>
#include <comdef.h>  // you will need this
#include <clocale>
#include <locale>
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

char returnval[94256000];

//function to clean the metadata in Wikimedia
std::string cleanMetadata(std::string input) {
	std::string strings = input;
	std::regex reg("(<[^>]*>)");
	std::smatch m;
	if (std::regex_search(strings, m, reg)) {
		strings = std::regex_replace(strings, reg, "");
	}
	else {
		strings = strings;
	}
	return strings;
}


//function to write data into variable
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}


//main function
char* decripta(char* s) {


	//set encoding rules
	std::setlocale(LC_ALL, "");

	//object CURL
	CURL *curl;
	CURLcode res;

	//variable to store data
	std::string readBuffer;


	//init SSL curl mode
	curl_global_init(CURL_GLOBAL_ALL);

	//variables to store metadata
	std::string strings;
	std::string url;
	std::string artist;
	std::string title;
	std::string date;
	std::string description;

	//wchar to store metadata
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring_convert<std::codecvt_utf8<char>> converterCiao;
	std::wstring wstrings;
	std::wstring wUrl;
	std::wstring wArtist;
	std::wstring wTitle;
	std::wstring wDate;
	std::wstring wDesc;
	std::wstring none = converter.from_bytes("No avaiable");
	std::wstring sep = converter.from_bytes("$");

	//to memorize the random pageId from Wikimedia
	const char* pageId = "";

	//initialize curl
	curl = curl_easy_init();

	//if CURL is OK
	if (curl)
	{	

		//add custom header
		struct curl_slist *chunk = NULL;
		chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.100 Safari/537.36");
		chunk = curl_slist_append(chunk, "Accept: text/html");

		//encapsulate custom header
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		//pass url to curl
		curl_easy_setopt(curl, CURLOPT_URL, s);
		
		//write data into variable
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		
		//perform request
		res = curl_easy_perform(curl);


		//json object
		const char* json = readBuffer.c_str();
		Document document;
		document.Parse(json);

		//access to pages node
		const Value& a = document["query"]["pages"];
		
		//fetch random node
		for (Value::ConstMemberIterator itr = a.MemberBegin(); itr != a.MemberEnd(); ++itr) {
			const char* nameObject;
			nameObject = itr->name.GetString();
			pageId = nameObject;
		}

		//get url of artwork
		const Value& b = a[pageId]["imageinfo"];
		url = b[0]["thumburl"].GetString();
		wUrl = converter.from_bytes(url);
		
		//get artist name
		artist = b[0]["extmetadata"]["Artist"]["value"].GetString();
		artist = cleanMetadata(artist);
		wArtist = converter.from_bytes(artist);
		
		//get artwork title
		title = b[0]["extmetadata"]["ObjectName"]["value"].GetString();
		title = cleanMetadata(title);
		wTitle = converter.from_bytes(title);


		//get artwork date creation
		const Value& c = b[0]["extmetadata"];
		if (c.HasMember("DateTimeOriginal")) {
			date = b[0]["extmetadata"]["DateTimeOriginal"]["value"].GetString();
			wDate = converter.from_bytes(date);
		}
		else {
			date = "No avaiable date";
			wDate = none;
		}


		//get image description
		if (c.HasMember("ImageDescription")) {
			description = c["ImageDescription"]["value"].GetString();
			description = cleanMetadata(description);
			wDesc = converter.from_bytes(description);
		}
		else {
			description = "No avaiable description";
			wDesc = none;
		}
		

		//concanetate metadata and url
		strings = url+"$"+ artist + "$" + title + "$" + date + "$" + description;
		wstrings = wUrl + sep + wArtist + sep + wTitle + sep + wDate + sep + wDesc;


		//end request
		curl_easy_cleanup(curl);
	}

	//convert wchar in char
	const wchar_t *wcstr = wstrings.c_str();
	_bstr_t b(wcstr);
	const char* cstr = b;
	int ii = strlen(cstr);
	strcpy_s(returnval, cstr);
	return returnval;
	
}
