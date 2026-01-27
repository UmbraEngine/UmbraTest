#pragma once
#include <iostream>

inline bool g_umbra_test_debug_enabled = 0;

inline void check_debug(int argc, char* argv[]){
  for(int i = 1; i < argc; ++i){
    std::string_view arg = argv[i];
    if(arg == "-d" || arg == "--debug"){
      g_umbra_test_debug_enabled = true;
    }
  }
}

