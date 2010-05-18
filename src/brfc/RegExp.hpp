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

#include <brfc/String.hpp>

#include <unicode/regex.h>

namespace brfc {

class RegExp {
  public:
    explicit RegExp(const String& pattern);
    
    /**
     * @return position of the last match or -1 if no match
     */
    int index_in(const String& str, int pos=0);    

    /**
     * @return length of the last match or -1 if no match
     */
    int matched_length() const;    

    /**
     * @return text of the entire match
     */
    String cap() const;    

    /**
     * @return position of the last match or -1 if no match
     */
    int pos() const;

  private:
    mutable UErrorCode uerr_;
    bool matches_;
    UnicodeString input_;
    RegexMatcher re_;
};

} // namespace brfc
