#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "communication.h"
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
  return status;
}

int main(void){
  if(!main_env_diag()){
    return 1;
  }
  curl_global_init(CURL_GLOBAL_ALL);
  const char *discordwebhookurl=getenv("WEBHOOKURL");
  const char *jsondata=getenv("WEBHOOKDATA");

  discordsetupcommunications(discordwebhookurl);

  discordsendrawmessage(jsondata);

  discordcleanupcommunications();
  curl_global_cleanup();
  return 0;
}
