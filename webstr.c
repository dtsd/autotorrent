#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <iconv.h>

#include "webstr.h"

int auto_wbs_urlencode(char * str, const char * src)
{
	assert(str);

	const char * HEX_DATA = "0123456789ABCDEF";

	int offset = 0;
	while(1)
	{
		char c = * src;
		if(!c)
		{
			break;
		}
		else if(('A' <= c && c <= 'Z') 
			|| ('a' <= c && c <= 'z')
			|| ('0' <= c && c <= '9'))
		{
			* (str + offset) = c;
			++offset;
		}
		else
		{
			* (str + offset) = '%';
			++offset;
			* (str + offset) = HEX_DATA[c >> 4];
			++offset;
			* (str + offset) = HEX_DATA[c & 15];
			++offset;
		}
		++src;
	}

	* (str + offset) = '\0';
	return offset;
}

void auto_wbs_replace(char * str, const char * src, const char * from,
	const char * to)
{
	assert(src);

	* str = '\0';
	while(1)
	{
		char * pc = strstr(src, from);
		if(!pc)
			break;
		int offset = pc - src;
		strncat(str, src, offset);
		strcat(str, to);
		src += offset + strlen(from);
	}
	strcat(str, src);
}

int auto_wbs_cp1251_to_utf8(char * str, const char * src, const size_t str_len)
{
	iconv_t * iconv_handle = iconv_open("UTF-8", "CP1251");

	const char ** src_ptr = & src;
	size_t src_left = strlen(src);

	size_t str_left = str_len;

	size_t err;
	while(src_left)
	{
		err = iconv(iconv_handle, (char **) src_ptr, & src_left,
			& str, & str_left);
		if(err == (size_t)-1)
			break;
	}
	* str = '\0';
	iconv_close(iconv_handle);
	return str_len - str_left;
}

size_t auto_wbs_strfsize(char * str, long long size)
{
	const char * SIZE_DATA = "KMGTPEZY"; 
	
	size_t i = 0;
	while(size >= 1024 && i <= strlen(SIZE_DATA))
	{
		size = size >> 10;
		++i;
	}

	if(i)
		return sprintf(str, "%lld%c", size, * (SIZE_DATA + i - 1));
	else
		return sprintf(str, "%lld", size);
}

size_t auto_wbs_strftime_rfc822(char * str, const size_t str_len, 
	int timestamp)
{
	return strftime(str, str_len,
		"%a, %d %b %Y %H:%M:%S GMT", 
		gmtime((const time_t *) & timestamp));
}
