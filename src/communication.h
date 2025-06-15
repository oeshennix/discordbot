#include <stddef.h>

struct outgoingrequest{
  char* data;
  size_t len;
};

size_t ReadData(char* buffer, size_t size, size_t count, struct outgoingrequest* str);
int discordsetupcommunications();
int discordcleanupcommunications();
int discordsendrawmessage();
