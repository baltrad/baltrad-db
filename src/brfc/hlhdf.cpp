#include <brfc/hlhdf.hpp>

namespace brfc {

void
init_hlhdflib() {
    static bool inited = false;
    if (not inited) {
        HL_init();
        HL_disableErrorReporting();
        inited = true;
    }
}

} // namespace brfc
