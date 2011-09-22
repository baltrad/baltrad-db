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
#ifndef BRFC_FIXED_SIZE_SET_HPP
#define BRFC_FIXED_SIZE_SET_HPP

#include <cmath>
#include <list>
#include <stdexcept>

#include <boost/function.hpp>

namespace brfc {

/**
 * @brief a set with a fixed size, dropping oldest values
 */
template<typename T>
class FixedSizeSet {
  public:
    typedef boost::function<void(T&)> RemovalCallback;
    
    /**
     * @brief default removal callback, no-op
     */
    class default_remove_callback {
      public:
        void operator()(T&) { }
    };
    
    /**
     * @brief constructor
     * @param max_size maximum size
     * @throw std::invalid_argument for invalid @c max_size
     */
    explicit FixedSizeSet(size_t max_size)
            : max_size_(max_size)
            , overflow_(std::ceil(max_size_ * 0.1f))
            , removal_callback_(default_remove_callback())
            , values_() {
        if (max_size == 0)
            throw std::invalid_argument("max_size must be > 0");
    }
    
    /**
     * @brief current size
     */
    size_t size() const { return values_.size(); }
    
    /**
     * @brief maximum size
     */
    size_t max_size() const { return max_size_; }
    
    /**
     * @brief set maximum size
     * @throw std::invalid_argument for invalid @c value
     *
     * if current overflow is greater than the new maximum size,
     * overflow is set to be equal to the maximum size
     */
    void max_size(size_t value) {
        if (value == 0)
            throw std::invalid_argument("max_size must be > 0");

        max_size_ = value;
        if (overflow_ > max_size_)
            overflow(max_size_);
    }
    
    /**
     * @brief overflow
     */
    size_t overflow() const { return overflow_; }
    
    /**
     * @brief set overflow
     * @throw std::invalid_argument for invalid @c value
     */
    void overflow(size_t value) {
        if (value == 0 or value > max_size_)
            throw std::invalid_argument("overflow must be > 0 and <= max_size");

        overflow_ = value;
    }
    
    /**
     * @brief set function to call when a value is removed
     */
    void removal_callback(RemovalCallback callback) {
        removal_callback_ = callback;
    }
    
    /**
     * @brief add a value
     *
     * if current size exceeds max_size, remove_overflow() is called
     */
    void add(const T& value) {
        typename std::list<T>::iterator it =
            std::find(values_.begin(), values_.end(), value);

        if (it != values_.end()) {
            values_.erase(it);
        }

        if (values_.size() >= max_size_)
            remove_overflow();

        values_.push_front(value);
    }
    
    /**
     * @brief test if @c value is contained in this set
     */
    bool contains(const T& value) const {
        return std::find(values_.begin(), values_.end(), value) != values_.end();
    }
    
    /**
     * @brief remove @c value from this set
     *
     * call removal callback for the value
     */
    void remove(const T& value) {
        typename std::list<T>::iterator it =
            std::find(values_.begin(), values_.end(), value);
        if (it != values_.end()) {
            removal_callback_(*it);
            values_.erase(it);
        }
    }
    
    /**
     * @brief remove oldest items from the set
     *
     * remove items so that size < max_size - overflow
     */
    void remove_overflow() {
        int remove_count = overflow_ - (max_size_ - values_.size());
        if (remove_count <= 0)
            return;
        
        typename std::list<T>::iterator it = values_.end();
        std::advance(it, -remove_count);

        typename std::list<T>::iterator it2(it);
        for ( ; it2 != values_.end(); ++it2) {
            removal_callback_(*it2);
        }

        values_.erase(it, values_.end());
    }

  private:
    size_t max_size_;
    size_t overflow_;
    RemovalCallback removal_callback_;
    std::list<T> values_;
};

} // namespace brfc

#endif // BRFC_UTIL_FIXED_SIZE_SET_HPP
