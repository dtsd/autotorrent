#if !defined(AUTO_STORAGE_H)
#define AUTO_STORAGE_H

#include <sqlite3.h>

struct auto_strg
{
	sqlite3 * sqlite_handle;
	sqlite3_stmt * sqlite_stmt;
};
typedef struct auto_strg auto_strg_t;

auto_strg_t * 		auto_strg_init();
void 				auto_strg_free(auto_strg_t *);

void				auto_strg_begin_transaction(auto_strg_t *);
void				auto_strg_commit_transaction(auto_strg_t *);
void				auto_strg_rollback_transaction(auto_strg_t *);

/*torrent categs*/
void				auto_strg_clear_torrent_categs(auto_strg_t *);
void				auto_strg_save_torrent_categ(auto_strg_t *, 
						int id, const char * name);

/*torrents*/
size_t				auto_strg_get_torrent_categs_to_crawl(auto_strg_t *,
						int * vector, size_t vector_size, int start);

int					auto_strg_get_torrent_last_crawled(auto_strg_t *, 
						int categ);
void				auto_strg_set_torrent_last_crawled(auto_strg_t *, 
						int categ, int timestamp);

void				auto_strg_save_torrent(auto_strg_t *, int categ,
						const char * url, const char * name, long long size,
						const char * download_url, int seed, int leech, 
						int download_count, int timestamp);


#endif /*AUTO_STORAGE_H*/
