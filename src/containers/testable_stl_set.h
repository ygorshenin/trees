#ifndef CONTAINERS_TESTABLE_STL_SET_H_
#define CONTAINERS_TESTABLE_STL_SET_H_

#include <cstddef>

#include <set>

#include "containers/testable.h"


namespace containers {

template<class T>
class TestableStlSet : public TestableInterface<T, TestableStlSet<T> > {
  public:
    void Assign(size_t n, const T *data) {
      set_.erase(set_.begin(), set_.end());
      set_.insert(data, data + n);
    }

    bool Contains(const T &key) {
      return set_.find(key) != set_.end();
    }

  private:
    std::set<T> set_;
}; // class TestableStlSet

}  // namespace containers

#endif // CONTAINERS_TESTABLE_STL_SET_H_
