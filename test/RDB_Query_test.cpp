#include <gtest/gtest.h>

#include <brfc/Query.hpp>
#include <brfc/ResultSet.hpp>

#include <brfc/expr/Factory.hpp>
#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Literal.hpp>
#include <brfc/expr/BinaryOperator.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/Root.hpp>
#include <brfc/oh5/File.hpp>

#include <brfc/test/TestRDB.hpp>

#include <boost/foreach.hpp>

#include <QtCore/QDate>
#include <QtCore/QStringList>
#include <QtCore/QTime>
#include <QtCore/QVariant>

#include "config.hpp"
#include "common.hpp"

namespace brfc {

/**
 * test data for this fixture:
 *
 * src name obj  date       time  xsize ysize
 *   1 td1  PVOL 2000-01-01 12:00     1     2
 *   2 td2  PVOL 2000-01-01 12:01     2     2
 *   1 td3  PVOL 2000-01-01 12:02     3
 *                                    3
 *   2 td4  CVOL 2001-01-01 12:00     6     4
 *                                          5
 *   1 td5  SCAN 2002-02-01 12:00     5     2
 *                                    2     5
 */
struct RDB_Query_test : public testing::TestWithParam<const char*> {
    RDB_Query_test()
            : xpr()
            , src1("WMO:02606,RAD:SE50,PLC:Ängelholm")
            , src2("WMO:02666,RAD:SE51,PLC:Karlskrona")
            , db(TestRDBEnv::get_database(GetParam()))
            , td1(oh5::File::minimal("PVOL", QDate(2000, 1, 1), QTime(12, 0), src1))
            , td2(oh5::File::minimal("PVOL", QDate(2000, 1, 1), QTime(12, 1), src2))
            , td3(oh5::File::minimal("PVOL", QDate(2000, 1, 1), QTime(12, 2), src1))
            , td4(oh5::File::minimal("CVOL", QDate(2001, 1, 1), QTime(12, 0), src2))
            , td5(oh5::File::minimal("SCAN", QDate(2002, 2, 1), QTime(12, 0), src1))
            , query(db) {
    }

    void add_attribute(oh5::File& file, const QString& path, const Variant& value) {
        QStringList names = path.split("/");
        shared_ptr<oh5::Attribute> attr = make_shared<oh5::Attribute>(names.takeLast(), value);
        QString attrgroup_name = names.takeLast();
        shared_ptr<oh5::Group> group = file.root();
        shared_ptr<oh5::Group> next_group;
        BOOST_FOREACH(const QString& name, names) {
            next_group = group->group_by_name(name);
            if (not next_group) {
                next_group = make_shared<oh5::Group>(name);
                group->add_child(next_group);
            }
            group = next_group;
        }
        if (not group->has_child_by_name(attrgroup_name)) {
            shared_ptr<oh5::AttributeGroup> attrgroup =
                make_shared<oh5::AttributeGroup>(attrgroup_name);
            group->add_child(attrgroup);
            group = attrgroup;
        } else {
            group = dynamic_pointer_cast<oh5::Group>(group->child_by_name(attrgroup_name));
        }

        group->add_child(attr);
    }

    virtual void SetUp() {
        add_attribute(*td1, "do1/where/xsize", Variant(1));
        add_attribute(*td1, "do1/where/ysize", Variant(2));
        td1->source(db->load_source(src1));

        add_attribute(*td2, "do1/where/xsize", Variant(2));
        add_attribute(*td2, "do1/where/ysize", Variant(2));
        td2->source(db->load_source(src2));

        add_attribute(*td3, "do1/where/xsize", Variant(3));
        add_attribute(*td3, "do2/where/xsize", Variant(3));
        td3->source(db->load_source(src1));

        add_attribute(*td4, "do1/where/xsize", Variant(6));
        add_attribute(*td4, "do1/where/ysize", Variant(4));
        add_attribute(*td4, "do2/where/ysize", Variant(5));
        td4->source(db->load_source(src2));

        add_attribute(*td5, "do1/where/xsize", Variant(5));
        add_attribute(*td5, "do1/where/ysize", Variant(2));
        add_attribute(*td5, "do2/where/xsize", Variant(2));
        add_attribute(*td5, "do2/where/ysize", Variant(5));
        td5->source(db->load_source(src1));

        db->save_file("td1", *td1);
        db->save_file("td2", *td2);
        db->save_file("td3", *td3);
        db->save_file("td4", *td4);
        db->save_file("td5", *td5);
    }

    virtual void TearDown() {
        db->clean();
    }
    
    expr::Factory xpr;
    QString src1, src2;
    test::TestRDB* db;
    shared_ptr<oh5::File> td1, td2, td3, td4, td5;
    Query query;
};

TEST_P(RDB_Query_test, test_simple) {
    shared_ptr<ResultSet> r = 
            query.fetch(xpr.attribute("path"))
                 .filter(xpr.attribute("where/xsize")->eq(xpr.integer(1)))
                 .execute();
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->string(0), "td1");
    ASSERT_TRUE(not r->next());
}

QStringList
extract_strings_at(ResultSet& r, int pos) {
    QStringList strings;
    while (r.next()) {
        strings.append(r.string(pos));
    }
    return strings;
}

TEST_P(RDB_Query_test, test_list_all_files) {
    shared_ptr<ResultSet> r = query.fetch(xpr.attribute("path")).execute();

    EXPECT_EQ(r->size(), 5);
    const QStringList& v = extract_strings_at(*r, 0);

    EXPECT_TRUE(v.contains("td1"));
    EXPECT_TRUE(v.contains("td2"));
    EXPECT_TRUE(v.contains("td3"));
    EXPECT_TRUE(v.contains("td4"));
    EXPECT_TRUE(v.contains("td5"));
}

TEST_P(RDB_Query_test, test_filter_by_object) {
    shared_ptr<ResultSet> r =
        query.fetch(xpr.attribute("path"))
             .filter(xpr.attribute("what/object")->eq(xpr.string("PVOL")))
             .execute();

    EXPECT_EQ(r->size(), 3);
    const QStringList& v = extract_strings_at(*r, 0);

    EXPECT_TRUE(v.contains("td1"));
    EXPECT_TRUE(v.contains("td2"));
    EXPECT_TRUE(v.contains("td3"));
}

TEST_P(RDB_Query_test, test_fetch_xsize_filtering_by_xsize) {
    shared_ptr<ResultSet> r =
        query.fetch(xpr.attribute("where/xsize"))
             .filter(xpr.attribute("where/xsize")->eq(xpr.integer(2)))
             .execute();
    EXPECT_EQ(r->size(), 2);
}

TEST_P(RDB_Query_test, test_filter_by_xsize_or_ysize) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    expr::AttributePtr ysize = xpr.attribute("where/ysize");
    shared_ptr<ResultSet> r =
        query.fetch(xpr.attribute("path"))
             .filter(xsize->eq(xpr.integer(1))->or_(ysize->eq(xpr.integer(2))))
             .execute();

    EXPECT_EQ(r->size(), 3);
    const QStringList& v = extract_strings_at(*r, 0);

    EXPECT_TRUE(v.contains("td1"));
    EXPECT_TRUE(v.contains("td2"));
    EXPECT_TRUE(v.contains("td5"));
}

TEST_P(RDB_Query_test, test_filter_by_xsize_distinct) {
    expr::AttributePtr xsize = xpr.attribute("where/xsize");
    shared_ptr<ResultSet> r = 
        query.fetch(xpr.attribute("path"))
             .distinct(true)
             .filter(xsize->eq(xpr.integer(3)))
             .execute();
    EXPECT_EQ(r->size(), 1);
    ASSERT_TRUE(r->next());
    EXPECT_EQ(r->string(0), "td3");
}

TEST_P(RDB_Query_test, test_select_by_wmo_code) {
    expr::AttributePtr wmo_code = xpr.attribute("src_WMO");
    shared_ptr<ResultSet> r =
        query.fetch(xpr.attribute("path"))
             .filter(wmo_code->eq(xpr.integer(2666)))
             .execute();

    EXPECT_EQ(r->size(), 2);
    const QStringList& v = extract_strings_at(*r, 0);

    EXPECT_TRUE(v.contains("td2"));
    EXPECT_TRUE(v.contains("td4"));
}

TEST_P(RDB_Query_test, test_select_by_place) {
    shared_ptr<ResultSet> r =
        query.fetch(xpr.attribute("path"))
             .filter(xpr.attribute("src_PLC")->eq(xpr.string("Ängelholm")))
             .execute();

    EXPECT_EQ(r->size(), 3);
    const QStringList& v = extract_strings_at(*r, 0);

    EXPECT_TRUE(v.contains("td1"));
    EXPECT_TRUE(v.contains("td3"));
    EXPECT_TRUE(v.contains("td5"));
}

TEST_P(RDB_Query_test, test_has_file) {
    bool result = false;
    ASSERT_NO_THROW(result = db->has_file(*td1));
    EXPECT_TRUE(result);
}

TEST_P(RDB_Query_test, test_has_nx_file) {
    bool result = false;
    shared_ptr<oh5::File> td = oh5::File::minimal("PVOL", QDate(2000, 1, 10), QTime(12, 0), src1);
    td->source(db->load_source(src1));
    ASSERT_NO_THROW(result = db->has_file(*td));
    EXPECT_FALSE(result);
}

#if BRFC_TEST_DSN_COUNT >= 1
INSTANTIATE_TEST_CASE_P(RDB_Query_test_p,
                        RDB_Query_test,
                        ::testing::ValuesIn(test_dsns));
#endif // BRFC_TEST_DSN_COUNT

} // namespace brfc

