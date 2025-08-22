#pragma once
#include <stddef.h>
#include <stdint.h>
#include <Arduino.h>
template<typename T, size_t N>
class circular_queue {
public:
  circular_queue() : _head(0), _tail(0), _count(0) {}
  inline void reset(){ _head=_tail=_count=0; }
  inline bool push(const T& v){ if(full()) return false; _buf[_head]=v; _head=(_head+1)%N; _count++; return true; }
  inline bool pop(T& out){ if(empty()) return false; out=_buf[_tail]; _tail=(_tail+1)%N; _count--; return true; }
  inline bool empty() const { return _count==0; }
  inline bool full() const { return _count==N; }
  inline size_t size() const { return _count; }
  inline size_t capacity() const { return N; }
  inline size_t free_space() const { return N-_count; }
  inline const T& operator[](size_t i) const { size_t idx=(_tail+i)%N; return _buf[idx]; }
private:
  T _buf[N]; size_t _head,_tail,_count;
};
using cq8_256  = circular_queue<uint8_t, 256>;
using cq8_512  = circular_queue<uint8_t, 512>;
using cq8_1024 = circular_queue<uint8_t, 1024>;
