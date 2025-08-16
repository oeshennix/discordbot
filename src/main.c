#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "gateway.h"
#include "gateway_events.h"
#include <pthread.h>
char* BotUserId;

//static cJSON basejsonmessage;
static void* sendmessage(void *userdata){
  cJSON *cjsonobject;
  cJSON *jsonpayload=((struct gateway_payload*)userdata)->payload;
  struct discordgateway *discordgateway=((struct gateway_payload*)userdata)->discordgateway;
  cjsonobject=cJSON_GetObjectItemCaseSensitive(jsonpayload,"d");
  if(!cjsonobject){
    fputs("inner payload \"d\" is invalid",stderr);
    return NULL;
  }

  //message
  cJSON *message=cJSON_CreateObject();
  cJSON_AddStringToObject(message,"content","test message");
  char* postfields=cJSON_PrintUnformatted(message);
  //header
  struct curl_slist *list;
  char* AuthorizationHeader=malloc(256*sizeof(char));
  sprintf(AuthorizationHeader,"Authorization: Bot %s",discordgateway->Token);
  list= curl_slist_append(list,AuthorizationHeader);
  list= curl_slist_append(list,"Content-Type: application/json");
  //url
  CURL *duplicatedhandle=curl_easy_duphandle(discordgateway->curl);
  char* url= malloc(256*sizeof(char));
  cjsonobject=cJSON_GetObjectItemCaseSensitive(cjsonobject,"channel_id");
  char* channel_id=cJSON_GetStringValue(cjsonobject);
  sprintf(url,"https://discord.com/api/v10/channels/%s/messages",channel_id);

  curl_easy_setopt(duplicatedhandle,CURLOPT_POSTFIELDS,postfields);
  curl_easy_setopt(duplicatedhandle,CURLOPT_HTTPHEADER,list);
  curl_easy_setopt(duplicatedhandle,CURLOPT_URL,url);
  CURLcode res = curl_easy_perform(duplicatedhandle);
  /* Check for errors */
  if(res != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
      curl_easy_strerror(res));
  /* always cleanup */
  curl_easy_cleanup(duplicatedhandle);

  free(AuthorizationHeader);
  free(url);

  cJSON_Delete(jsonpayload);
  free(userdata);
  return NULL;
}

static void dispatch(struct gateway_payload* gateway_payload){
  cJSON *jsonpayload=gateway_payload->payload;
  struct discordgateway *discordgateway=gateway_payload->discordgateway;

  cJSON* cjsonobject;
  cjsonobject=cJSON_GetObjectItemCaseSensitive(jsonpayload,"d");
  if(!cjsonobject) return;
  fputs("d",stdout);
  fflush(stdout);
  cjsonobject=cJSON_GetObjectItemCaseSensitive(cjsonobject,"author");
  if(!cjsonobject) return;
  fputs("author",stdout);
  fflush(stdout);
  cjsonobject=cJSON_GetObjectItemCaseSensitive(cjsonobject,"id");
  if(!cjsonobject) return;
  fputs("id",stdout);
  fflush(stdout);
  char* Author_id=cJSON_GetStringValue(cjsonobject);
  if(strcmp(Author_id,BotUserId)==0)
    return;


  char *val=cJSON_Print(jsonpayload);
  fputs(val,stdout);
  pthread_t messaging_thread;
  pthread_create(&messaging_thread,NULL,sendmessage,(void*)gateway_payload);
  pthread_detach(messaging_thread);
  
  return;
}

void* gatewayhandler(void* args){
  void** gatewayfunctions[WEBHOOKS_UPDATE]={};
  gatewayfunctions[MESSAGE_CREATE]=(void*)dispatch;
  struct discordgateway *gateway=gateway_easy_init(NULL,gatewayfunctions);
  if(gateway==NULL){
    fputs("did not setup gateway_easy_init correctly",stderr);
    return NULL;
  }
  char *discordtoken=getenv("DISCORDTOKEN");
  gateway->Token=discordtoken;
  gateway->Intents=(double)(1<<10 | 1<<9 | 1<<0 );
  int err=gateway_start(gateway, "wss://gateway.discord.gg/?v=10&encoding=json");
}

int main_env_diag(){
  int status=1;
  if(getenv("DISCORDTOKEN")==NULL){
    fputs("DISCORDTOKEN environment variable not defined\n",stderr);
    status=0;
  }
  if(getenv("BOTUSERID")==NULL){
    fputs("BOTUSERID environment variable not defined\n",stderr);
    status=0;
  }
  return status;
}

int main(void){
  if(!main_env_diag()){
    return 1;
  }
  // setup json object to be cloned
  //basejson=cJSON_CreateObject();
  curl_global_init(CURL_GLOBAL_ALL);
  const char *discordwebhookurl=getenv("WEBHOOKURL");
  BotUserId=getenv("BOTUSERID");
  pthread_t gatewaythread;

  pthread_create(&gatewaythread,NULL,gatewayhandler,NULL);

  curl_global_cleanup();
  pthread_join(gatewaythread,NULL);
  return 0;
}
