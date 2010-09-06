#ifndef BRFC_MOCK_HASHER_HPP
#define BRFC_MOCK_HASHER_HPP

#include <gmock/gmock.h>

#include <brfc/FileHasher.hpp>

namespace brfc {

class MockHasher : public FileHasher {
  public:
    MOCK_CONST_METHOD0(do_name, String());
    MOCK_METHOD2(do_hash, String(const oh5::File&, const oh5::Source&));
};

} // namespace brfc

#endif // BRFC_MOCK_HASHER_HPP
