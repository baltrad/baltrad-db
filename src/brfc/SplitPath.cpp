#include <brfc/SplitPath.hpp>

#include <brfc/assert.hpp>

#include <QtCore/QStringList>

namespace brfc {

SplitPath::SplitPath(const QString& path)
        : attribute_name()
        , data_object_path() {
    split(path);
}

void
SplitPath::split(const QString& path) {
    QString sep = QString::fromUtf8("/");
    QStringList elems = path.split(sep);

    BRFC_ASSERT(elems.size() >= 2);

    // last element is always attribute name
    attribute_name = elems.takeLast();

    // test if next to last element is a valid grouping
    QStringList valid_groups;
    valid_groups.append(QString::fromUtf8("what"));
    valid_groups.append(QString::fromUtf8("where"));
    valid_groups.append(QString::fromUtf8("how"));

    if (valid_groups.contains(elems.last())) {
        attribute_name = elems.takeLast() + sep + attribute_name;
    }

    // rest of the element is data object path
    if (elems.size() <= 1) {
        data_object_path = sep;
    } else {
        data_object_path = elems.join(sep);
    }
}


}
