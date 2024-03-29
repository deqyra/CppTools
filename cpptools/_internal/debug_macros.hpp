#include <stdexcept>
#include <cstring>

#include <cpptools/exception/exception.hpp>

#include "debug_expr.hpp"
#include "debug_log.hpp"
#include "utility_macros.hpp"

// !!! IMPORTANT !!!
//
// The below definitions are meant to be TRANSIENT. This header file does NOT
// have include guards, so that it can be included several times by different
// header/source files in the same translation unit, which can configure it in
// different ways prior to inclusion.
// This file:
// - DOES NOT check whether definitions exist prior to defining them;
// - DOES NOT undef the definitions prior to redefining them.
// Every source file which includes this file MUST undef all the definitions
// once it has no need for them anymore. This can be done by including 
// undef_debug_macros.hpp.
// To prevent accidents, this file emits an error if a reminder macro has not
// been defined prior to its own inclusion. If the macro is defined, no error is
// emitted and the macro is then undefined.

#ifndef CPPTOOLS_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
# error Make sure to include <cpptools/_internal/undef_debug_macros.hpp> after this file. When you have, silence this error by defining CPPTOOLS_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE prior to including this file.
#else
# undef CPPTOOLS_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
#endif

// !!! IMPORTANT !!!
//
// The below definitions are non-empty ONLY IF CPPTOOLS_DEBUG_ENABLED is defined
// AND non-zero.

#if CPPTOOLS_ENABLE_DEBUG_MASTER_SWITCH != 0
# define CPPTOOLS_DEBUG_ENABLED 1
#endif

// CPPTOOLS_DEBUG_ASSERT: check that an expression is true, taking an action
// to report failure. The nature of the action taken depends on the value of
// CPPTOOLS_DEBUG_POLICY, which must be defined prior to including this file.
// Possible values:
// - CPPTOOLS_DEBUG_POLICY_LOG:
//   a message is logged to an appropriate channel of the debug log, with an
//   appropriate error level
// - CPPTOOLS_DEBUG_POLICY_THROW:
//   an exception of appropriate type is thrown
// - CPPTOOLS_DEBUG_POLICY_LOG_AND_THROW:
//   both of the above actions take place
// Default value: CPPTOOLS_DEBUG_POLICY_THROW

#ifndef CPPTOOLS_DEBUG_POLICY_LOG
# define CPPTOOLS_DEBUG_POLICY_LOG 1
#endif

#ifndef CPPTOOLS_DEBUG_POLICY_THROW
# define CPPTOOLS_DEBUG_POLICY_THROW 2
#endif

#ifndef CPPTOOLS_DEBUG_POLICY_LOG_AND_THROW
# define CPPTOOLS_DEBUG_POLICY_LOG_AND_THROW 3
#endif

#ifndef CPPTOOLS_DEBUG_POLICY
# define CPPTOOLS_DEBUG_POLICY CPPTOOLS_DEBUG_POLICY_THROW
#endif

#if CPPTOOLS_DEBUG_ENABLED != 0
# ifdef CPPTOOLS_DEBUG_ASSERT
#   error CPPTOOLS_DEBUG_ASSERT is already defined.
# elif CPPTOOLS_DEBUG_POLICY == CPPTOOLS_DEBUG_POLICY_LOG
#   define CPPTOOLS_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...)     \
      {                                                                        \
        using namespace ::tools::internal::debug_expr;                         \
        if (!(cond)) {                                                         \
          using namespace ::tools::internal::debug_log;                        \
          std::string full_message = PREPEND_FUNCTION(message);                \
          log(level, channel, full_message);                                   \
        }                                                                      \
      }
# elif CPPTOOLS_DEBUG_POLICY == CPPTOOLS_DEBUG_POLICY_THROW
#   define CPPTOOLS_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...)     \
      {                                                                        \
        using namespace ::tools::internal::debug_expr;                         \
        if (!(cond)) {                                                         \
          std::string full_message = PREPEND_FUNCTION(message);                \
          CPPTOOLS_THROW(ex_t, __VA_ARGS__).with_message(full_message);        \
        }                                                                      \
      }
# elif CPPTOOLS_DEBUG_POLICY == CPPTOOLS_DEBUG_POLICY_LOG_AND_THROW
#   define CPPTOOLS_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...)     \
      {                                                                        \
        using namespace ::tools::internal::debug_expr;                         \
        if (!(cond)) {                                                         \
          using namespace ::tools::internal::debug_log;                        \
          std::string full_message = PREPEND_FUNCTION(message);                \
          log(level, channel, full_message);                                   \
          CPPTOOLS_THROW(ex_t, __VA_ARGS__).with_message(full_message);        \
        }                                                                      \
      }
# else
#   warn CPPTOOLS_DEBUG_POLICY is defined as an unknown policy. Debug assertions will be disabled.
#   define CPPTOOLS_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...)
# endif
#elif defined(CPPTOOLS_TURN_DEBUG_ASSERTIONS_INTO_ASSUMPTIONS)
# ifndef CPPTOOLS_ASSUME
#   error CPPTOOLS_ASSUME is undefined because no suitable implementation was found on your system. If appropriate, consider defining CPPTOOLS_ALLOW_EVALUATING_ASSUMPTIONS to a non-0 value.
# else
#   define CPPTOOLS_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...)     \
      CPPTOOLS_ASSUME(cond);
# endif
#else
# define CPPTOOLS_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...)
#endif

// CPPTOOLS_NOEXCEPT_RELEASE, CPPTOOLS_NOEXCEPT_RELEASE_AND: conditional 
// noexcept specification based on whether debug is enabled in a file. This can
// be used for function which are noexcept per se, but use debug assertions 
// which may throw in debug builds. 
// Since file-specific debug control macros cannot be known within this file, 
// this file instead uses macro CPPTOOLS_LOCAL_DEBUG_MACRO, which must be 
// defined prior to inclusion as an alias to the file-specific debug control 
// macro.

#if CPPTOOLS_DEBUG_ENABLED != 0
# if defined(NOEXCEPT_RELEASE)
#   error NOEXCEPT_RELEASE is already defined.
# elif defined(NOEXCEPT_RELEASE_AND)
#   error NOEXCEPT_RELEASE_AND is already defined.
# else
#   define CPPTOOLS_NOEXCEPT_RELEASE           noexcept(CPPTOOLS_LOCAL_DEBUG_MACRO == 0)
#   define CPPTOOLS_NOEXCEPT_RELEASE_AND(cond) noexcept(CPPTOOLS_LOCAL_DEBUG_MACRO == 0 && (cond))
# endif
#endif

#if CPPTOOLS_LOCAL_DEBUG_MACRO != 0
# if CPPTOOLS_DEBUG_ENABLED == 0
#   warning CPPTOOLS_LOCAL_DEBUG_MACRO is defined and non-zero but CPPTOOLS_DEBUG_ENABLED is not, debug assertions will NOT run
# endif
#endif
