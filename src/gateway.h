#include <curl/curl.h>

#ifndef GATEWAY_ENCODING
#define GATEWAY_ENCODING 1
#endif


struct discordgateway{
  CURL *curl;
  void (*gatewayfunction)();
  char* Token;
  double Intents;
  //long UserId;
  void *userdata;
  double sequence;
};

struct discordgateway newdiscordgateway();
int gateway_start(struct discordgateway *gateway,char *url);
int gateway_cleanup(struct discordgateway *gateway);
#ifndef NO_GATEWAY_EASY_INIT
#if GATEWAY_ENCODING 
#include <cjson/cJSON.h>
#endif
struct gateway_payload{
  struct discordgateway *discordgateway;
#if GATEWAY_ENCODING 
  cJSON *payload;
#endif
};

struct discordgateway* gateway_easy_init(void (*dispatchfunction)(),void *readyfunction);
void gateway_easy_cleanup(struct discordgateway *discordgateway);
#endif
