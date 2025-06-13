#include <stdio.h>
#include <string.h>
#include "../discord.h"
int main(){
  char* data="{\"content\":\"hello\"}";
  struct discordoutgoingrequest r;
  r.data=data;
  r.len=strlen(data);
  for(int i=0;i<10;i++){
    char i[255];
    int am=ReadData(i,sizeof(char),12,&r);
    printf("%s %d\n",i,am);
  }
  return 0;
}
