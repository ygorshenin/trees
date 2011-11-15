#ifndef TYPELIST_TYPELIST_H_
#define TYPELIST_TYPELIST_H_

#include <cstddef>


namespace typelist {

struct NullType {
}; // struct NullType

template<class T, class U>
struct TypeList {
  typedef T Head;
  typedef U Tail;
}; // struct TypeList


template<class> struct Length;

template<>
struct Length<NullType> {
  static const size_t kLength = 0;
}; // struct Length

template<class T, class U>
struct Length< TypeList<T, U> > {
  static const size_t kLength = 1 + Length<U>::kLength;
}; // struct Length

}  // namespace typelist

#endif // TYPELIST_TYPELIST_H_
