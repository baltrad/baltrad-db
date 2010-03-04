#ifndef BRFC_SMART_PTR_HPP
#define BRFC_SMART_PTR_HPP

#include <memory>

#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace brfc {

using std::auto_ptr;

using boost::scoped_ptr;
using boost::shared_ptr;
using boost::weak_ptr;

using boost::enable_shared_from_this;
using boost::make_shared;

using boost::const_pointer_cast;
using boost::dynamic_pointer_cast;
using boost::static_pointer_cast;

}

#endif // BRFC_SMART_PTR_HPP
