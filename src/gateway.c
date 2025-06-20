#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "gateway.h"
#include <ncursesw/ncurses.h>
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
static struct gateway_easy_attributes{
  void(*dispatch)();
  void(*ready)();
}
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
    //fputs("\x1b[9;2H1",stderr);
    res = curl_ws_recv(gateway->curl,buffer+offset,buffer_size-offset,&recv,&meta);
    offset+=recv;
    if(res==CURLE_OK){
      if(meta->bytesleft==0){
        return buffer;
      }else if(meta->bytesleft > buffer_size - offset){
        fflush(stdout);
        buffer_size*=2;
        buffer=realloc(buffer,buffer_size);
      }
    }
    //printf("%ld",res);
    if(res==CURLE_AGAIN){
      //fputs("\x1b[10;2;HCURLE_AGAIN Error I dont want to handle it rn",stderr);
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
    //fprintf(stderr,"SENDSTATUS_CODE %d\n",res);
    if(res==CURLE_AGAIN){
      fputs("CURLE_AGAIN Error I dont want to handle it rn",stderr);
      usleep(100000);
      res=CURLE_OK;
    }
  }
}
typedef struct _gateway_heartbeat_deferred{
  double time;
  struct discordgateway* gateway;
} gateway_heartbeat_deferred;

static void *gateway_Heartbeat(void* arg){
  gateway_heartbeat_deferred *data = (gateway_heartbeat_deferred*)arg;
  long heartbeattime=data->time;
  struct discordgateway *gateway=data->gateway;

  printf("loop heartbeat");
  while(1){
    printf("await heartbeat");
    fflush(stdout);
    usleep(heartbeattime);
    printf("await heartbeat2");
    fflush(stdout);
    cJSON *jsonpayload=cJSON_CreateObject();
    cJSON_AddNumberToObject(jsonpayload,"op",1);
    cJSON_AddNumberToObject(jsonpayload,"d",gateway->sequence);
    char* payload=cJSON_PrintUnformatted(jsonpayload);
    cJSON_Delete(jsonpayload);
    gateway_send(gateway,payload);
    printf("sent heartbeat");
    free(payload);
  }
}

static void gateway_easy_handle_establishing(struct discordgateway *gateway){
  char *payload_recv;
  cJSON *IdentifyPayload = cJSON_CreateObject();
  cJSON_AddNumberToObject(IdentifyPayload,"op",2);

  cJSON *Identify = cJSON_CreateObject();
  cJSON_AddStringToObject(Identify,"token",gateway->Token);
  cJSON_AddNumberToObject(Identify,"intents",gateway->Intents);

  cJSON *Properties = cJSON_CreateObject();
  cJSON *Properties_os = cJSON_CreateString("linux");
  cJSON_AddItemToObject(Properties,"os",Properties_os);
  cJSON *Properties_browser = cJSON_CreateString("unknown");
  cJSON_AddItemToObject(Properties,"browser",Properties_browser);
  cJSON *Properties_device = cJSON_CreateString("unknown");
  cJSON_AddItemToObject(Properties,"device",Properties_device);

  cJSON_AddItemToObject(Identify,"properties",Properties);
  cJSON_AddItemToObject(IdentifyPayload,"d",Identify);
  char *jsonidentify=cJSON_PrintUnformatted(IdentifyPayload);
  cJSON_Delete(Identify);
  //printf("\n\nIDENTIFY:%s\n\n",jsonidentify);

  CURLcode res = CURLE_OK;
  char *buffer=calloc(256,sizeof(char));
  size_t buffer_size=256;
  size_t offset = 0;
  int stage=0;
  payload_recv=gateway_recieve(gateway);
  printf("%p %s",payload_recv,payload_recv);
  cJSON *hellopayload=cJSON_Parse(payload_recv);
  free(payload_recv);
  if(hellopayload==NULL){
    fputs("Could not parse (HELLO payload)",stderr);
    return;
  }
  cJSON* jsonop = cJSON_GetObjectItemCaseSensitive(hellopayload, "op");
  if(jsonop==NULL || !cJSON_IsNumber(jsonop)){
    fputs("(HELLO payload).op either NULL or not a string",stderr);
    return;
  }
  cJSON *jsond = cJSON_GetObjectItemCaseSensitive(hellopayload, "d");
  if(jsond==NULL || !cJSON_IsObject(jsond)){
    fputs("(HELLO payload).d either NULL or not an object",stderr);
    return;
  }
  cJSON *jsonheartbeat = cJSON_GetObjectItemCaseSensitive(jsond,"heartbeat_interval");
  if(jsonheartbeat==NULL || !cJSON_IsNumber(jsonheartbeat)){
    fputs("(HELLO payload).d either NULL or not an object",stderr);
    return;
  }
  long heartbeat=(long)jsonheartbeat->valuedouble;

  long heartbeat_micro=heartbeat*1000;

  cJSON *heartbeatpayload=cJSON_CreateObject();
  cJSON_AddNumberToObject(heartbeatpayload,"op",1);
  cJSON_AddNullToObject(heartbeatpayload,"d");
  char *jsonpayload=cJSON_PrintUnformatted(heartbeatpayload);
  printf("%s\n\n",jsonpayload);
  cJSON_Delete(heartbeatpayload);
  gateway_send(gateway,jsonpayload);
  free(jsonpayload);
  
  pthread_t heartbeat_deferred;
  gateway_heartbeat_deferred arg;
  arg.time=heartbeat_micro;
  arg.gateway=gateway;
  
  pthread_create(&heartbeat_deferred,NULL,gateway_Heartbeat,&arg);
  pthread_detach(heartbeat_deferred);

  payload_recv=gateway_recieve(gateway);
  printf("ack%sack",ACK);
  free(payload_recv);

  printf("%s\n\n",jsonidentify);
  CURLcode *i=gateway_send(gateway,jsonidentify);
  //printf("SDFJSDJKSDFKLSDFKLSDFLKJSDFKLJLKDJSF%d\n\n",i);
  free(jsonidentify);
  
  printf("GATEWAY LISTENING");
  fflush(stdout);

  while(1){
    usleep(10000);
    payload_recv=gateway_recieve(gateway);
    cJSON *jsonpayload=cJSON_Parse(payload_recv);
    if(jsonpayload==NULL){
      fputs(stderr,"Could not parse payload received");
      break;
    }
    long op = (long)cJSON_GetObjectItemCaseSensitive(jsonpayload,"op");
    struct gateway_easy_attributes attr=(struct gateway_easy_attributes)gateway->userdata;
    switch(op){
      case(0)://Dispatch
        if(gateway_easy_attributes.dispatch)
          gateway_easy_attributes.dispatch(jsonpayload);
        break;
      case(1)://HeartBeat
        break;
      case(11)://HearbeatACK
        break;
      default:
        fputs("opcode not recognized");
        break;
    }

    cJSON_Delete(jsonpayload);
    free(payload_recv);
  };
  printf("HOW!!!");
  return;
}

struct discordgateway gateway_easy_init(void (*dispatchfunction)(),void (*readyfunction)()){
  if(gatewayfunction==NULL){
    return NULL;
  }
  struct discordgateway newgateway;
  newgateway.curl = curl_easy_init();
  struct gateway_easy_attributes attr;
  attr.dispatch=dispatch;
  attr.ready=readyfunction;
  newgateway.userdata=gatewayfunction;
  //curl_easy_setopt(newgateway.curl, CURLOPT_SSL_VERIFYSTATUS,1L);
  curl_easy_setopt(newgateway.curl, CURLOPT_CAINFO,"./cacert.pem");
  curl_easy_setopt(newgateway.curl, CURLOPT_CAPATH,"./cacert.pem");
  //curl_easy_setopt(newgateway.curl, CURLOPT_WS_OPTIONS,CURLWS_NOAUTOPONG);

  newgateway.gatewayfunction=gateway_easy_handle_establishing;
  return newgateway;
}
#endif
