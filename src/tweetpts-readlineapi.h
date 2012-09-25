#ifndef __TWEETPTS_READLINEAPI__
#define __TWEETPTS_READLINEAPI__

#include <readline/readline.h>
#include <readline/history.h>

gchar* readlineapi_plain(gchar *prompt);
gchar *readlineapi_wgetstr(void);


#endif
