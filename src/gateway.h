#include <curl/curl.h>

#ifndef GATEWAY_ENCODING
#define GATEWAY_ENCODING 1
#endif


struct discordgateway{
  CURL *curl;
  void (*gatewayfunction)();
  char* Token;
  double Intents;
  void *userdata;
};

struct discordgateway newdiscordgateway();
int gateway_start(struct discordgateway *gateway,char *url);
int gateway_cleanup(struct discordgateway *gateway);
#ifndef NO_GATEWAY_EASY_INIT

struct discordgateway gateway_easy_init(void (*gatewayfunction)());
#endif
