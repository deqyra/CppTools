#ifndef MAKE_BASIC_EXCEPTION_HPP
#define MAKE_BASIC_EXCEPTION_HPP

#include <stdexcept>
#include <string>

#define DECLARE_BASIC_EXCEPTION(NAME) class NAME : public std::exception    \
    {                                                                       \
        private:    /* Attributes */                                        \
            std::string _message;                                           \
                                                                            \
        public:     /* Public methods */                                    \
            NAME(const char* message);                                      \
            NAME(std::string message);                                      \
            const char* what() const noexcept;                              \
    };

#define DEFINE_BASIC_EXCEPTION(NAME)        \
    NAME::NAME(const char* message) :       \
        _message(message)                   \
    {                                       \
                                            \
    }                                       \
                                            \
    NAME::NAME(std::string message) :       \
        _message(message)                   \
    {                                       \
                                            \
    }                                       \
                                            \
    const char* NAME::what() const noexcept \
    {                                       \
        return _message.c_str();            \
    }

#endif//MAKE_BASIC_EXCEPTION_HPP