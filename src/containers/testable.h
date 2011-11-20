#ifndef CONTAINERS_TESTABLE_H_
#define CONTAINERS_TESTABLE_H_

#include <cstddef>

#include "boost/utility.hpp"


namespace containers {

// CRTP idiom is used here, to avoid inline functions overhead.
template<typename T, typename Derived>
class TestableInterface : public boost::noncopyable {
  public:
    void Assign(size_t n, const T* data) {
      AsDerived()->Assign(n, data);
    }

    bool Contains(const T &key) {
      return AsDerived()->Contains(key);
    }

  private:
    Derived* AsDerived() {
      return static_cast<Derived*>(this);
    }
}; // class TestableInterface

}  // namespace containers

#endif // CONTAINERS_TESTABLE_H_
