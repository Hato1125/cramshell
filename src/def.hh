#ifndef _CLAMSHELL_DEF_HH
#define _CLAMSHELL_DEF_HH

namespace clamshell {
  constexpr int major = 1;
  constexpr int minor = 0;
  constexpr int patch = 0;
}

#ifdef DEBUG
  #define IF_RELEASE(code)
  #define IF_DEBUG(code) code
#else
  #define IF_DEBUG(code)
  #define IF_RELEASE(code) code
#endif

#endif
