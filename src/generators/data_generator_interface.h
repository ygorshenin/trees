#ifndef GENERATORS_DATA_GENERATOR_INTERFACE_H_
#define GENERATORS_DATA_GENERATOR_INTERFACE_H_

#include "boost/utility.hpp"


namespace generators {

template<typename T>
class DataGeneratorInterface : boost::noncopyable {
 public:
  DataGeneratorInterface() {}

  virtual ~DataGeneratorInterface() {}

  virtual void Generate(T* object) = 0;
}; // class DataGeneratorInterface

}  // namespace generators

#endif // GENERATORS_DATA_GENERATOR_INTERFACE_H_
