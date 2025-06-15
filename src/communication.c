#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "communication.h"

size_t ReadData(char* buffer, size_t size, size_t count, struct outgoingrequest* str){
  if(str->len==0)
    return 0;

  size_t to_copy = (str->len < count) ? (str->len) : count;
  memcpy(buffer, str->data, to_copy);
  str->len -= to_copy;
  str->data += to_copy;
  return to_copy;
}

static CURL *basediscordcomm;
int discordsetupcommunications(char* discordurl){
  basediscordcomm=curl_easy_init();
  if(!basediscordcomm){
    return 0;
  }
  struct curl_slist *list;
  list = curl_slist_append(NULL, "Content-Type: application/json");
  curl_easy_setopt(basediscordcomm, CURLOPT_SSL_VERIFYSTATUS,1L);
  curl_easy_setopt(basediscordcomm, CURLOPT_CAINFO,"./cacert.pem");
  curl_easy_setopt(basediscordcomm, CURLOPT_CAPATH,"./cacert.pem");

  curl_easy_setopt(basediscordcomm, CURLOPT_POST,1L);
  curl_easy_setopt(basediscordcomm, CURLOPT_HTTPHEADER,list);

  curl_easy_setopt(basediscordcomm, CURLOPT_URL, discordurl);

  return 1;
}
int discordcleanupcommunications(){
  curl_easy_cleanup(basediscordcomm);
  return 1;
}
int discordsendrawmessage(char* message){
  //curl_easy_setopt(curl, CURLOPT_URL, discordwebhookurl);
  CURLcode res;
  struct outgoingrequest r;
  r.data=message;
  r.len=strlen(message);
  curl_easy_setopt(basediscordcomm, CURLOPT_READFUNCTION, ReadData);
  curl_easy_setopt(basediscordcomm, CURLOPT_READDATA, &r);
  res = curl_easy_perform(basediscordcomm);
  /* Check for errors */
  if(res != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));

  return 1;
}
