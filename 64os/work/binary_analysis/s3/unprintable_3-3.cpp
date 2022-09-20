#include <iostream>
#include <string>

int main(){
  std::string input;
  std::cin >> input;

  if(input == "\x01\x02\x03\x04\x05"){
    std::cout << "success" << std::endl;
  } else {
    std::cout << "fail" << std::endl;
  }

  return 0;
}

