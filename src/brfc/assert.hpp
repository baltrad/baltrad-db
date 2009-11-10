#ifndef BRFC_ASSERT_H
#define BRFC_ASSERT_H

#include <stdexcept>
#include <string>

#include <boost/lexical_cast.hpp>

namespace brfc {

class assertion_error : public std::runtime_error {
  public:
    assertion_error(const std::string& msg)
            : std::runtime_error(msg) {
    }
};

inline void assert_fn(bool expr,
                      const std::string& expr_str,
                      const std::string& file,
                      unsigned int line) {
    if (!expr)
        throw assertion_error(file + ":" + 
                              boost::lexical_cast<std::string>(line) +
                              "   expr: " + expr_str);
}

}

#define BRFC_ASSERT(expr) assert_fn(expr, #expr, __FILE__, __LINE__);

#endif // BRFC_ASSERT_H

