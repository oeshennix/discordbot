#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "gateway.h"
#ifndef NO_GATEWAY_EASY_INIT
#include <cjson/cJSON.h>
#endif

struct discordgateway newgateway(){
  struct discordgateway newgateway;
  newgateway.curl = curl_easy_init();
  return newgateway;
}
int gateway_start(struct discordgateway *gateway,char* url){
  CURLcode res;
  curl_easy_opt(gateway->curl,CURLOPT_URL,url);
  curl_easy_opt(gateway->curl,CURLOPT_CONNECT_ONLY,2L);
  res = curl_easy_perform(curl);
  if(res != CURLE_OK){
    fputs("failed to connect to gateway",stderr);
    return 0;
  }
  
  return 1;
}
#ifndef NO_GATEWAY_EASY_INIT
static void gateway_easy_handle_establishing(struct discordgateway *gateway){
  char buffer[256];
  size_t offset = 0;
  CURLcode res = CURLE_OK;
  size_t recv;
  const struct curl_ws_frame *meta;
  res = curl_ws_recv(gateway->curl,buffer+offset,sizeof(buffer)-offset,&reccv,&meta);
}

struct discordgateway gateway_easy_init(void (*gatewayfunction)()){
  struct discordgateway newgateway;
  newgateway.curl = curl_easy_init();
  return newgateway;
}
#endif
