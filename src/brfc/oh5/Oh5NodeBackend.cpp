/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

This file is part of baltrad-db.

baltrad-db is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

baltrad-db is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.
*/

#include <brfc/oh5/Oh5NodeBackend.hpp>

#include <memory>
#include <list>

#include <boost/foreach.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/oh5/Oh5Node.hpp>

namespace brfc {

Oh5NodeBackend::~Oh5NodeBackend() {

}

Oh5Node&
Oh5NodeBackend::add(const Oh5Node& parent, Oh5Node* node) {
    if (not node)
        throw value_error("node == 0");
    if (node->has_backend())
        throw value_error("node already attached to backend");
    std::auto_ptr<Oh5Node> nodep(node); // take ownership
    if (&parent.backend() != this)
        throw value_error("parent not attached to this backend");
    node->parent(const_cast<Oh5Node*>(&parent));
    Oh5Node& rnode = do_add(nodep.release());
    rnode.backend(this);
    return rnode;
}

Oh5Node&
Oh5NodeBackend::add(const std::string& path, Oh5Node* n) {
    Oh5Node* parent = node(path);
    if (not parent)
        throw lookup_error("No node by path: " + path);
    return add(*parent, n);
}

std::vector<Oh5Node*>
Oh5NodeBackend::children(const Oh5Node& node) {
    std::vector<const Oh5Node*> cnodes = const_this().do_children(node);
    std::vector<Oh5Node*> nodes;
    nodes.reserve(cnodes.size());
    std::vector<const Oh5Node*>::iterator iter = cnodes.begin();
    for ( ; iter != cnodes.end(); ++iter) {
        nodes.push_back(const_cast<Oh5Node*>(*iter));
    }
    return nodes;
}


const Oh5Node*
Oh5NodeBackend::child_by_name(const Oh5Node& node, const std::string& name) const {
    BOOST_FOREACH(const Oh5Node* n, children(node)) {
        if (n->name() == name)
            return n;
    }
    return 0;
}

const Oh5Node*
Oh5NodeBackend::child_by_path(const Oh5Node& node, const std::string& path) const {
    if (boost::starts_with(path, "/") and node.parent())
        throw value_error("path must not be absolute");
    std::list<std::string> names;
    boost::split(names, path, boost::is_any_of("/"),
                 boost::token_compress_on);
    const Oh5Node* cur_n = &node;
    const Oh5Node* child_n = &node;
    BOOST_FOREACH(const std::string& name, names) {
        if (name.empty())
            continue;
        child_n = child_by_name(*cur_n, name);
        if (not child_n)
            break;
        cur_n = child_n;
    }
    return child_n;
}

} // namespace brfc