#include <curl/curl.h>

struct discordgateway{
  curl *curl;
  void (*gatewayfunction)();
  void *extradata;
}

struct discordgateway newdiscordgateway();
int gateway_start(struct discordgateway gateway);
int gateway_cleanup(struct discordgateway gateway);
#ifndef NO_GATEWAY_EASY_INIT

void gateway_easy_init(void (*gatewayfunction)());
#endif
