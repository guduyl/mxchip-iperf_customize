#include "mxos.h"
#include "url.h"

#define url_log(format, ...)  custom_log("URL", format, ##__VA_ARGS__)

int main( void )
{
  /* Output on debug serial port */
    url_log( "Url Option!" );

    url_field_t *url = url_parse("schema://usr:pwd@localhost:port/path?a=b&c=d&e=f#foo");

    url_field_print(url);
    url_free(url);

    return 0;
}


