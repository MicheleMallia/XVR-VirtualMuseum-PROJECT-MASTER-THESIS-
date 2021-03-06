// curlGetEuropeanaUrl.cpp : Defines the entry point for the console application.
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
#include "curlGetEuropeanaUrl.h"
#include <codecvt>
#include <comdef.h>
#include <locale>
#include <clocale>

using namespace rapidjson;

char returnval[94256000];


//store data into a variable
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}


//main function
char* decripta(char* s) {


	//set encoding for wide char
	std::setlocale(LC_ALL, "");

	//CURL obj
	CURL *curl;
	CURLcode res;

	//informantion variable
	std::string readBuffer;
	std::string url;
	std::string artist;
	std::string title;
	std::string date;
	std::string description;
	

	//wchar information variable
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring_convert<std::codecvt_utf8_utf16<char>> converterCiao;
	std::wstring wstrings;
	std::wstring wUrl;
	std::wstring wArtist;
	std::wstring wTitle;
	std::wstring wDate;
	std::wstring wDesc;
	std::wstring none = converter.from_bytes("No avaiable");
	std::wstring sep = converter.from_bytes("$");

	//initialize SSL
	curl_global_init(CURL_GLOBAL_ALL);

	//store data into strings
	std::string strings;

	//Europeana pageID
	const char* pageId ="";

	//initialize CURL
	curl = curl_easy_init();

	//if CURL is OK
	if (curl)
	{

		//add custom header
		struct curl_slist *chunk = NULL;
		chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.100 Safari/537.36");
		chunk = curl_slist_append(chunk, "Accept: application/json");

		//encapsulate header
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		//write data into variable
		curl_easy_setopt(curl, CURLOPT_URL, s);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		
		//perform request
		res = curl_easy_perform(curl);


		//initialize json object
		const char* json = readBuffer.c_str();
		Document document;
		document.Parse(json);

		//access to retrieved json object
		const Value& a = document["object"]["aggregations"][0];

		//take image url
		if (a.HasMember("edmIsShownBy")) {
			url = a["edmIsShownBy"].GetString();
			wUrl = converter.from_bytes(url);
		}
		else {
			url = "";
			wUrl = none;
		}
		

		//take artist name
		const Value& b = document["object"]["proxies"][0];
		if (b.HasMember("dcCreator")) {
			const Value& d = b["dcCreator"];
			for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
				const char* nameObject;
				nameObject = itr->name.GetString();
				pageId = nameObject;
				int size = d[pageId].Size();
				if (size == 0) {
					artist = "No avaiable title";
					wArtist = converter.from_bytes(artist);
				}
				else {
					artist = d[pageId][0].GetString();
					wArtist = converter.from_bytes(artist);
					break;
				}
			}
	
		}
		else {
			artist = "No avaiable artist name";
			wArtist = none;
		}


		//take artwork name
		if (b.HasMember("dcTitle")) {
			const Value& d = b["dcTitle"];
			for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
				const char* nameObject;
				nameObject = itr->name.GetString();
				pageId = nameObject;
				int size = d[pageId].Size();
				if (size == 0) {
					title = "No avaiable title";
					wTitle = none;
				}
				else {
					title = d[pageId][0].GetString();
					wTitle = converter.from_bytes(title);
					break;
				}
			}
		}
		else {
			title = "No avaiable title";
			wTitle = none;
		}


		//take date about artwork
		if (b.HasMember("dctermsCreated")) {
			const Value& d = b["dctermsCreated"];
			for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
				const char* nameObject;
				nameObject = itr->name.GetString();
				pageId = nameObject;
				break;
			}
			date = d[pageId][0].GetString();
			wDate = converter.from_bytes(date);
		}
		else if (b.HasMember("dcDate")) {
			const Value& d = b["dcDate"];
			for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
				const char* nameObject;
				nameObject = itr->name.GetString();
				pageId = nameObject;
				break;
			}
			date = d[pageId][0].GetString();
			wDate = converter.from_bytes(date);
		}
		else {
			date = "No avaiable date";
			wDate = none;
		}


		//take artwork description
		if (b.HasMember("dcDescription")) {
			const Value& d = b["dcDescription"];
			for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
				const char* nameObject;
				nameObject = itr->name.GetString();
				pageId = nameObject;
				int size = d[pageId].Size();
				if (size == 0) {
					description = "No avaiable title";
					wDesc = none;
				}
				else {
					description = d[pageId][0].GetString();
					wDesc = converter.from_bytes(description);
					break;
				}
			}

		}
		else {
			description = "No avaiable description";
			wDesc = none;
		}


		//concatenate all data into a string
		strings = url + "$" + artist + "$" + title + "$" + date + "$" + description;
		wstrings = wUrl + sep + wArtist + sep + wTitle + sep + wDate + sep + wDesc;

		//end CURL
		curl_easy_cleanup(curl);
	}

	//convert wchar into char
	const wchar_t *wcstr = wstrings.c_str();
	_bstr_t b(wcstr);
	const char* cstr = b;
	int ii = strlen(cstr);
	strcpy_s(returnval, cstr);
	return returnval;
}


