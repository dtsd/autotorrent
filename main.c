#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include <pcre.h>

/*parsing html*/

const int AUTO_PARSE_CATEGS_VECTOR_SIZE = 2;

void feed___parse_string(const char * data)
{
	const char * error;
	int captureList[AUTO_PARSE_CATEGS_VECTOR_SIZE * 3];
	int count, errorOffset;

	char * pattern = "<title>([^<]+)</title>";

	pcre * pcreHandle = pcre_compile(pattern, PCRE_CASELESS,
		& error, & errorOffset, 0);

	assert(pcreHandle);

	int startOffset = 0;
	while(1)
	{
		count = pcre_exec(pcreHandle, NULL, data, strlen(data), 
			startOffset, 0, captureList, AUTO_PARSE_CATEGS_VECTOR_SIZE * 3);

		if(count < 0)
			break;

		assert(count == AUTO_PARSE_CATEGS_VECTOR_SIZE);
		
		const char * title;

		pcre_get_substring(data, captureList, count, 1, & title);

		printf("%s\n", title);

		pcre_free_substring(title);

		startOffset = captureList[2 * (AUTO_PARSE_CATEGS_VECTOR_SIZE - 1) + 1];
	}
}

size_t feed___parse_callback(void * ptr, size_t size, size_t nmemb, void * stream)
{
	size_t totalSize = size * nmemb;
	char * data = malloc(totalSize + 1);
	* (data + totalSize) = '\0';

	memcpy(data, ptr, totalSize);

	feed___parse_string(data);
	
	free(data);

	return totalSize;
}

void feed_parse(const char * url)
{
	CURL * handle;
	CURLcode result;
	handle = curl_easy_init();

	curl_easy_setopt(handle, CURLOPT_URL, url);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, 
		feed___parse_callback);
	result = curl_easy_perform(handle);

	assert(result == 0);

	curl_easy_cleanup(handle);
}

int main(int argc, char ** argv)
{
	assert(argc == 2);

	feed_parse(* (argv + 1));

	return 0;
}
