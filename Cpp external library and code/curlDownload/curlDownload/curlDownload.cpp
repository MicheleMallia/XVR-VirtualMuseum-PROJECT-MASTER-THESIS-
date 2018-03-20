// curlDownload.cpp: definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <curl.h>
#include <fstream>
#include "curlDownload.h"
#include <sys/stat.h>
#include <ctime>

std::clock_t begin = clock();


//to write data into hard disk
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}


//check if file is empty
bool is_empty(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}


//main function
int curlDownload(char* url, char* path) {


		//CURL object
		CURL *curl;

		//variable to store data
		std::string readBuffer;

		//initialize SSL request
		curl_global_init(CURL_GLOBAL_ALL);
		curl = curl_easy_init();

		//if curl is not ok
		if (!curl) {
			return 1;
		}

		//open file stream
		FILE *fp;
		CURLcode res;

		//add custom header
		struct curl_slist *chunk = NULL;
		chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.100 Safari/537.36");
		chunk = curl_slist_append(chunk, "Accept: text/html, image/jpeg");
		
		//encapsulate the header
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		//open the file stream
		fp = fopen(path, "wb");

		//set custom parameter for the get request
		curl_easy_setopt(curl, CURLOPT_URL, url);

		//follow redirect
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

		//set timeout
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);


		//perform request
		res = curl_easy_perform(curl);
		
		//check dimension of file
		int size = ftell(fp);
		
		//end request
		curl_easy_cleanup(curl);

		//close filestream
		fclose(fp);

		std::clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;		
		std::ifstream file(path);

		//check if is a valid file
		if (!is_empty(file) && (size >10000) && (size<1000000))
		{	
			//OK
			return 0;
		}
		else {
			//NOT OK
			return 1;
		}
}
