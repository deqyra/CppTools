#include <stdexcept>
#include <cstring>

#include <cpptools/exception/exception.hpp>

#include "debug_log.hpp"

// This header defines CPPTOOLS_DEBUG_ASSERT, whose behaviour depends on CPPTOOLS_DEBUG_POLICY:
// - if it is equal to CPPTOOLS_DEBUG_POLICY_LOG, a message is logged to an appropriate
//   channel of the debug log, with an appropriate error level
// - if it is equal to CPPTOOLS_DEBUG_POLICY_THROW, an exception of appropriate type is thrown
// - if it is equal to CPPTOOLS_DEBUG_POLICY_LOG_AND_THROW, both of the above actions take place
// CPPTOOLS_DEBUG_ENABLED must be defined and set to a non-zero value to enable debug altogether

#ifndef CPPTOOLS_DEBUG_POLICY_LOG
#   define CPPTOOLS_DEBUG_POLICY_LOG 1
#endif

#ifndef CPPTOOLS_DEBUG_POLICY_THROW
#   define CPPTOOLS_DEBUG_POLICY_THROW 2
#endif

#ifndef CPPTOOLS_DEBUG_POLICY_LOG_AND_THROW
#   define CPPTOOLS_DEBUG_POLICY_LOG_AND_THROW 3
#endif

#ifndef CPPTOOLS_DEBUG_POLICY
#   define CPPTOOLS_DEBUG_POLICY CPPTOOLS_DEBUG_POLICY_THROW
#endif

#if CPPTOOLS_DEBUG_ENABLED != 0
#   ifdef CPPTOOLS_DEBUG_ASSERT
#       warning CPPTOOLS_DEBUG_ASSERT is already defined, undefing...
#       undef CPPTOOLS_DEBUG_ASSERT
#   endif
#   if CPPTOOLS_DEBUG_POLICY == CPPTOOLS_DEBUG_POLICY_LOG
#       define CPPTOOLS_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...) \
            if (!(cond))                                                       \
            {                                                                  \
                using namespace tools::internal::debug_log;                 \
                log(level, channel, message);                                  \
            }
#   elif CPPTOOLS_DEBUG_POLICY == CPPTOOLS_DEBUG_POLICY_THROW
#       define CPPTOOLS_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...) \
            if (!(cond))                                                       \
            {                                                                  \
                CPPTOOLS_THROW(ex_t, ##__VA_ARGS__).with_message(message);     \
            }
#   elif CPPTOOLS_DEBUG_POLICY == CPPTOOLS_DEBUG_POLICY_LOG_AND_THROW
#       define CPPTOOLS_DEBUG_ASSERT(cond, channel, level, message, ex_t, ...) \
            if (!(cond))                                                       \
            {                                                                  \
                using namespace tools::internal::debug_log;                 \
                log(level, channel, message);                                  \
                CPPTOOLS_THROW(ex_t, ##__VA_ARGS__).with_message(message);     \
            }
#   else
#       define CPPTOOLS_DEBUG_ASSERT(level, channel, cond, message, throw_cond)
#   endif
#else
#   define CPPTOOLS_DEBUG_ASSERT(level, channel, cond, message, throw_cond)
#endif
