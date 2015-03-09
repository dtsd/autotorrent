#if !defined(AUTO_WEBSTR_H)
#define AUTO_WEBSTR_H

int 				auto_wbs_urlencode(char * str, const char * src);
void 				auto_wbs_replace(char * str, const char * src, 
						const char * from, const char * to);

int					auto_wbs_cp1251_to_utf8(char * str, const char * src,
						const size_t str_len);

size_t				auto_wbs_strfsize(char * str, long long size);
size_t				auto_wbs_strftime_rfc822(char * str, 
						const size_t str_len, int timestamp);
#endif /*AUTO_WEBSTR_H*/


