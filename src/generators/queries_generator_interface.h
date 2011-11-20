#ifndef GENERATORS_QUERIES_GENERATOR_INTERFACE_H_
#define GENERATORS_QUERIES_GENERATOR_INTERFACE_H_

#include "boost/utility.hpp"


namespace generators {

template<typename T>
class QueriesGeneratorInterface : boost::noncopyable {
 public:
  QueriesGeneratorInterface() {}

  virtual ~QueriesGeneratorInterface() {}

  virtual void Generate(T* object) = 0;
}; // class QueriesGeneratorInterace

}  // namespace generators

#endif // GENERATORS_QUERIES_GENERATOR_INTERFACE_H_
