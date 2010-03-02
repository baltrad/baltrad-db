#ifndef BRFC_DEFAULT_FILE_NAMER_HPP
#define BRFC_DEFAULT_FILE_NAMER_HPP

#include <brfc/FileNamer.hpp>

namespace brfc {

/**
 * @brief default FileNamer implementation
 *
 * name files as Z_\$(type)_C_\$(CCCC)_ \$(timestamp)_\$(freeformat).h5
 *
 * where:
 *   - @c type is the value of "/what/object"
 *   - @c CCCC is the WMO CCCC associated with the Source
 *   - @c timestamp is the value of "/what/date" and "/what/time" formatted
 *     as @c yyyyMMddhhmmss
 *   - @c freeformat contains the node_id associated with the Source
 */
class DefaultFileNamer : public FileNamer {
  public:
    DefaultFileNamer();

  protected:
    virtual QString do_name(const File& f) const;
};

} // namespace brfc

#endif // BRFC_DEFAULT_FILE_NAMER_HPP
