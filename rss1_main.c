#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "storage.h"
#include "webstr.h"

void auto_rss1_usage()
{
}

int main(int argc, char ** argv)
{
	printf("Content-Type: application/rss+xml; charset=utf-8\n");
	printf("\n");

	const char * query_string = getenv("QUERY_STRING");
	if(!query_string)
		return -1;

	int categ = 0;
	if(sscanf(query_string, "categ=%d", & categ) <= 0)
		return -1;

	const char * this_url = "http://78.107.254.238:58081/cgi/auto_rss1.sh";
	printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	printf("<rss version=\"2.0\" xmlns:atom=\"http://www.w3.org/2005/Atom\">\n");
	printf("<channel>\n");
	printf("<title>categ %d</title>\n", categ);
	printf("<description>All crawled torrents</description>\n");
	printf("<link>%s</link>\n", this_url);
	printf("<atom:link href=\"%s\" rel=\"self\" type=\"application/rss+xml\"/>",
		this_url);
	printf("<language>en-us</language>\n");

	auto_strg_t * storage = auto_strg_init();

	char sql[4096];
	sprintf(sql, "SELECT t.url, t.name, t.size, t.download_url,"
		" t.timestamp, c.name, t.id"
		" FROM auto_torrent t"
		" LEFT JOIN auto_torrent_categ c ON t.categ = c.id"
		" WHERE t.categ = %i"
		" ORDER BY t.timestamp DESC LIMIT %i", categ, 100);

	sqlite3_prepare_v2(storage->sqlite_handle, sql, -1, 
		& storage->sqlite_stmt, 0);

	int result;
	while(1)
	{
		result = sqlite3_step(storage->sqlite_stmt);
		if(result != SQLITE_ROW)
			break;

		const unsigned char * url 
			= sqlite3_column_text(storage->sqlite_stmt, 0);
		const unsigned char * name 
			= sqlite3_column_text(storage->sqlite_stmt, 1);
		long long size = sqlite3_column_int64(storage->sqlite_stmt, 2);
		/*
		const unsigned char * download_url 
			= sqlite3_column_text(storage->sqlite_stmt, 3);
			*/
		int timestamp = sqlite3_column_int(storage->sqlite_stmt, 4);
		const unsigned char * categ_name 
			= sqlite3_column_text(storage->sqlite_stmt, 5);

		int id = sqlite3_column_int(storage->sqlite_stmt, 6);

		const int timestamp_buffer_size = 256;
		char timestamp_buffer[timestamp_buffer_size];
		auto_wbs_strftime_rfc822(timestamp_buffer, timestamp_buffer_size,
			timestamp);

		char size_buffer[256];
		auto_wbs_strfsize(size_buffer, size);

		printf("<item>\n");
		printf("<title>%s</title>\n", name);
		printf("<link>http://torrents.ru/forum/%s</link>\n", url);
		printf("<description>Category: %s; Published: %s; Size: %s;</description>\n", 
			categ_name, timestamp_buffer, size_buffer);

		printf("<pubDate>%s</pubDate>\n", timestamp_buffer);
		printf("<guid>http://torrents.ru/forum/%s#%i</guid>\n", url, id);
		printf("</item>\n");
	}

	sqlite3_finalize(storage->sqlite_stmt);

	auto_strg_free(storage);

	printf("</channel>\n");
	printf("</rss>\n");

	return 0;
}
