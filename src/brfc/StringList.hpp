/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

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

#ifndef BRFC_STRING_LIST_HPP
#define BRFC_STRING_LIST_HPP

#include <list>

#include <brfc/assert.hpp>
#include <brfc/String.hpp>

namespace brfc {

class StringList {
  public:
    typedef std::list<String> container_t;

    typedef container_t::iterator iterator;
    typedef container_t::const_iterator const_iterator;

    StringList()
            : list_() {

    }

    explicit StringList(const String& str)
            : list_() {
        list_.push_back(str);
    }

    StringList(const StringList& other)
            : list_(other.list_) {

    }
    
    String join(const String& sep) const;

    bool empty() const {
        return list_.empty();
    }

    size_t size() const {
        return list_.size();
    }

    bool contains(const String& value) const;

    void append(const String& value) {
        list_.push_back(value);
    }

    void sort() {
        list_.sort();
    }

    String& front() {
        return list_.front();
    }

    const String& front() const {
        return list_.front();
    }

    void push_front(const String& str) {
        list_.push_front(str);
    }

    void push_back(const String& str) {
        list_.push_back(str);
    }

    iterator begin() {
        return list_.begin();
    }

    const_iterator begin() const {
        return list_.begin();
    }

    iterator end() {
        return list_.end();
    }
    
    const_iterator end() const {
        return list_.end();
    }

    const String& at(int i) const;

    String take_first() {
        BRFC_ASSERT(not list_.empty());
        String s = list_.front();
        list_.pop_front();
        return s;
    }

    String take_last() {
        BRFC_ASSERT(not list_.empty());
        String s = list_.back();
        list_.pop_back();
        return s;
    }


  private:
    container_t list_;
};

} // namespace brfc

#endif // BRFC_STRING_LIST_HPP
