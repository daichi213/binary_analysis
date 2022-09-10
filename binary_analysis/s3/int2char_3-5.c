#include <stdio.h>
#include <stdlib.h>

int main(){
  int32_t *c = malloc(sizeof(int32_t));
  *c = 0x65746166;

  printf("%s\n", (char *)c);
}

