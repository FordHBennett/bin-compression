#include "debug_functions.hpp"
#include <iostream>

#define ERROR_MSG_MACRO(msg) \
    std::cerr << msg << " OCCURED IN: " << '\n'; \
    std::cerr << "      File: " << __FILE__ << '\n'; \
    std::cerr << "      Function: " << __PRETTY_FUNCTION__ << '\n'; \
    std::cerr << "      Line: " << __LINE__ << '\n'; \

#define ERROR_MSG_AND_EXIT_MACRO(msg) \
    std::cerr << msg << " OCCURED IN: " << '\n'; \
    std::cerr << "      File: " << __FILE__ << '\n'; \
    std::cerr << "      Function: " << __PRETTY_FUNCTION__ << '\n'; \
    std::cerr << "      Line: " << __LINE__ << std::endl; \
    std::exit(EXIT_FAILURE);

#ifdef DEBUG_MODE
void PRINT_DEBUG(const std::string& message) {
    std::cerr << message.c_str() << '\n';
}
#else
void PRINT_DEBUG(const std::string& message) {
}
#endif

#ifdef DEBUG_MODE
void ERROR_MSG(const std::string& message) {
    ERROR_MSG_MACRO(message.c_str());
}
#else
void ERROR_MSG(const std::string& message) {
}
#endif

#ifdef DEBUG_MODE
void ERROR_MSG_AND_EXIT(const std::string& message) {
    ERROR_MSG_AND_EXIT_MACRO(message.c_str());
}
#else
void ERROR_MSG_AND_EXIT(const std::string& message) {
}
#endif