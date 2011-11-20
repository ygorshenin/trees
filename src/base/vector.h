#ifndef BASE_VECTOR_H_
#define BASE_VECTOR_H_


namespace base {

template<size_t N, typename T>
class Vector;

template<typename T>
class MetaComparer {
 public:
  static int Compare(T &lhs, T &rhs) {
    if (lhs < rhs)
      return -1;
    else if (lhs == rhs)
      return 0;
    else
      return +1;
  }
}; // class MetaComparer

template<>
class MetaComparer<double> {
 public:
  static const double kEpsilon;

  static int Compare(double lhs, double rhs) {
    if (lhs + kEpsilon < rhs)
      return -1;
    else if (rhs + kEpsilon < lhs)
      return +1;
    else
      return 0;
  }
 }; // class MetaComparer

const double MetaComparer<double>::kEpsilon = 1e-6;

template<size_t N, size_t I, typename T>
class MetaVectorComparer {
 public:
  static bool Compare(const Vector<N, T> &lhs, const Vector<N, T> &rhs) {
    return
      MetaComparer<T>::Compare(lhs[I], rhs[I]) < 0 ||
      MetaComparer<T>::Compare(lhs[I], rhs[I]) == 0 &&
      MetaVectorComparer<N, I + 1, T>::Compare(lhs, rhs);
  }
}; // class MetaVectorComparer

template<size_t N, typename T>
class MetaVectorComparer<N, N, T> {
 public:
  static bool Compare(const Vector<N, T> &lhs, const Vector<N, T> &rhs) {
    return false;
  }
}; // class MetaVectorComparer

template<size_t N, size_t I, typename T>
class MetaVectorEquals {
 public:
  static bool Equals(const Vector<N, T> &lhs, const Vector<N, T> &rhs) {
    return
      MetaComparer<T>::Compare(lhs[I], rhs[I]) == 0 &&
      MetaVectorEquals<N, I + 1, T>::Equals(lhs, rhs);
  }
}; // class MetaVectorEquals

template<size_t N, typename T>
class MetaVectorEquals<N, N, T> {
 public:
  static bool Equals(const Vector<N, T> &lhs, const Vector<N, T> &rhs) {
    return true;
  }
}; // class MetaVectorEquals

template<size_t N, typename T>
class Vector {
 public:
  T& operator [] (size_t index) {
    return buffer_[index];
  }

  const T& operator [] (size_t index) const {
    return buffer_[index];
  }

  bool operator < (const Vector<N, T> &other) const {
    return MetaVectorComparer<N, 0, T>::Compare(*this, other);
  }

  bool operator == (const Vector<N, T> &other) const {
    return MetaVectorEquals<N, 0, T>::Equals(*this, other);
  }

 private:
  T buffer_[N];
}; // class Vector

}  // namespace base

#endif // BASE_VECTOR_H_
