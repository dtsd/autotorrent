#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "mycurl.h"

auto_mycrl_parse_call_t * auto_mycrl___parse_call = 0;

size_t auto_mycrl___write_callback(void * ptr, 
	size_t size, size_t nmemb, void * stream)
{
	size_t total_size = size * nmemb;
	char * string = malloc(total_size + 1);
	* (string + total_size) = '\0';
	memcpy(string, ptr, total_size);
	(* auto_mycrl___parse_call)(string);
	free(string);
	return total_size;
}

void auto_mycrl_perform(auto_mycrl_setup_call_t * setup_call, 
	auto_mycrl_parse_call_t * parse_call)
{
	CURL * curl = curl_easy_init();
	assert(curl);

	(* setup_call)(curl);
	auto_mycrl___parse_call = parse_call;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 
		auto_mycrl___write_callback);

	curl_easy_perform(curl);

	curl_easy_cleanup(curl);
}

