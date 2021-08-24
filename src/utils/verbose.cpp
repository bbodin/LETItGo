/*
 * verbose.cpp
 *
 *  Created on: 9 févr. 2011
 *      Author: toky
 */

#include <execinfo.h>
#include <iostream>
#include <verbose.h>

namespace letitgo {
namespace utils {
int VERBOSE_MODE = MIN_LEVEL;
bool VERBOSE_GUESS = true;
bool VERBOSE_COLOR = true;
std::set<std::string> VERBOSE_CUSTOM_MODES;
void myterminate(int) {
  print_trace("", 0);
  exit(1);
}

/**
 * C++ stack traces with GCC
 * Website : http://tombarta.wordpress.com/2008/08/01/c-stack-traces-with-gcc/
 */
void print_trace(const char *file, int line) {
  const int max_depth = 100;
  int stack_depth;
  void *stack_addrs[max_depth];
  char **stack_strings;

  stack_depth = backtrace(stack_addrs, max_depth);
  stack_strings = backtrace_symbols(stack_addrs, stack_depth);

  // fprintf(out, "Call stack from %s:%d:\n", file, line);
  std::cerr << std::endl;
  std::cerr << PURPLE_COLOR;
  std::cerr << "Call stack from " << file << ":" << line << ":" << std::endl;
  for (int i = 1; i < stack_depth; i++) {
    // fprintf(out, "    %s\n", stack_strings[i]);
    std::cerr << "    " << stack_strings[i] << std::endl;
  }
  std::cerr << RESET_COLOR;
  delete (stack_strings); // malloc()ed by backtrace_symbols
                          // fflush(out);
}

} // namespace utils
} // namespace letitgo
