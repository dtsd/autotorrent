#if !defined(AUTO_MYCURL_H)
#define AUTO_MYCURL_H

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

typedef void auto_mycrl_setup_call_t(CURL *);
typedef void auto_mycrl_parse_call_t(char *);

void auto_mycrl_perform(auto_mycrl_setup_call_t *, auto_mycrl_parse_call_t *);

#endif /*AUTO_MYCURL_H*/
