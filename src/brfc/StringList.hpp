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
#include <string>

#include <brfc/assert.hpp>

namespace brfc {

class StringList {
  public:
    enum SplitPolicy {
        KEEP_EMPTY_PARTS,
        SKIP_EMPTY_PARTS
    };

    typedef std::list<std::string> container_t;

    typedef container_t::iterator iterator;
    typedef container_t::const_iterator const_iterator;

    StringList()
            : list_() {

    }

    explicit StringList(const std::string& str)
            : list_() {
        list_.push_back(str);
    }

    explicit StringList(const container_t& container)
            : list_(container) {

    }

    StringList(container_t::const_iterator begin,
               container_t::const_iterator end)
            : list_(begin, end) {
    }

    StringList(const StringList& other)
            : list_(other.list_) {

    }

    static StringList split(const std::string& str,
                            const std::string& sep,
                            SplitPolicy policy=KEEP_EMPTY_PARTS);
    
    std::string join(const std::string& sep) const;

    bool empty() const {
        return list_.empty();
    }

    size_t size() const {
        return list_.size();
    }

    bool contains(const std::string& value) const;

    void append(const std::string& value) {
        list_.push_back(value);
    }

    void sort() {
        list_.sort();
    }

    std::string& front() {
        return list_.front();
    }

    const std::string& front() const {
        return list_.front();
    }

    std::string& back() {
        return list_.back();
    }

    const std::string& back() const {
        return list_.back();
    }

    void push_front(const std::string& str) {
        list_.push_front(str);
    }

    void push_back(const std::string& str) {
        list_.push_back(str);
    }

    void clear() {
        return list_.clear();
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

    const std::string& at(int i) const;

    std::string take_first() {
        BRFC_ASSERT(not list_.empty());
        std::string s = list_.front();
        list_.pop_front();
        return s;
    }

    std::string take_last() {
        BRFC_ASSERT(not list_.empty());
        std::string s = list_.back();
        list_.pop_back();
        return s;
    }


  private:
    container_t list_;
};

} // namespace brfc

#endif // BRFC_STRING_LIST_HPP
