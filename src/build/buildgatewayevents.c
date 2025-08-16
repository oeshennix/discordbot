#include <string.h>
#include <stdio.h>
#include "gateway_events_list.c"
void sortGatewayEvents(){
  size_t length=0;
  while(GatewayEvents[length]){
    ++length;
  }
  //sort
  for(int box=0;box<length;box++){
    for(int i=0;i<length-1-box;i++){
      if(strcmp(GatewayEvents[i],GatewayEvents[i+1])>0){
        char* temp=GatewayEvents[i+1];
        GatewayEvents[i+1]=GatewayEvents[i];
        GatewayEvents[i]=temp;
      }
    }
  }
}
int main(){
  FILE *outputFile;
  outputFile=fopen("gateway_events.c","w");
  
  sortGatewayEvents();
  fprintf(outputFile,"static char* GatewayEventsSorted[]={\n");

  size_t i=0;
  while(GatewayEvents[i]){
    fprintf(outputFile,"  \"%s\",\n",GatewayEvents[i]);
    ++i;
  }
  int gatewaybinmax=0;
  while(i>>gatewaybinmax){
    printf("%ld\n",i>>gatewaybinmax);
    gatewaybinmax++;
  }
  fprintf(outputFile,"};\n");
  fprintf(outputFile,"\n#define GATEWAYEVENTBINMAX %d",gatewaybinmax);
  fclose(outputFile);

  //create headerfile
  outputFile=fopen("gateway_events.h","w");
  i=0;
  //fprintf(struct GatewayFunction
  //fprintf(outputFile,"#define \n",GatewayEvents[i],i);
  while(GatewayEvents[i]){
    fprintf(outputFile,"#define %s %ld\n",GatewayEvents[i],i);
    ++i;
  }
  fclose(outputFile);
}
