#ifndef BRFC_MOCK_HASHER_HPP
#define BRFC_MOCK_HASHER_HPP

#include <gmock/gmock.h>

#include <brfc/FileHasher.hpp>

namespace brfc {

class MockHasher : public FileHasher {
  public:
    MockHasher()
        : FileHasher("mock") {
    }

    MOCK_CONST_METHOD1(do_hash, String(const oh5::Oh5File&));
};

} // namespace brfc

#endif // BRFC_MOCK_HASHER_HPP
