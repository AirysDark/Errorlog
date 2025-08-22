#pragma once
#include <Arduino.h>

template <typename T, size_t N>
class circular_queue {
public:
  circular_queue() : head_(0), tail_(0), count_(0) {}

  inline bool push(const T& v) {
    if (full()) return false;
    buf_[head_] = v;
    head_ = (head_ + 1) % N;
    count_++;
    return true;
  }

  inline bool pop(T& out) {
    if (empty()) return false;
    out = buf_[tail_];
    tail_ = (tail_ + 1) % N;
    count_--;
    return true;
  }

  inline bool peek(T& out) const {
    if (empty()) return false;
    out = buf_[tail_];
    return true;
  }

  inline void clear() { head_ = tail_ = count_ = 0; }
  inline bool empty() const { return count_ == 0; }
  inline bool full()  const { return count_ == N; }
  inline size_t size() const { return count_; }
  inline size_t capacity() const { return N; }

private:
  T buf_[N];
  size_t head_, tail_, count_;
};