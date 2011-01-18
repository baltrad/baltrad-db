#include <brfc/smart_ptr.hpp>

namespace brfc {

void
no_delete(const void* /*ptr*/) {
    // pass
}

} // namespace brfc
