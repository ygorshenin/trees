#ifndef CONTAINERS_TESTABLE_BUCKET_BINARY_SEARCH_H_
#define CONTAINERS_TESTABLE_BUCKET_BINARY_SEARCH_H_

#include <algorithm>
#include <new>

#include "base/panic.h"
#include "boost/scoped_array.hpp"
#include "containers/testable.h"


extern const int kExitMemoryFailure;


namespace containers {

template<typename T>
class TestableBucketBinarySearch :
  public TestableInterface<T, TestableBucketBinarySearch<T> > {
  public:
    void Assign(size_t n, const T* data) {
      num_objects_ = n;
      data_.reset(new (std::nothrow) T [num_objects_]);
      if (data_.get() == NULL)
      	panic("TestableBinarySearch: can't allocate memory, "
      	      "terminating...", kExitMemoryFailure);
      std::copy(data, data + num_objects_, data_.get());

      std::sort(data_.get(), data_.get() + num_objects_);
      num_objects_ = std::unique(data_.get(), data_.get() + num_objects_) -
      	data_.get();

      bucket_size_ = std::max(1,
        static_cast<int>(sqrt(static_cast<double>(num_objects_))));
      bucket_size_ = std::min(bucket_size_, num_objects_);

      if (bucket_size_ != 0) {
      	num_buckets_ = (num_objects_ + bucket_size_ - 1) / bucket_size_;
      	buckets_.reset(new (std::nothrow) T [num_buckets_]);
      	for (size_t i = 0; i < num_buckets_; ++i)
      	  buckets_[i] = data_[i * bucket_size_];
      } else
      	num_buckets_ = 0;
    }

    bool Contains(const T &key) {
      size_t bucket_index = std::upper_bound(buckets_.get(),
      					     buckets_.get() + num_buckets_,
      					     key) - buckets_.get();
      if (bucket_index == 0)
	return false;
      size_t from = (bucket_index - 1) * bucket_size_;
      size_t to = std::min(from + bucket_size_, num_objects_);
      return std::binary_search(data_.get() + from, data_.get() + to, key);
    }

  private:
    size_t num_objects_;
    boost::scoped_array<T> data_;
    size_t num_buckets_, bucket_size_;
    boost::scoped_array<T> buckets_;
}; // class TestableBucketBinarySearch

}  // namespace containers

#endif // CONTAINERS_TESTABLE_BUCKET_BINARY_SEARCH_H_
