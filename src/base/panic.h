#ifndef BASE_PANIC_H_
#define BASE_PANIC_H_

#include <cstdlib>

#include <iostream>
#include <string>


void panic(const std::string &msg, int code) {
  std::clog << msg << std::endl;
  exit(code);
}

#endif // BASE_PANIC_H_
