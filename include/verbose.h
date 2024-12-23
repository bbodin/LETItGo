/*
 * verbose.h
 *
 *  Created on: 1 févr. 2011
 *      Author: toky
 *
 */

#ifndef VERBOSE_H_
#define VERBOSE_H_

#include <iostream>
#include <set>
#include <string>

#define TXT_NO_IMPLEMENTATION "FIXME"
#define TXT_NEVER_HAPPEND "Internal error"

#ifdef __RELEASE_MODE__
#define __RELEASE__ "R"
#else
#ifdef __DEBUG_MODE__
#define __RELEASE__ "D"
#else
#define __RELEASE__ "X"
#endif
#endif

namespace utils {

inline const std::string getFilename(const std::string s) {
  return s.substr(s.find_last_of("/\\") + 1);
}

extern int VERBOSE_MODE;
extern bool VERBOSE_COLOR;
extern bool VERBOSE_GUESS;

extern std::set<std::string> VERBOSE_CUSTOM_MODES;


const int MIN_LEVEL = 0;
const int MAX_LEVEL = 10;

const int ERROR_LEVEL = 1;
const int WARNING_LEVEL = 2;
const int PB_LEVEL = 3;
const int INFO_LEVEL = 4;
const int ILP_LEVEL = 7;
const int DEBUG_LEVEL = 8;
const int EXTRA_DEBUG_LEVEL = 10;

void print_trace(const char *file, int line);
inline void set_verbose_mode(const int m) { VERBOSE_MODE = m; VERBOSE_CUSTOM_MODES.clear();}
inline void set_verbose_custom_mode(const std::string mode, bool v) {
	if (!v) {VERBOSE_CUSTOM_MODES.erase(mode);}
	if (v) {VERBOSE_CUSTOM_MODES.insert(mode);}
}

inline int is_info_verbose() { return VERBOSE_MODE >= INFO_LEVEL; }
inline int get_verbose_mode() { return VERBOSE_MODE; }
inline void set_verbose_color(const bool v) { utils::VERBOSE_COLOR = v; }
inline void set_verbose_guess(const bool v) { VERBOSE_GUESS = v; }

void myterminate(int);

} // namespace commons

// \033 remplace \e !!!
#define PURPLE_COLOR (utils::VERBOSE_COLOR ? "\033[1;35m" : "")
#define RED_COLOR (utils::VERBOSE_COLOR ? "\033[1;31m" : "")
#define YELLOW_COLOR (utils::VERBOSE_COLOR ? "\033[0;33m" : "")
#define GREEN_COLOR (utils::VERBOSE_COLOR ? "\033[1;32m" : "")
#define BLUE_COLOR (utils::VERBOSE_COLOR ? "\033[1;34m" : "")
#define RESET_COLOR (utils::VERBOSE_COLOR ? "\033[0m" : "")

#define __SHOW_LEVEL                                                           \
  "[ " << __RELEASE__ << "  " << utils::getFilename(__FILE__) << ":"         \
       << __LINE__ << "]" << RESET_COLOR << " "

#ifdef __RELEASE_MODE__
#define EXIT_ON_FAILURE() /*int* toto = NULL; *toto = 1;*/ exit(EXIT_FAILURE)
#else
#define EXIT_ON_FAILURE()                                                      \
  {                                                                            \
    int *toto = NULL;                                                          \
    *toto = 1;                                                                 \
  }                                                                            \
  exit(EXIT_FAILURE)

#endif

static inline bool VERBOSE_IS_EXTRA_DEBUG() {
  return (utils::VERBOSE_MODE >= utils::EXTRA_DEBUG_LEVEL);
}
static inline bool VERBOSE_IS_DEBUG() {
  return (utils::VERBOSE_MODE >= utils::DEBUG_LEVEL);
}
static inline bool VERBOSE_IS_INFO() {
  return (utils::VERBOSE_MODE >= utils::INFO_LEVEL);
}
static inline bool VERBOSE_IS_ERROR() {
  return (utils::VERBOSE_MODE >= utils::ERROR_LEVEL);
}
static inline bool VERBOSE_IS_WARNING() {
  return (utils::VERBOSE_MODE >= utils::WARNING_LEVEL);
}
static inline bool VERBOSE_IS_PB() {
  return (utils::VERBOSE_MODE >= utils::PB_LEVEL);
}
static inline bool VERBOSE_IS_ILP() {
  return (utils::VERBOSE_MODE >= utils::ILP_LEVEL);
}

#ifndef __RELEASE_MODE__
#define VERBOSE_EXTRA_DEBUG(m)                                                 \
  if (VERBOSE_IS_EXTRA_DEBUG())                                                \
    std::cerr << BLUE_COLOR << "[X]" << __SHOW_LEVEL << m                      \
              << std::string(20, ' ') << "\n"; // PRINT_STATE();
#define VERBOSE_CUSTOM_DEBUG(code, m)                                          \
  if (utils::VERBOSE_CUSTOM_MODES.count(code)) {                             \
	  std::cerr << PURPLE_COLOR << "[C][" << code << "]" << __SHOW_LEVEL << m                      \
	  << std::string(20, ' ') << "\n";   }
#define VERBOSE_DEBUG(m)                                                       \
  if (VERBOSE_IS_DEBUG())                                                      \
    std::cerr << BLUE_COLOR << "[D]" << __SHOW_LEVEL << m                      \
              << std::string(20, ' ') << "\n"; // PRINT_STATE();
#define VERBOSE_ILP(m)                                                         \
  if (VERBOSE_IS_ILP())                                                        \
    std::cerr << PURPLE_COLOR << "[L]" << __SHOW_LEVEL << m                    \
              << std::string(20, ' ') << "\n";
#define VERBOSE_DEBUG_ASSERT(test, mess)                                       \
  if (!(test)) {                                                               \
    VERBOSE_ERROR("Assertion failed : " << #test);                             \
    VERBOSE_ERROR(mess);                                                       \
    ERROR();                                                                   \
  }
#define VERBOSE_DEBUG_ASSERT_CONTAIN(a, b)                                     \
  if (!(a | b)) {                                                              \
    VERBOSE_ERROR("Assertion failed : " << #a << "(=" << a << ") ==" << #b     \
                                        << "(=" << b << ")");                  \
    ERROR();                                                                   \
  }
#define VERBOSE_DEBUG_ASSERT_EQUALS(a, b)                                      \
  if (a != b) {                                                                \
    VERBOSE_ERROR("Assertion failed : " << #a << "(=" << a << ") ==" << #b     \
                                        << "(=" << b << ")");                  \
    ERROR();                                                                   \
  }
#define VERBOSE_DEBUG_ASSERT_GreaterThan(a, b)                                 \
  if (a <= b) {                                                                \
    VERBOSE_ERROR("Assertion failed : " << #a << "(=" << a << ") >" << #b      \
                                        << "(=" << b << ")");                  \
    ERROR();                                                                   \
  }
#define VERBOSE_DEBUG_ASSERT_GreaterEqualThan(a, b)                            \
  if (a < b) {                                                                 \
    VERBOSE_ERROR("Assertion failed : " << #a << "(=" << a << ") >=" << #b     \
                                        << "(=" << b << ")");                  \
    ERROR();                                                                   \
  }
#else
#define VERBOSE_EXTRA_DEBUG(m) ;
#define VERBOSE_CUSTOM_DEBUG(code, m) ;
#define VERBOSE_CUSTOM(m) ;
#define VERBOSE_DEBUG(m) ;
#define VERBOSE_ILP(m) ;
#define VERBOSE_DEBUG_ASSERT(test, mess) ;
#define VERBOSE_DEBUG_ASSERT_CONTAIN(a, b) ;
#define VERBOSE_DEBUG_ASSERT_EQUALS(a, b) ;
#define VERBOSE_DEBUG_ASSERT_GreaterThan(a, b) ;
#define VERBOSE_DEBUG_ASSERT_GreaterEqualThan(a, b) ;
#endif

#define VERBOSE_INFO(m)                                                        \
  if (VERBOSE_IS_INFO())                                                       \
    std::cerr << GREEN_COLOR << "[I]" << __SHOW_LEVEL << m                     \
              << std::string(20, ' ') << "\n"; // PRINT_STATE();
#define VERBOSE_WARNING(m)                                                     \
  if (VERBOSE_IS_WARNING())                                                    \
    std::cerr << YELLOW_COLOR << "[W]" << __SHOW_LEVEL << m                    \
              << std::string(20, ' ') << "\n";
#define VERBOSE_ERROR(m)                                                       \
  if (VERBOSE_IS_ERROR())                                                      \
    std::cerr << RED_COLOR << "[E]" << __SHOW_LEVEL << m                       \
              << std::string(20, ' ') << "\n";
#define VERBOSE_DSE(m)                                                         \
  if (VERBOSE_IS_PB())                                                         \
    std::cerr << PURPLE_COLOR << "[DSE] " << RESET_COLOR << m;

#define VERBOSE_ASSERT(test, mess)                                             \
  if (!(test)) {                                                               \
    VERBOSE_ERROR("Assertion failed : " << #test);                             \
    VERBOSE_ERROR(mess);                                                       \
    ERROR();                                                                   \
  }
#define VERBOSE_ASSERT_CONTAIN(a, b)                                           \
  if (!(a | b)) {                                                              \
    VERBOSE_ERROR("Assertion failed : " << #a << "(=" << a << ") ==" << #b     \
                                        << "(=" << b << ")");                  \
    ERROR();                                                                   \
  }
#define VERBOSE_ASSERT_EQUALS(a, b)                                            \
  if (a != b) {                                                                \
    VERBOSE_ERROR("Assertion failed : " << #a << "(=" << a << ") ==" << #b     \
                                        << "(=" << b << ")");                  \
    ERROR();                                                                   \
  }
#define VERBOSE_ASSERT_GreaterThan(a, b)                                       \
  if (a <= b) {                                                                \
    VERBOSE_ERROR("Assertion failed : " << #a << "(=" << a << ") >" << #b      \
                                        << "(=" << b << ")");                  \
    ERROR();                                                                   \
  }
#define VERBOSE_ASSERT_GreaterEqualThan(a, b)                                  \
  if (a < b) {                                                                 \
    VERBOSE_ERROR("Assertion failed : " << #a << "(=" << a << ") >=" << #b     \
                                        << "(=" << b << ")");                  \
    ERROR();                                                                   \
  }

#ifndef __RELEASE_MODE__
#define VERBOSE_BAD_PROGRAMMING()                                              \
  {                                                                            \
    VERBOSE_ERROR("Internal Error");                                           \
    VERBOSE_BACKTRACE();                                                       \
    std::cerr << std::flush;                                                   \
    EXIT_ON_FAILURE();                                                         \
  }
#else
#define VERBOSE_BAD_PROGRAMMING()                                              \
  {                                                                            \
    VERBOSE_ERROR("Internal Error");                                           \
    VERBOSE_BACKTRACE();                                                       \
    std::cerr << std::flush;                                                   \
    EXIT_ON_FAILURE();                                                         \
  }
#endif
#define ERROR() VERBOSE_BAD_PROGRAMMING();
#define VERBOSE_FAILURE() VERBOSE_BAD_PROGRAMMING();
#define FAILED(MESSAGE)                                                        \
  {                                                                            \
    VERBOSE_ERROR(MESSAGE);                                                    \
    std::cerr << std::flush;                                                   \
    VERBOSE_BACKTRACE();                                                       \
    EXIT_ON_FAILURE();                                                         \
  }

#define VERBOSE_BACKTRACE() utils::print_trace(__FILE__, __LINE__);

#endif /* VERBOSE_H_ */
