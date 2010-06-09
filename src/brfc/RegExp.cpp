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

#include <brfc/RegExp.hpp>

#include <brfc/exceptions.hpp>

namespace brfc {

RegExp::RegExp(const String& pattern)
        : uerr_(U_ZERO_ERROR)
        , matches_(false)
        , input_()
        , re_(pattern.utf16(), 0, uerr_) {
    if (U_FAILURE(uerr_))
        throw value_error(u_errorName(uerr_));
}

int
RegExp::index_in(const String& str, int pos) {
    input_.setTo(str.utf16(), str.length()); 
    re_.reset(input_);
    matches_ = re_.find(pos, uerr_);
    if (U_FAILURE(uerr_))
        throw std::runtime_error(u_errorName(uerr_));
    return this->pos();
}

bool
RegExp::match(const String& str, int pos) {
    input_.setTo(str.utf16(), str.length());
    re_.reset(input_);
    matches_ = re_.matches(pos, uerr_);
    return matches_;    
}

int
RegExp::matched_length() const {
    if (not matches_)
        return -1;
    int start = re_.start(uerr_);
    if (U_FAILURE(uerr_))
        throw std::runtime_error(u_errorName(uerr_));
    int end = re_.end(uerr_);
    if (U_FAILURE(uerr_))
        throw std::runtime_error(u_errorName(uerr_));
    return end - start;
}

String
RegExp::cap() const {
    if (not matches_)
        return String();
    String str(re_.group(uerr_));
    if (U_FAILURE(uerr_))
        throw std::runtime_error(u_errorName(uerr_));
    return str;
}

int
RegExp::pos() const {
    if (not matches_)
        return -1;
    int start = re_.start(uerr_);
    if (U_FAILURE(uerr_))
        throw std::runtime_error(u_errorName(uerr_));
    return start;
}

String
RegExp::group(int n) const {
    String grp(re_.group(n, uerr_));
    if (U_FAILURE(uerr_))
        throw std::runtime_error(u_errorName(uerr_));
    return grp;
}

int
RegExp::group_count() const {
    return re_.groupCount();
}

} // namespace brfc
