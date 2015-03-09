#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "crawl.h"

void auto_crwl_usage()
{
}

int main(int argc, char ** argv)
{
	auto_crwl_login();
	/*auto_crwl_torrent_categs();*/
	auto_crwl_torrents();

	return 0;
}

