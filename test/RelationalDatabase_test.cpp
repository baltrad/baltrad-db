#include <gtest/gtest.h>

#include <brfc/oh5/Source.hpp>
#include <brfc/oh5/SourceCentre.hpp>
#include <brfc/oh5/SourceRadar.hpp>

#include <brfc/test/TestRDB.hpp>

#include "config.hpp"
#include "common.hpp"

namespace brfc {

class RelationalDatabase_test : public testing::TestWithParam<const char*> {
  public:
    RelationalDatabase_test()
            : db(TestRDBEnv::get_database(GetParam())) {

    }

    virtual void SetUp() {

    }

    virtual void TearDown() {
        db->clean();
    }

    test::TestRDB* db;
};


TEST_P(RelationalDatabase_test, load_source_by_plc) {
    shared_ptr<oh5::Source> src = db->load_source("PLC:Legionowo");
    EXPECT_TRUE(src);
    shared_ptr<oh5::SourceRadar> radar =
        dynamic_pointer_cast<oh5::SourceRadar>(src);
    ASSERT_TRUE(radar);
    EXPECT_EQ("Legionowo", radar->place());
    EXPECT_EQ("PL41", radar->radar_site());
    ASSERT_TRUE(radar->centre());
    EXPECT_EQ("pl", radar->centre()->node_id());
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(RelationalDatabase_test_p,
                        RelationalDatabase_test,
                        ::testing::ValuesIn(test_dsns));
#endif // BRFC_TEST_DSN_COUNT

} // namespace brfc
