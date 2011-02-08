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

#ifndef BRFC_UTIL_QUEUE_HPP
#define BRFC_UTIL_QUEUE_HPP

#include <queue>
#include <stdexcept>

#include <boost/thread/condition.hpp>

namespace brfc {

class queue_empty : public std::runtime_error {
  public:
    queue_empty()
            : std::runtime_error("queue empty") {
    }
};

class queue_full : public std::runtime_error {
  public:
    queue_full()
            : std::runtime_error("queue full") {
    }
};

/**
 * @brief a synchronized queue
 */
template<typename T>
class Queue {
  public:
    /**
     * @brief 
     */

    /**
     * @param max maximum size of the queue (0 means no limit on size)
     */
    Queue(size_t max_size=0)
            : mutex_()
            , not_empty_()
            , not_full_()
            , max_size_(max_size)
            , queue_() {
    }
    
    /**
     * @brief get a value from the front of the queue
     * @param timeout time to wait (in msecs). value < 0 means wait forever
     * @throw queue_empty if timeout occurs and no value available
     * @return value from the front of the queue
     */
    T get(int timeout=-1) {
        boost::unique_lock<boost::mutex> lock(mutex_);
        
        if (timeout < 0) { // don't wait
            if (empty_unsynced())
                throw queue_empty();
        } else if (timeout == 0) { // wait forever
            while (empty_unsynced()) {
                not_empty_.wait(lock);
            }
        } else { // wait at most timeout msecs
            const boost::system_time abs_timeout =
                boost::get_system_time() + boost::posix_time::milliseconds(timeout);
            while (empty_unsynced()) {
                if (not not_empty_.timed_wait(lock, abs_timeout))
                    throw queue_empty(); // timed out
            }
        }

        // there is at least one value in the queue if we got here
        T value = queue_.front();
        queue_.pop();
        lock.unlock();
        not_full_.notify_one(); // one value can be added now
        return value;
    }
    
    /**
     * @brief put a value to the end of the queue
     * @param value the value to put
     * @param timeout time to wait (in msecs). value < 0 means wait forever
     * @throw queue_full if timeout occurs and queue is still full
     */
    void put(const T& value, int timeout=-1) {
        boost::unique_lock<boost::mutex> lock(mutex_);
        
        if (timeout < 0) { // don't wait
            if (full_unsynced())
                throw queue_full();
        } else if (timeout == 0) { // wait forever
            while (full_unsynced())
                not_full_.wait(lock);
        } else { // wait at most timeout msecs
            const boost::system_time abs_timeout =
                boost::get_system_time() + boost::posix_time::milliseconds(timeout);
            while (full_unsynced()) {
                if (not not_full_.timed_wait(lock, abs_timeout)) {
                    throw queue_full(); // timed out
                }
            }
        }

        // there is space for at least one value in the queue if we got here
        queue_.push(value);
        lock.unlock();
        not_empty_.notify_one(); // one value can be added now
    }

    size_t size() const {
        boost::unique_lock<boost::mutex> lock(mutex_);
        return queue_.size();
    }
    
    /**
     * @brief maximum size of this queue (0 means no limit on size)
     */
    size_t max_size() const {
        return max_size_;
    }

    /**
     * @brief test if queue is full
     * @return true if size() == max_size()
     */
    bool full() const {
        boost::unique_lock<boost::mutex> lock(mutex_);
        return full_unsynced();
    }
    
    /**
     * @brief test if queue is full
     * @return true if size() == 0
     */
    bool empty() const {
        boost::unique_lock<boost::mutex> lock(mutex_);
        return empty_unsynced();
    }

  private:
    bool full_unsynced() const {
        return max_size_ > 0 and queue_.size() == max_size_;
    }

    bool empty_unsynced() const {
        return queue_.empty();
    }
    
    mutable boost::mutex mutex_;
    boost::condition not_empty_;
    boost::condition not_full_;
    size_t max_size_;
    std::queue<T> queue_;
};

} // namespace brfc

#endif // BRFC_UTIL_QUEUE_HPP
