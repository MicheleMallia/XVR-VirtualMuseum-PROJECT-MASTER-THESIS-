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

std::string cleanMetadata(std::string input) {
	std::string strings = input;
	std::regex reg("(<[^>]*>)");
	std::smatch m;
	if (std::regex_search(strings, m, reg)) {
		strings = std::regex_replace(strings, reg, "");
		//std::string line = "\n";
		//size_t pos = strings.find("\n");
		//strings.replace(pos, line.length(), "");
	}
	else {
		strings = strings;
	}
	return strings;
}


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

char* decripta(char* s) {

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl_global_init(CURL_GLOBAL_ALL);
	std::string strings;
	std::string url;
	std::string artist;
	std::string title;
	std::string date;
	std::string description;

	const char* pageId = "";
	curl = curl_easy_init();
	if (curl)
	{
		struct curl_slist *chunk = NULL;

		chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.100 Safari/537.36");

		/* Add a custom header */
		chunk = curl_slist_append(chunk, "Accept: text/html");

		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		curl_easy_setopt(curl, CURLOPT_URL, s);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);

		const char* json = readBuffer.c_str();
		Document document;
		document.Parse(json);
		const Value& a = document["query"]["pages"];
		for (Value::ConstMemberIterator itr = a.MemberBegin(); itr != a.MemberEnd(); ++itr) {
			const char* nameObject;
			nameObject = itr->name.GetString();
			pageId = nameObject;
		}

		const Value& b = a[pageId]["imageinfo"];
		url = b[0]["thumburl"].GetString();
		
		artist = b[0]["extmetadata"]["Artist"]["value"].GetString();
		artist = cleanMetadata(artist);
		title = b[0]["extmetadata"]["ObjectName"]["value"].GetString();
		title = cleanMetadata(title);

		const Value& c = b[0]["extmetadata"];

		if (c.HasMember("DateTimeOriginal")) {
			date = b[0]["extmetadata"]["DateTimeOriginal"]["value"].GetString();
		}
		else {
			date = "No avaiable date";
		}

		if (c.HasMember("ImageDescription")) {
			description = c["ImageDescription"]["value"].GetString();
			description = cleanMetadata(description);
		}
		else {
			description = "No avaiable description";
		}
		
		strings = url+"$"+ artist + "$" + title + "$" + date + "$" + description;

		curl_easy_cleanup(curl);
	}

	const char *cstr = strings.c_str();
	int ii = strlen(cstr);
	strcpy_s(returnval, cstr);
	return  returnval;
}
