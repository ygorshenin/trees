#ifndef TYPELIST_MACROS_H_
#define TYPELIST_MACROS_H_

#include "typelist/typelist.h"


#define TYPELIST_1(T1) \
  typelist::TypeList<T1, typelist::NullType>
#define TYPELIST_2(T1, T2) \
  typelist::TypeList<T1, TYPELIST_1(T2) >
#define TYPELIST_3(T1, T2, T3) \
  typelist::TypeList<T1, TYPELIST_2(T2, T3) >
#define TYPELIST_4(T1, T2, T3, T4) \
  typelist::TypeList<T1, TYPELIST_3(T2, T3, T4) >

#endif // TYPELIST_MACROS_H_
