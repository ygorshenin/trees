#ifndef CONTAINERS_TESTABLE_BINARY_SEARCH_H_
#define CONTAINERS_TESTABLE_BINARY_SEARCH_H_

#include <algorithm>
#include <new>

#include "base/panic.h"
#include "boost/scoped_array.hpp"
#include "containers/testable.h"


extern const int kExitMemoryFailure;


namespace containers {

template<typename T>
class TestableBinarySearch :
  public TestableInterface<T, TestableBinarySearch<T> > {
  public:
    TestableBinarySearch(): data_(NULL) {
    }

    void Assign(size_t n, const T* data) {
      n_ = n;
      data_.reset(new (std::nothrow) T [n_]);
      if (data_.get() == NULL)
	panic("TestableBinarySearch: can't allocate memory, "
	      "terminating...", kExitMemoryFailure);
      std::copy(data, data + n_, data_.get());
      std::sort(data_.get(), data_.get() + n_);
    }

    bool Contains(const T &key) {
      return std::binary_search(data_.get(), data_.get() + n_, key);
    }

  private:
    size_t n_;
    boost::scoped_array<T> data_;
}; // class TestableBinarySearch

}  // namespace containers

#endif // CONTAINERS_TESTABLE_BINARY_SEARCH_H_
