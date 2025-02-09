#ifndef __PL_H__
#define __PL_H__

#pragma GCC optimize(2)

#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <vector>

std::ifstream file_handle;
std::string line_code;
int line;
char c;

bool compile_error;
void die(std::string error_info, int l = -1) {
  compile_error = true;
  if (l < 0)
    std::cout << "line " << line << ",column " << line_code.length() << ":"
              << error_info << '\n';
  else
    std::cout << "line " << l << ":" << error_info << '\n';
  return;
}
#include "pl_ast.h"
#include "pl_code.h"
#include "pl_lex.h"
#include "pl_parse.h"
#include "pl_vm.h"

using namespace std;

#endif
