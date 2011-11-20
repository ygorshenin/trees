#ifndef GENERATORS_QUERIES_RANDOM_GENERATOR_H_
#define GENERATORS_QUERIES_RANDOM_GENERATOR_H_

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <new>

#include "base/panic.h"
#include "base/vector.h"
#include "boost/scoped_array.hpp"
#include "generators/queries_generator_interface.h"


extern const int kExitMemoryFailure;


namespace generators {

template<class T>
class QueriesRandomGenerator : public QueriesGeneratorInterface<T> {
 public:
  QueriesRandomGenerator(size_t num_objects, const T* objects) :
    num_objects_(num_objects), objects_(new (std::nothrow) T [num_objects_]) {
    if (objects_.get() == NULL)
      panic("QueriesRandomGenerator: can't allocate memory, "
	    "terminating...", kExitMemoryFailure);
    std::copy(objects, objects + num_objects_, objects_.get());
  }

  virtual void Generate(T* object) {
    if (rand() % 2 == 0 || num_objects_ == 0)
      generator_.Generate(object);
    else
      *object = objects_[rand() % num_objects_];
  }

 private:
  size_t num_objects_;
  boost::scoped_array<T> objects_;
  DataRandomGenerator<T> generator_;
}; // class QueriesRandomGenerator

}  // namespace generators

#endif // GENERATORS_QUERIES_RANDOM_GENERATOR_H_
