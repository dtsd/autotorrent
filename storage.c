#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "storage.h"

void auto_strg___setup_tables(auto_strg_t * this)
{
	/*torrent categories*/
	sqlite3_prepare_v2(this->sqlite_handle, 
		"CREATE TABLE IF NOT EXISTS"
		" auto_torrent_categ(id INTEGER PRIMARY KEY, name VARCHAR(255))", 
		-1, & this->sqlite_stmt, 0);
	int result = sqlite3_step(this->sqlite_stmt);
	sqlite3_finalize(this->sqlite_stmt);
	assert(result == SQLITE_DONE);

	sqlite3_prepare_v2(this->sqlite_handle, 
		"CREATE TABLE IF NOT EXISTS"
		" auto_torrent_categ_to_crawl(id INTEGER PRIMARY KEY)", 
		-1, & this->sqlite_stmt, 0);
	result = sqlite3_step(this->sqlite_stmt);
	sqlite3_finalize(this->sqlite_stmt);
	assert(result == SQLITE_DONE);

	sqlite3_prepare_v2(this->sqlite_handle, 
		"CREATE TABLE IF NOT EXISTS"
		" auto_torrent_last_crawled(id INTEGER PRIMARY KEY, timestamp INTEGER)", 
		-1, & this->sqlite_stmt, 0);
	result = sqlite3_step(this->sqlite_stmt);
	sqlite3_finalize(this->sqlite_stmt);
	assert(result == SQLITE_DONE);

	sqlite3_prepare_v2(this->sqlite_handle, 
		"CREATE TABLE IF NOT EXISTS"
		" auto_torrent(id INTEGER PRIMARY KEY AUTOINCREMENT," 
		" categ INTEGER, url VARCHAR(255), name VARCHAR(255),"
		" size BIGINT, download_url VARCHAR(255),"
		" seed INTEGER, leech INTEGER, download_count INTEGER,"
		" timestamp INTEGER)",
		-1, & this->sqlite_stmt, 0);
	result = sqlite3_step(this->sqlite_stmt);
	sqlite3_finalize(this->sqlite_stmt);
	assert(result == SQLITE_DONE);
}

auto_strg_t * auto_strg_init()
{
	auto_strg_t * this = malloc(sizeof(auto_strg_t));

	const char * dbName = getenv("AUTO_DB_FILE");

	assert(dbName);

	int result = sqlite3_open(dbName, & this->sqlite_handle);

	assert(result == SQLITE_OK);
	
	auto_strg___setup_tables(this);

	return this;
}

void auto_strg_free(auto_strg_t * this)
{
	sqlite3_close(this->sqlite_handle);
	free(this);
}


void auto_strg_clear_torrent_categs(auto_strg_t * this)
{
	char * sql = "DELETE FROM auto_torrent_categ";

	sqlite3_prepare_v2(this->sqlite_handle, sql, -1, 
		& this->sqlite_stmt, 0);
	int result = sqlite3_step(this->sqlite_stmt);
	assert(result == SQLITE_DONE);
	sqlite3_finalize(this->sqlite_stmt);
}

void auto_strg_save_torrent_categ(auto_strg_t * this, 
	int id, const char * name)
{
	char sql[4096];
	sprintf(sql, 
		"INSERT INTO auto_torrent_categ(id, name)"
			" VALUES(%i, '%s')", id, name);

	sqlite3_prepare_v2(this->sqlite_handle, sql, -1, 
		& this->sqlite_stmt, 0);
	int result = sqlite3_step(this->sqlite_stmt);
	sqlite3_finalize(this->sqlite_stmt);
	assert(result == SQLITE_DONE);
}

void auto_strg_begin_transaction(auto_strg_t * this)
{
	char * sql = "BEGIN";

	sqlite3_prepare_v2(this->sqlite_handle, sql, -1, 
		& this->sqlite_stmt, 0);
	int result = sqlite3_step(this->sqlite_stmt);
	sqlite3_finalize(this->sqlite_stmt);
	assert(result == SQLITE_DONE);
}

void auto_strg_commit_transaction(auto_strg_t * this)
{
	char * sql = "COMMIT";

	sqlite3_prepare_v2(this->sqlite_handle, sql, -1, 
		& this->sqlite_stmt, 0);
	int result = sqlite3_step(this->sqlite_stmt);
	sqlite3_finalize(this->sqlite_stmt);
	assert(result == SQLITE_DONE);
}

void auto_strg_rollback_transaction(auto_strg_t * this)
{
	char * sql = "ROLLBACK";

	sqlite3_prepare_v2(this->sqlite_handle, sql, -1, 
		& this->sqlite_stmt, 0);
	int result = sqlite3_step(this->sqlite_stmt);
	sqlite3_finalize(this->sqlite_stmt);
	assert(result == SQLITE_DONE);
}

size_t auto_strg_get_torrent_categs_to_crawl(auto_strg_t * this,
	int * vector, size_t vector_size, int start)
{
	assert(vector);
	assert(start < vector_size);

	char * sql = "SELECT id FROM auto_torrent_categ_to_crawl ORDER BY id";

	sqlite3_prepare_v2(this->sqlite_handle, sql, -1, 
		& this->sqlite_stmt, 0);

	int result = 0;
	int offset = 0;
	while(offset < vector_size)
	{
		result = sqlite3_step(this->sqlite_stmt);
		
		if(result != SQLITE_ROW)
			break;
		
		* (vector + offset) = sqlite3_column_int(this->sqlite_stmt, 0);

		++offset;
	}
	sqlite3_finalize(this->sqlite_stmt);

	if(offset < vector_size)
	{
		assert(result == SQLITE_DONE);
		return offset;
	}
	else
		return 0;
}

int auto_strg_get_torrent_last_crawled(auto_strg_t * this, int categ)
{
	char sql[4096];
	sprintf(sql, 
		"SELECT timestamp FROM auto_torrent_last_crawled WHERE id = %i", categ);

	sqlite3_prepare_v2(this->sqlite_handle, sql, -1, 
		& this->sqlite_stmt, 0);

	int last_crawled = 0;
	if(sqlite3_step(this->sqlite_stmt) == SQLITE_ROW)
	{
		last_crawled = sqlite3_column_int(this->sqlite_stmt, 0);
	}
	sqlite3_finalize(this->sqlite_stmt);
	return last_crawled;
}

void auto_strg_set_torrent_last_crawled(auto_strg_t * this, int categ, int timestamp)
{
	char sql[4096];
	sprintf(sql, 
		"INSERT INTO auto_torrent_last_crawled(id, timestamp)"
		" VALUES(%i, %i)", categ, timestamp);
	sqlite3_prepare_v2(this->sqlite_handle, sql, -1, 
		& this->sqlite_stmt, 0);
	int result = sqlite3_step(this->sqlite_stmt);
	sqlite3_finalize(this->sqlite_stmt);
	if(result == SQLITE_DONE)
		return;

	sprintf(sql, 
		"UPDATE auto_torrent_last_crawled SET timestamp = %i"
		" WHERE id = %i", timestamp, categ);
	sqlite3_prepare_v2(this->sqlite_handle, sql, -1, 
		& this->sqlite_stmt, 0);
	result = sqlite3_step(this->sqlite_stmt);
	sqlite3_finalize(this->sqlite_stmt);
	assert(result == SQLITE_DONE);
}

void auto_strg_save_torrent(auto_strg_t * this, int categ, const char * url,
	const char * name, long long size, const char * download_url, int seed,
	int leech, int download_count, int timestamp)
{
	char sql[4096];
	sprintf(sql, 
		"INSERT INTO auto_torrent(id, categ, url, name, size, download_url,"
			" seed, leech, download_count, timestamp)"
			" VALUES(NULL, %i, '%s', '%s', %lld, '%s',"
			" %i, %i, %i, %i)", categ, url, name, size, download_url, seed,
		leech, download_count, timestamp);

	sqlite3_prepare_v2(this->sqlite_handle, sql, -1, 
		& this->sqlite_stmt, 0);
	int result = sqlite3_step(this->sqlite_stmt);
	sqlite3_finalize(this->sqlite_stmt);
	assert(result == SQLITE_DONE);
}

