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

#ifndef BRFC_VISIT_HPP
#define BRFC_VISIT_HPP

#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/utility.hpp>

#include <brfc/exceptions.hpp>

namespace mpl = ::boost::mpl;

namespace brfc {

namespace detail {

    template <class Last, class T, class Visitor>
    bool visit(Last, Last, T*, Visitor&) {
        return false;
    }

    template <class First, class Last, class T, class Visitor>
    bool visit(First, Last, T* object, Visitor& v) {
        typedef typename mpl::deref<First>::type type;
        type* typed_object = dynamic_cast<type*>(object);

        if (typed_object) {
            v(*typed_object);
            return true;
        } else {
            return detail::visit(typename mpl::next<First>::type(),
                                 Last(),
                                 object,
                                 v);
        }
    }

} // namespace detail

/**
 * @brief generic visitor
 *
 * visitor must define a boost::mpl::sequence accepted_types which
 * is a sequence of all types the visitor is able to handle.
 *
 * a handler for a type is operator()(T&).
 *
 * this implementation sacrifices efficiency in order to be
 * non-intrusive. Compared to a typical visitor pattern of complexity O(1),
 * this algorithm has a complexity of O(n), where n is the number of types
 * specified in Visitor::accepted_types.
 *
 * for each type in visitor::accepted_types, the object is dynamically cast
 * to the type and if successful, handled by that type. This means types
 * further down the inheritance hierarchy should be specified first in
 * accepted_types.
 *
 * @return true if the visitor handled the object
 */
template<class T, class Visitor>
bool visit(T& object, Visitor& with_visitor) {
    typedef typename Visitor::accepted_types seq;

    return detail::visit(typename mpl::begin<seq>::type(),
                         typename mpl::end<seq>::type(),
                         boost::addressof(object),
                         with_visitor);
}

} // namespace brfc

#endif // BRFC_VISIT_HPP
