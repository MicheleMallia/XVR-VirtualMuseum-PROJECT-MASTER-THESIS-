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

using namespace rapidjson;

char returnval[1256000];

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}


char* decripta(char* s) {

	CURL *curl;
	CURLcode res;
	std::string readBuffer;
	std::string url;
	std::string artist;
	std::string title;
	std::string date;
	std::string description;
	//std::string url(s);

	curl_global_init(CURL_GLOBAL_ALL);
	std::string strings;
	const char* pageId ="";
	curl = curl_easy_init();
	if (curl)
	{
		struct curl_slist *chunk = NULL;

		chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.100 Safari/537.36");

		/* Add a custom header */
		chunk = curl_slist_append(chunk, "Accept: application/json");

		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		curl_easy_setopt(curl, CURLOPT_URL, s);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);


		const char* json = readBuffer.c_str();
		Document document;
		document.Parse(json);
		const Value& a = document["object"]["aggregations"][0];

		if (a.HasMember("edmIsShownBy")) {
			url = a["edmIsShownBy"].GetString();
		}
		else {
			url = "";
		}
		

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
				}
				else {
					artist = d[pageId][0].GetString();
					break;
				}
			}
	
		}
		else {
			artist = "No avaiable artist name";
		}

		if (b.HasMember("dcTitle")) {
			const Value& d = b["dcTitle"];
			for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
				const char* nameObject;
				nameObject = itr->name.GetString();
				pageId = nameObject;
				int size = d[pageId].Size();
				if (size == 0) {
					title = "No avaiable title";
				}
				else {
					title = d[pageId][0].GetString();
					break;
				}
			}
		}
		else {
			title = "No avaiable title";
		}

		if (b.HasMember("dctermsCreated")) {
			const Value& d = b["dctermsCreated"];
			for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
				const char* nameObject;
				nameObject = itr->name.GetString();
				pageId = nameObject;
				break;
			}
			date = d[pageId][0].GetString();
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
		}
		else {
			date = "No avaiable date";
		}

		if (b.HasMember("dcDescription")) {
			const Value& d = b["dcDescription"];
			for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
				const char* nameObject;
				nameObject = itr->name.GetString();
				pageId = nameObject;
				int size = d[pageId].Size();
				if (size == 0) {
					description = "No avaiable title";
				}
				else {
					description = d[pageId][0].GetString();
					break;
				}
			}

		}
		else {
			description = "No avaiable description";
		}


		strings = url + "$" + artist + "$" + title + "$" + date + "$" + description;

		curl_easy_cleanup(curl);
	}

	const char *cstr = strings.c_str();
	int ii = strlen(cstr);
	strcpy_s(returnval, cstr);
	return returnval;
}


