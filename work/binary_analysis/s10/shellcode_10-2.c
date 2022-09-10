#include <stdio.h>

void vuln() {
  char buffer[128] = {};
  printf("%p\n", buffer);
  scanf("%[^\n]", buffer);

  return;
}

int main() {
  vuln();
  printf("failed\n");
  return 0;
}
