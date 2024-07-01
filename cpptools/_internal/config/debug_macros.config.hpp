#include <stdexcept>
#include <cstring>

#include <cpptools/exception/exception.hpp>

#include <cpptools/_internal/assume.hpp>
#include <cpptools/_internal/debug_expr.hpp>
#include <cpptools/_internal/debug_log.hpp>
#include <cpptools/_internal/utility_macros.hpp>

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

#ifndef @CONFIGURE_DEBUG_PROJECT_MACRO@_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
# error Make sure to include undef_debug_macros.hpp after this file. When you have, silence this error by defining @CONFIGURE_DEBUG_PROJECT_MACRO@_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE prior to including this file.
#else
# undef @CONFIGURE_DEBUG_PROJECT_MACRO@_I_HAVE_INCLUDED_UNDEF_DEBUG_MACROS_LATER_ON_IN_THIS_FILE
#endif

// !!! IMPORTANT !!!
//
// The below definitions are non-empty ONLY IF @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ENABLED is defined
// AND non-zero.

#if @CONFIGURE_DEBUG_PROJECT_MACRO@_ENABLE_DEBUG_MASTER_SWITCH != 0
# define @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ENABLED 1
#endif

// @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ASSERT: check that an expression is true, taking an action
// to report failure. The nature of the action taken depends on the value of
// @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY, which must be defined prior to including this file.
// Possible values:
// - @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_LOG:
//   a message is logged to an appropriate channel of the debug log, with an
//   appropriate error level
// - @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_THROW:
//   an exception of appropriate type is thrown
// - @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_LOG_AND_THROW:
//   both of the above actions take place
// Default value: @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_THROW

#ifndef @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_LOG
# define @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_LOG 1
#endif

#ifndef @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_THROW
# define @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_THROW 2
#endif

#ifndef @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_LOG_AND_THROW
# define @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_LOG_AND_THROW 3
#endif

#ifndef @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY
# define @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_THROW
#endif

#if @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ENABLED != 0
# ifdef @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ASSERT
#   error @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ASSERT is already defined.
# elif @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY == @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_LOG
#   define @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...)     \
      {                                                                        \
        using namespace ::tools::internal::debug_expr;                         \
        if (!(cond)) {                                                         \
          using namespace ::tools::internal::debug_log;                        \
          std::string full_message = PREPEND_FUNCTION(message);                \
          log(level, channel, full_message);                                   \
        }                                                                      \
      }
# elif @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY == @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_THROW
#   define @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...)     \
      {                                                                        \
        using namespace ::tools::internal::debug_expr;                         \
        if (!(cond)) {                                                         \
          std::string full_message = PREPEND_FUNCTION(message);                \
          @CONFIGURE_DEBUG_PROJECT_MACRO@_THROW(ex_t, __VA_ARGS__).with_message(full_message);        \
        }                                                                      \
      }
# elif @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY == @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY_LOG_AND_THROW
#   define @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...)     \
      {                                                                        \
        using namespace ::tools::internal::debug_expr;                         \
        if (!(cond)) {                                                         \
          using namespace ::tools::internal::debug_log;                        \
          std::string full_message = PREPEND_FUNCTION(message);                \
          log(level, channel, full_message);                                   \
          @CONFIGURE_DEBUG_PROJECT_MACRO@_THROW(ex_t, __VA_ARGS__).with_message(full_message);        \
        }                                                                      \
      }
# else
#   warn @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_POLICY is defined as an unknown policy. Debug assertions will be disabled.
#   define @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...)
# endif
#elif defined(@CONFIGURE_DEBUG_PROJECT_MACRO@_TURN_DEBUG_ASSERTIONS_INTO_ASSUMPTIONS)
# ifndef CPPTOOLS_ASSUME
#   error CPPTOOLS_ASSUME is undefined because no suitable implementation was found on your system. If appropriate, consider defining @CONFIGURE_DEBUG_PROJECT_MACRO@_ALLOW_EVALUATING_ASSUMPTIONS to a non-0 value.
# else
#   define @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...)     \
      CPPTOOLS_ASSUME(cond);
# endif
#else
# define @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...)
#endif

// @CONFIGURE_DEBUG_PROJECT_MACRO@_NOEXCEPT_RELEASE, @CONFIGURE_DEBUG_PROJECT_MACRO@_NOEXCEPT_RELEASE_AND: conditional 
// noexcept specification based on whether debug is enabled in a file. This can
// be used for function which are noexcept per se, but use debug assertions 
// which may throw in debug builds. 
// Since file-specific debug control macros cannot be known within this file, 
// this file instead uses macro @CONFIGURE_DEBUG_PROJECT_MACRO@_LOCAL_DEBUG_MACRO, which must be 
// defined prior to inclusion as an alias to the file-specific debug control 
// macro.

#if @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ENABLED != 0
# if defined(@CONFIGURE_DEBUG_PROJECT_MACRO@_NOEXCEPT_RELEASE)
#   error @CONFIGURE_DEBUG_PROJECT_MACRO@_NOEXCEPT_RELEASE is already defined.
# elif defined(@CONFIGURE_DEBUG_PROJECT_MACRO@_NOEXCEPT_RELEASE_AND)
#   error @CONFIGURE_DEBUG_PROJECT_MACRO@_NOEXCEPT_RELEASE_AND is already defined.
# else
#   define @CONFIGURE_DEBUG_PROJECT_MACRO@_NOEXCEPT_RELEASE           noexcept(@CONFIGURE_DEBUG_PROJECT_MACRO@_LOCAL_DEBUG_MACRO == 0)
#   define @CONFIGURE_DEBUG_PROJECT_MACRO@_NOEXCEPT_RELEASE_AND(cond) noexcept(@CONFIGURE_DEBUG_PROJECT_MACRO@_LOCAL_DEBUG_MACRO == 0 && (cond))
# endif
#else
# define @CONFIGURE_DEBUG_PROJECT_MACRO@_NOEXCEPT_RELEASE           noexcept
# define @CONFIGURE_DEBUG_PROJECT_MACRO@_NOEXCEPT_RELEASE_AND(cond) noexcept(cond)
#endif

#if @CONFIGURE_DEBUG_PROJECT_MACRO@_LOCAL_DEBUG_MACRO != 0
# if @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ENABLED == 0
#   warning @CONFIGURE_DEBUG_PROJECT_MACRO@_LOCAL_DEBUG_MACRO is defined and non-zero but @CONFIGURE_DEBUG_PROJECT_MACRO@_DEBUG_ENABLED is not, debug assertions will NOT run
# endif 
#endif
