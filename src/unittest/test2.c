#include <stdio.h>
int main(){
  FILE* data=fopen("data.json","r");
  for(int i=0;i<10;i++){
    char i[255];
    int am=fread(i,1,12,data);
    printf("%s %d\n",i,am);
  }
  return 0;
}
