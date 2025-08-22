#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

// Minimal, header-only circular queue used by EspSoftwareSerial on ESP32.
// Replaces the old Boost-based circular_queue to remove external deps.

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <Arduino.h>

template <typename T>
class CircularQueue {
 public:
  explicit CircularQueue(size_t capacity)
  : _cap(capacity ? capacity : 1),
    _buf((T*)malloc(sizeof(T) * _cap)),
    _head(0), _tail(0), _count(0) {}

  ~CircularQueue() {
    if (_buf) free(_buf);
  }

  inline bool push(const T& v) {
    if (full()) return false;
    _buf[_head] = v;
    _head = (_head + 1) % _cap;
    _count++;
    return true;
  }

  inline bool pop(T& out) {
    if (empty()) return false;
    out = _buf[_tail];
    _tail = (_tail + 1) % _cap;
    _count--;
    return true;
  }

  inline bool empty() const { return _count == 0; }
  inline bool full()  const { return _count == _cap; }
  inline void clear() { _head = _tail = _count = 0; }
  inline size_t size() const { return _count; }
  inline size_t capacity() const { return _cap; }

 private:
  size_t _cap;
  T* _buf;
  size_t _head, _tail, _count;
};

#endif // CIRCULAR_QUEUE_H