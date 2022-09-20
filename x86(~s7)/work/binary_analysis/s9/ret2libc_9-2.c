#include <stdio.h>
#include <string.h>

char global[] = "/bin/sh";

void vuln(){
  printf("global:%p\n",global);
  printf("文字列を入力してください\n");
  char overflowme[32];
  scanf("%[^\n]", overflowme);
}

int main(){
  vuln();
  printf("failed!\n");
  return 0;
}

