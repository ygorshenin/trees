#ifndef GENERATORS_RANDOM_GENERATOR_H_
#define GENERATORS_RANDOM_GENERATOR_H_

#include <algorithm>
#include <cstddef>
#include <cstdlib>

#include "base/vector.h"
#include "generators/data_generator_interface.h"


namespace generators {

template<typename T>
class DataRandomGenerator : public DataGeneratorInterface<T> {
 public:
  virtual void Generate(T* object);
}; // class DataRandomGenerator

template<size_t N, typename T>
class DataRandomGenerator<base::Vector<N, T> > :
  public DataGeneratorInterface<base::Vector<N, T> > {
 public:
  virtual void Generate(base::Vector<N, T>* v) {
    for (size_t i = 0; i < N; ++i)
      (*v)[i] = rand();
  }
}; // class DataRandomGenerator

}  // namespace generators

#endif // GENERATORS_DATA_RANDOM_GENERATOR_H_
