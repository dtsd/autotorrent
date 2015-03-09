#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "crawl_status.h"

auto_crst_t * auto_crst___global = 0;

auto_crst_t * auto_crst_init()
{
	auto_crst_t * this = malloc(sizeof(auto_crst_t));
	this->storage = 0;
	this->categ = 0;
	this->last_crawled = 0;
	this->most_recent = 0;
	return this;
}

void auto_crst_free(auto_crst_t * this)
{
	free(this);
}

void auto_crst_set_global(auto_crst_t * this)
{
	auto_crst___global = this;
}

auto_crst_t * 	auto_crst_get_global()
{
	return auto_crst___global;
}
