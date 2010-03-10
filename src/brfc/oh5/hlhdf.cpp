#include <brfc/oh5/hlhdf.hpp>

namespace brfc {
namespace oh5 {

void
init_hlhdflib() {
    static bool inited = false;
    if (not inited) {
        HL_init();
        HL_disableErrorReporting();
        inited = true;
    }
}

} // namespace oh5
} // namespace brfc
