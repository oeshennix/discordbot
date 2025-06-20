#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "communication.h"
#include "gateway.h"
#include <pthread.h>

int main_env_diag(){
  int status=1;
  if(getenv("WEBHOOKDATA")==NULL){
    fputs("WEBHOOKDATA environment variable not defined\n",stderr);
    status=0;
  }
  if(getenv("WEBHOOKURL")==NULL){
    fputs("WEBHOOKURL environment variable not defined\n",stderr);
    status=0;
  }
  if(getenv("DISCORDTOKEN")==NULL){
    fputs("DISCORDTOKEN environment variable not defined\n",stderr);
    status=0;
  }
  return status;
}

void a(){
}
void* gatewayhandler(void* args){
  struct discordgateway gateway=gateway_easy_init(NULL);
  char *discordtoken=getenv("DISCORDTOKEN");
  gateway.Token=discordtoken;
  gateway.Intents=(double)(1<<10 | 1<<9 | 1<<0 );
  int err=gateway_start(&gateway, "wss://gateway.discord.gg/?v=10&encoding=json");
}

int main(void){
  if(!main_env_diag()){
    return 1;
  }
  curl_global_init(CURL_GLOBAL_ALL);
  const char *discordwebhookurl=getenv("WEBHOOKURL");
  const char *jsondata=getenv("WEBHOOKDATA");
  pthread_t gatewaythread;

  pthread_create(&gatewaythread,NULL,gatewayhandler,NULL);

  discordsetupcommunications(discordwebhookurl);

  discordcleanupcommunications();
  curl_global_cleanup();
  pthread_join(gatewaythread,NULL);
  return 0;
}
