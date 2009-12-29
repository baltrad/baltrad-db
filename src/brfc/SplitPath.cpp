#include <brfc/SplitPath.hpp>

#include <brfc/assert.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>

#include <vector>

namespace brfc {

SplitPath::SplitPath(const std::string& path)
        : attribute_name()
        , data_object_path() {
    split(path);
}

void
SplitPath::split(const std::string& path) {
    std::vector<std::string> path_vec;
    boost::split(path_vec, path, boost::is_any_of("/"));

    BRFC_ASSERT(path_vec.size() >= 2);

    // last element is always attribute name
    attribute_name = path_vec.back(); path_vec.pop_back();

    // test if next to last element is a valid grouping
    const std::string& group = path_vec.back();
    if (group == "what" || group == "where" || group == "how") {
    	attribute_name = group + "/" + attribute_name;
        path_vec.pop_back();
    }

    // rest of the element is data object path
    if (path_vec.size() <= 1) {
        data_object_path = "/";
    } else {
        data_object_path = boost::join(path_vec, "/");
    }
}


}
