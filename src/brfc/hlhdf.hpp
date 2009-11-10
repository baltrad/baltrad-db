#ifndef BRFC_HLHDF_HPP
#define BRFC_HLHDF_HPP

extern "C" {
    #include <hlhdf.h>
}

namespace brfc {

/**
 * @brief initialize HL-HDF library if not already initialized
 */
void init_hlhdflib();

} // namespace brfc

#endif // BRFC_HLHDF_HPP
