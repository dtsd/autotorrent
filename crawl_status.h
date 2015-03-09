#if !defined(AUTO_CRAWL_STATUS_H)
#define AUTO_CRAWL_STATUS_H

#include "storage.h"

struct auto_crst
{
	auto_strg_t * storage;
	int categ;
	int last_crawled;
	int most_recent;
	char post_data[1024];
};
typedef struct auto_crst auto_crst_t;

auto_crst_t *		auto_crst_init();
void 				auto_crst_free(auto_crst_t *);

void 				auto_crst_set_global(auto_crst_t *);
auto_crst_t * 		auto_crst_get_global();

#endif
