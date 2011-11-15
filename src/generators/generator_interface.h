#ifndef GENERATORS_GENERATOR_INTERFACE_H_
#define GENERATORS_GENERATOR_INTERFACE_H_

#include "boost/utility.hpp"


namespace generators {

template<typename T>
class GeneratorInterace: boost::noncopyable {
 public:
  GeneratorInterace() {}

  virtual ~GeneratorInterace() {}

  virtual void Generate(T *object) = 0;
}; // class GeneratorInterace

}  // namespace generators

#endif // GENERATORS_GENERATOR_INTERFACE_H_
