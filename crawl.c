#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <pcre.h>

#include "storage.h"
#include "mycurl.h"
#include "webstr.h"
#include "crawl_status.h"

/*common*/

void auto_crwl___common_setup_curl(CURL* curl)
{
	assert(curl);

	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "./auto.cookie");
	curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "./auto.cookie");
	curl_easy_setopt(curl, CURLOPT_ENCODING , "gzip,deflate");
}

/*torrents*/

void auto_crwl___torrents_parse(char * string)
{
	const int MATCH_VECTOR_SIZE = 100;

	auto_crst_t * status = auto_crst_get_global();
	assert(status);

	char * buffer = malloc(strlen(string) * sizeof(char) + 1);
	auto_wbs_replace(buffer, string, "<wbr>", " ");
	auto_wbs_replace(string, buffer, "&nbsp;", " ");
	free(buffer);

	const char * error;
	int match_vector[MATCH_VECTOR_SIZE * 3];
	int count, error_offset;

	char * pattern = "<tr[^>]+>\\s*<td[^>]+>\\s*<img[^>]+>\\s*</td>"
		"\\s*<td[^>]+>\\s*(<span[^>]+>)?[^<]+(</span>)?\\s*</td>"
		"\\s*<td[^>]+>\\s*<a[^>]+>[^<]+</a></td>\\s*"
		"<td[^>]+>\\s*<div>\\s*<a\\s*class=\"[^\"]+\"\\s*href=\"(?P<url>[^\"]+)\">\\s*(<b>)?(?P<name>[^<]+)(</b>)?\\s*</a>\\s*</div>\\s*</td>"
		"\\s*<td[^>]+>\\s*<a[^>]+>[^<]+</a>\\s*</td>"
		"\\s*<td[^>]+>\\s*<u>\\s*(?P<size>\\d+)\\s*</u>\\s*<a\\s*class=\"[^\"]+\"\\s*href=\"(?P<download_url>[^\"]+)\">[^<]+</a>"
		"\\s*</td>\\s*<td[^>]+>\\s*(<b>)?\\s*(?P<seed>\\d+)\\s*(</b>)?\\s*</td>"
		"\\s*<td[^>]+>\\s*(<b>)?\\s*(?P<leech>\\d+)\\s*(</b>)?\\s*</td>"
		"\\s*<td[^>]+>\\s*(?P<download_count>\\d+)\\s*</td>"
		"\\s*<td[^>]+>\\s*<u>\\s*(?P<timestamp>\\d+)\\s*</u>\\s*";

	pcre * pcre_handle = pcre_compile(pattern, PCRE_CASELESS,
		& error, & error_offset, 0);

	assert(pcre_handle);


	int offset = 0;
	while(1)
	{
		count = pcre_exec(pcre_handle, NULL, string, strlen(string), 
			offset, 0, match_vector, MATCH_VECTOR_SIZE * 3);

		if(count < 0)
			break;

		/*assert(count == MATCH_VECTOR_SIZE);*/
		
		const char * url, * name, * size, * download_url, * seed, * leech,
			* download_count, * timestamp;

		pcre_get_named_substring(pcre_handle, string, match_vector, count, "url", & url);
		pcre_get_named_substring(pcre_handle, string, match_vector, count, "name", & name);
		pcre_get_named_substring(pcre_handle, string, match_vector, count, "size", & size);
		pcre_get_named_substring(pcre_handle, string, match_vector, count, "download_url", & download_url);
		pcre_get_named_substring(pcre_handle, string, match_vector, count, "seed", & seed);
		pcre_get_named_substring(pcre_handle, string, match_vector, count, "leech", & leech);
		pcre_get_named_substring(pcre_handle, string, match_vector, count, "download_count", & download_count);
		pcre_get_named_substring(pcre_handle, string, match_vector, count, "timestamp", & timestamp);

		assert(status->storage);
		/*auto_strg_save_torrent_categ(auto_crwl___storage, 
			atoi(url), name);*/


		int current_timestamp = atoi(timestamp);
		if(current_timestamp > status->last_crawled)
		{
			/*update most recent*/
			status->most_recent = (current_timestamp > status->most_recent) 
					? current_timestamp : status->most_recent;

			/*save*/
			const size_t name_utf8_len = 4096;
			char name_utf8[name_utf8_len];
			auto_wbs_cp1251_to_utf8(name_utf8, name, name_utf8_len);

			auto_strg_save_torrent(status->storage, status->categ, url,
				name_utf8, atoll(size), download_url, 
				atoi(seed), atoi(leech), atoi(download_count), 
				atoi(timestamp));

			/*fprintf(stderr, "[PARSE] name:%s\n", buffer);
			fprintf(stderr, "[PARSE] url:%s\n", url);
			fprintf(stderr, "[PARSE] size:%i\n", atoi(size));
			fprintf(stderr, "[PARSE] timestamp:%i\n", atoi(timestamp));*/
		}

		pcre_free_substring(url);
		pcre_free_substring(name);
		pcre_free_substring(size);
		pcre_free_substring(download_url);
		pcre_free_substring(seed);
		pcre_free_substring(leech);
		pcre_free_substring(download_count);
		pcre_free_substring(timestamp);

		offset = match_vector[2 * 0 /*entire match*/ + 1];

		if(current_timestamp <= status->last_crawled)
			break;
	}
	pcre_free(pcre_handle);
}

void auto_crwl___torrents_setup_curl(CURL * curl)
{
	assert(curl);

	auto_crst_t * status = auto_crst_get_global();
	assert(status);

	auto_crwl___common_setup_curl(curl);

	curl_easy_setopt(curl, CURLOPT_URL, 
		"http://torrents.ru/forum/tracker.php");

	int i = auto_wbs_urlencode(status->post_data, "f[]");
	i += sprintf(status->post_data + i, 
		"=%i&o=%i&s=%i&submit=True", 
		status->categ /*f[]:category*/,
		1/*o:registered*/, 
		2/*s:desc*/
		);

	/*nm:name*/
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, status->post_data);
}


/*torrent categs*/

void auto_crwl___torrent_categs_parse(char * string)
{
	const int MATCH_VECTOR_SIZE = 100;

	auto_crst_t * status = auto_crst_get_global();
	assert(status);

	char * buffer = malloc(strlen(string) * sizeof(char) + 1);
	auto_wbs_replace(buffer, string, "<wbr>", " ");
	auto_wbs_replace(string, buffer, "&nbsp;", " ");
	free(buffer);

	const char * error;
	int match_vector[MATCH_VECTOR_SIZE * 3];
	int count, error_offset;

	char * pattern = "<option\\s+id=\"fs-\\d+\"\\s+value=\"(?P<value>\\d+)\"[^>]*>\\s*([|-]+)?(?P<name>[^<]+)</option>";

	pcre * pcre_handle = pcre_compile(pattern, PCRE_CASELESS,
		& error, & error_offset, 0);

	assert(pcre_handle);

	int offset = 0;
	while(1)
	{
		count = pcre_exec(pcre_handle, NULL, string, strlen(string), 
			offset, 0, match_vector, MATCH_VECTOR_SIZE * 3);

		if(count < 0)
			break;

		const char * value, * name;

		pcre_get_named_substring(pcre_handle, string, match_vector, count, "value", & value);
		pcre_get_named_substring(pcre_handle, string, match_vector, count, "name", & name);

		assert(status->storage);

		const int name_utf8_len = 4096;
		char name_utf8[name_utf8_len];
		auto_wbs_cp1251_to_utf8(name_utf8, name, name_utf8_len);
		auto_strg_save_torrent_categ(status->storage, atoi(value), name_utf8);

		pcre_free_substring(value);
		pcre_free_substring(name);

		offset = match_vector[2 * 0 /*entire match*/ + 1];
	}

	pcre_free(pcre_handle);
}

void auto_crwl___torrent_categs_setup_curl(CURL * curl)
{
	assert(curl);

	auto_crwl___common_setup_curl(curl);
	curl_easy_setopt(curl, CURLOPT_URL, 
		"http://torrents.ru/forum/tracker.php");
}

void auto_crwl_login()
{
	CURL * curl = curl_easy_init();
	assert(curl);

	auto_crwl___common_setup_curl(curl);

	curl_easy_setopt(curl, CURLOPT_URL, 
		"http://torrents.ru/forum/login.php");

	const char * login = getenv("AUTO_AUTH_LOGIN");
	const char * password = getenv("AUTO_AUTH_PASSWORD");

	assert(login && password);

	char post_data[4096];
	sprintf(post_data, "login_username=%s&login_password=%s&login=True", 
		login, password);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
	/*CURLcode result = */curl_easy_perform(curl);
	curl_easy_cleanup(curl);
}

void auto_crwl_torrent_categs()
{
	auto_crst_t * status = auto_crst_init();
	assert(status);
	auto_crst_set_global(status);

	status->storage = auto_strg_init();

	auto_strg_begin_transaction(status->storage);
	auto_strg_clear_torrent_categs(status->storage);

	auto_mycrl_perform(auto_crwl___torrent_categs_setup_curl,
		auto_crwl___torrent_categs_parse);

	auto_strg_commit_transaction(status->storage);
	auto_strg_free(status->storage);

	auto_crst_set_global(0);
	auto_crst_free(status);
}

void auto_crwl_torrents()
{
	auto_crst_t * status = auto_crst_init();
	assert(status);
	auto_crst_set_global(status);

	status->storage = auto_strg_init();

	const int CATEG_VECTOR_SIZE = 4096;
	int categ_vector[CATEG_VECTOR_SIZE];
	int count = auto_strg_get_torrent_categs_to_crawl(status->storage, 
		categ_vector, CATEG_VECTOR_SIZE, 0);

	assert(count > 0);

	for(int i = 0; i < count; ++i)
	{
		auto_strg_begin_transaction(status->storage);

		status->categ = * (categ_vector + i );
		status->last_crawled 
			= auto_strg_get_torrent_last_crawled(status->storage, 
			status->categ);
		status->most_recent = 0;

		auto_mycrl_perform(auto_crwl___torrents_setup_curl,
			auto_crwl___torrents_parse);

		if(status->most_recent > status->last_crawled)
		{
			auto_strg_set_torrent_last_crawled(status->storage,
				status->categ, status->most_recent);
		}

		auto_strg_commit_transaction(status->storage);
	}

	auto_strg_free(status->storage);

	auto_crst_set_global(0);
	auto_crst_free(status);
}
