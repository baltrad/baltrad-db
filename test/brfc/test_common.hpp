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

#ifndef BRFC_TEST_COMMON_HPP
#define BRFC_TEST_COMMON_HPP

#include <iosfwd>

#include <gtest/gtest.h>

namespace brfc {

class Date;
class DateTime;
class Oh5Scalar;
class Time;
class TimeDelta;
class Variant;

} // namespace brfc

std::ostream&
operator<<(std::ostream& out, const brfc::Date& value);

std::ostream&
operator<<(std::ostream& out, const brfc::Time& value);

std::ostream&
operator<<(std::ostream& out, const brfc::DateTime& value);

std::ostream&
operator<<(std::ostream& out, const brfc::TimeDelta& value);

std::ostream&
operator<<(std::ostream& out, const brfc::Variant& value);

std::ostream&
operator<<(std::ostream& out, const brfc::Oh5Scalar& value);

#endif // BRFC_TEST_COMMON_HPP
