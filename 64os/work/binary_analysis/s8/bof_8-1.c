#include <stdio.h>

void pwn(){
  printf("hacked!\n");
}

void vuln(){
  char overflowme[48];
  scanf("%[^\n]", overflowme);
}

int main(){
  vuln();
  printf("failed!\n");
  return 0;
}
