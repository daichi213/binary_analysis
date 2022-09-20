#include <stdio.h>

int main(){
  int flag = 0;
  char buf[16];

  scanf("%s", buf);
  if(flag != 0){
    printf("hacked!\n");
  }else{
    printf("failed!\n");
  }

  return 0;
}


