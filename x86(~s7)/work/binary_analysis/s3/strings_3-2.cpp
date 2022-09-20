#include <iostream>
#include <string>

int main(){
  std::string input;
  std::cin >> input;

  if(input == "secret_key"){
    std::cout << "success" << std::endl;
  } else {
    std::cout << "fail" << std::endl;
  }

  return 0;
}
