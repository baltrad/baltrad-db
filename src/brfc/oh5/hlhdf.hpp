#ifndef BRFC_OH5_HLHDF_HPP
#define BRFC_OH5_HLHDF_HPP

extern "C" {
    #include <hlhdf.h>
}

namespace brfc {
namespace oh5 {

/**
 * @brief initialize HL-HDF library if not already initialized
 */
void init_hlhdflib();

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_HLHDF_HPP
