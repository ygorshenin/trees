#ifndef BASE_TIMER_H_
#define BASE_TIMER_H_

#include "boost/chrono.hpp"
#include "boost/utility.hpp"


namespace base {

class Timer : public boost::noncopyable {
 public:
  Timer();

  void Restart();

  double Elapsed() const;

 private:
  boost::chrono::system_clock::time_point start_time_;
}; // class Timer

}  // namespace base

#endif // BASE_TIMER_H_
