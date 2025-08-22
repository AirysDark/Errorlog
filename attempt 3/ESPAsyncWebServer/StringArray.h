/*
 * Patched StringArray.h for ESPAsyncWebServer
 * Goal: avoid global name clashes with Arduino's LinkedList.
 * - Provides AsyncLinkedList<T> implementation.
 * - If Arduino LinkedList is already included (LinkedList_h defined), alias AsyncLinkedList to it.
 * - Otherwise, define AsyncLinkedList and (for ESPAsyncWebServer internal use) map LinkedList -> AsyncLinkedList.
 */

#pragma once
#include <stddef.h>
#include <stdint.h>
#include <functional>

// If the Arduino LinkedList library is already included, reuse it via alias
#if defined(LinkedList_h)
  // Arduino LinkedList is present; piggyback types so ESPAsyncWebServer can use AsyncLinkedList seamlessly.
  template<typename T>
  using AsyncLinkedList = ::LinkedList<T>;
#else

// Minimal singly-linked list tailored for ESPAsyncWebServer usage
template<typename T>
class AsyncLinkedList {
  public:
    using OnRemove = std::function<void(const T&)>;

    struct Node {
      T value;
      Node* next;
      explicit Node(const T& v) : value(v), next(nullptr) {}
    };

    explicit AsyncLinkedList(OnRemove onRemove = OnRemove())
      : _head(nullptr), _tail(nullptr), _size(0), _onRemove(onRemove) {}

    ~AsyncLinkedList() { clear(); }

    // Non-copyable
    AsyncLinkedList(const AsyncLinkedList&) = delete;
    AsyncLinkedList& operator=(const AsyncLinkedList&) = delete;

    // Movable
    AsyncLinkedList(AsyncLinkedList&& other) noexcept
      : _head(other._head), _tail(other._tail), _size(other._size), _onRemove(std::move(other._onRemove)) {
      other._head = other._tail = nullptr;
      other._size = 0;
    }
    AsyncLinkedList& operator=(AsyncLinkedList&& other) noexcept {
      if (this != &other) {
        clear();
        _head = other._head;
        _tail = other._tail;
        _size = other._size;
        _onRemove = std::move(other._onRemove);
        other._head = other._tail = nullptr;
        other._size = 0;
      }
      return *this;
    }

    inline size_t size() const { return _size; }
    inline size_t length() const { return _size; }
    inline bool empty() const { return _size == 0; }

    // Append value
    inline void add(const T& v) {
      Node* n = new Node(v);
      if (!_tail) { _head = _tail = n; }
      else { _tail->next = n; _tail = n; }
      ++_size;
    }

    // Remove first occurrence by value (requires operator== on T)
    inline bool remove(const T& v) {
      Node* prev = nullptr;
      Node* cur = _head;
      while (cur) {
        if (cur->value == v) {
          if (_onRemove) _onRemove(cur->value);
          if (prev) prev->next = cur->next; else _head = cur->next;
          if (cur == _tail) _tail = prev;
          delete cur;
          --_size;
          return true;
        }
        prev = cur;
        cur = cur->next;
      }
      return false;
    }

    // Clear list
    inline void clear() {
      Node* cur = _head;
      while (cur) {
        Node* nxt = cur->next;
        if (_onRemove) _onRemove(cur->value);
        delete cur;
        cur = nxt;
      }
      _head = _tail = nullptr;
      _size = 0;
    }

    // Random access (O(n)) helpers to match ESPAsyncWebServer usage
    inline T& operator[](size_t index) { return getRef(index); }
    inline const T& operator[](size_t index) const { return getRef(index); }

    inline T& get(size_t index) { return getRef(index); }
    inline const T& get(size_t index) const { return getRef(index); }

  private:
    inline T& getRef(size_t index) const {
      Node* cur = _head;
      while (cur && index--) cur = cur->next;
      // caller responsibility not to go out of bounds
      return cur->value;
    }

    Node* _head;
    Node* _tail;
    size_t _size;
    OnRemove _onRemove;
};

// If Arduino LinkedList is NOT present, ensure ESPAsyncWebServer's own code that references `LinkedList` still compiles
#ifndef ESPASYNCSERVER_NO_LINKEDLIST_ALIAS
  #define LinkedList AsyncLinkedList
#endif

#endif // !defined(LinkedList_h)
