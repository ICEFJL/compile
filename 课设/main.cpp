#include "pl.h"

int main() {
  string inputfile;
  cout << "PL/0 compiler/interpreter\n";
  cout << "input file name:";
  cin >> inputfile;

  init();
  if (!read_file(inputfile))
    exit(-1);
  ast result = parse();
  close_file();

  if (!compile_error) {
    result.print();
    for (int i = 0; i < exec_code.size(); ++i)
      printf("0x%.8x %s 0x%.8x 0x%.8x\n", i, opcode_name[exec_code[i].opcode],
             exec_code[i].level, exec_code[i].opnum);
  } else
    exit(0);

  vm_run();

  return 0;
}
