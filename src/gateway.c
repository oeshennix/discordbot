#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "gateway.h"
#ifndef NO_GATEWAY_EASY_INIT
#include <pthread.h>
#if GATEWAY_ENCODING 
#include <cjson/cJSON.h>
#elif
#error "ETF encoding is not supported";
#endif
#endif

struct discordgateway newgateway(){
  struct discordgateway newgateway;
  newgateway.curl = curl_easy_init();
  return newgateway;
}
int gateway_start(struct discordgateway *gateway,char *url){
  if(gateway->gatewayfunction==NULL){
    fputs("Gateway.gatewayfunction is a NULL_PTR",stderr);
    return 0;
  }
  CURLcode res;
  curl_easy_setopt(gateway->curl,CURLOPT_URL,url);
  curl_easy_setopt(gateway->curl,CURLOPT_CONNECT_ONLY,2L);
  res = curl_easy_perform(gateway->curl);
  if(res != CURLE_OK){
    fprintf(stderr,"failed to connect to gateway: %s",curl_easy_strerror(res));
    return 0;
  }
  gateway->gatewayfunction(gateway);
  
  return 1;
}
#ifndef NO_GATEWAY_EASY_INIT
static void gateway_easy_handle_payload(void *payloadbuffer,size_t payload_size){
  return;
}

static char *gateway_recieve(struct discordgateway *gateway){
  CURLcode res = CURLE_OK;
  char *buffer=calloc(256,sizeof(char));
  size_t buffer_size=256;
  size_t offset = 0;
  size_t recv;
  const struct curl_ws_frame *meta;
  while(!res){
    res = curl_ws_recv(gateway->curl,buffer+offset,buffer_size-offset,&recv,&meta);
    offset+=recv;
    if(res==CURLE_OK){
      if(meta->bytesleft==0){
        return buffer;
      }else if(meta->bytesleft > sizeof(buffer) - offset){
        puts("NOT ENOUGH BUFFER!!!");
        fflush(stdout);
        buffer_size*=2;
        buffer=realloc(buffer,buffer_size);
      }
    }
    fprintf(stderr,"asdfsfds %d",res);
    if(res==CURLE_AGAIN){
      fputs("CURLE_AGAIN Error I dont want to handle it rn",stderr);
      usleep(100000);
      res=CURLE_OK;
    }
  }
  return NULL;
}
static CURLcode *gateway_send(struct discordgateway *gateway,char* buffer){
  CURLcode res = CURLE_OK;
  size_t offset = 0;
  while(!res){
    size_t sent;
    const struct curl_ws_frame *meta;
    res = curl_ws_send(gateway->curl,buffer+offset,strlen(buffer)-offset,&sent,0,CURLWS_TEXT);
    offset+=sent;
    if(res==CURLE_OK){
      if(offset == strlen(buffer))
        break; /* finished sending */
    }
    fprintf(stderr,"SENDSTATUS_CODE %d\n",res);
    /*
    if(res==CURLE_AGAIN){
      fputs("CURLE_AGAIN Error I dont want to handle it rn",stderr);
      usleep(100000);
      res=CURLE_OK;
    }
    */
  }
}
static void gateway_easy_handle_establishing(struct discordgateway *gateway){
  cJSON *Identify = cJSON_CreateObject();
  cJSON *Identify_token=cJSON_CreateString(gateway->Token);
  cJSON_AddItemToObject(Identify,"token",Identify_token);
  cJSON *Identify_intents=cJSON_CreateNumber(gateway->Intents);
  cJSON_AddItemToObject(Identify,"intents",Identify_intents);

  cJSON *Properties = cJSON_CreateObject();
  cJSON *Properties_os = cJSON_CreateString("nixos");
  cJSON_AddItemToObject(Properties,"os",Properties_os);
  cJSON *Properties_browser = cJSON_CreateString("cgateway");
  cJSON_AddItemToObject(Properties,"browser",Properties_browser);
  cJSON *Properties_device = cJSON_CreateString("disco");
  cJSON_AddItemToObject(Properties,"device",Properties_device);

  cJSON_AddItemToObject(Identify,"properties",Properties);
  char *jsonidentify=cJSON_PrintUnformatted(Identify);
  printf("\n\nIDENTIFY:%s\n\n",jsonidentify);



  CURLcode res = CURLE_OK;
  char *buffer=calloc(256,sizeof(char));
  size_t buffer_size=256;
  size_t offset = 0;
  int stage=0;
  char* hello=gateway_recieve(gateway);
  printf("%p %s",hello,hello);
  cJSON *hellopayload=cJSON_Parse(hello);
  if(hellopayload==NULL){
    fputs("Could not parse (HELLO payload)",stderr);
    return;
  }
  free(hello);
  cJSON* op = cJSON_GetObjectItemCaseSensitive(hellopayload, "op");
  if(op==NULL || !cJSON_IsString(op)){
    fputs("(HELLO payload).op either NULL or not a string",stderr);
    return;
  }
  cJSON *d = cJSON_GetObjectItemCaseSensitive(hellopayload, "d");


  gateway_send(gateway,jsonidentify);
  cJSON_Delete(Identify);

  while(1){
    
  };
  return;
}

struct discordgateway gateway_easy_init(void (*gatewayfunction)()){
  struct discordgateway newgateway;
  newgateway.curl = curl_easy_init();
  newgateway.userdata=gatewayfunction;
  //curl_easy_setopt(newgateway.curl, CURLOPT_SSL_VERIFYSTATUS,1L);
  curl_easy_setopt(newgateway.curl, CURLOPT_CAINFO,"./cacert.pem");
  curl_easy_setopt(newgateway.curl, CURLOPT_CAPATH,"./cacert.pem");

  newgateway.gatewayfunction=gateway_easy_handle_establishing;
  return newgateway;
}
#endif
