/******************************************************************************/
/*                                                                            */
/*  Copyright 2026 NX Utils Foundation                                        */
/*                                                                            */
/*  Licensed under the Apache License, Version 2.0 (the "License");           */
/*  you may not use this file except in compliance with the License.          */
/*  You may obtain a copy of the License at                                   */
/*                                                                            */
/*      http://www.apache.org/licenses/LICENSE-2.0                            */
/*                                                                            */
/*  Unless required by applicable law or agreed to in writing, software       */
/*  distributed under the License is distributed on an "AS IS" BASIS,         */
/*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  */
/*  See the License for the specific language governing permissions and       */
/*  limitations under the License.                                            */
/*                                                                            */
/******************************************************************************/

#ifndef NX_BASE_LOCKFREE_RINGBUFFER_H__
#define NX_BASE_LOCKFREE_RINGBUFFER_H__

#include <array>
#include <atomic>
#include <cstddef>  // For size_t
#include <type_traits>

namespace nx {
namespace lockfree {

/// A lock-free single-producer single-consumer (SPSC) ring buffer.
///
/// This ring buffer provides lock-free operations for one producer thread
/// and one consumer thread. For other concurrency patterns, additional
/// synchronization is required.
template <typename T, size_t N>
class RingBuffer {
 public:
  static_assert(N > 0 && (N & (N - 1)) == 0,
                "RingBuffer capacity must be a power of two");

  RingBuffer() = default;

  ~RingBuffer() = default;

  RingBuffer(const RingBuffer &) = delete;
  RingBuffer &operator=(const RingBuffer &) = delete;
  RingBuffer(RingBuffer &&) = delete;
  RingBuffer &operator=(RingBuffer &&) = delete;

  /// Write an item into the ring buffer (producer only).
  ///
  /// Returns true if the item was successfully written, false if the buffer
  /// is full.
  template <typename U = T>
  bool Write(U &&value) {
    if (size_.load(std::memory_order::relaxed) == N) {
      return false;
    }
    data_[write_ptr_] = std::forward<U>(value);
    ++write_ptr_;
    write_ptr_ &= (N - 1);
    size_.fetch_add(1, std::memory_order::release);
    return true;
  }

  /// Read an item from the ring buffer (consumer only).
  ///
  /// Returns true if an item was successfully read, false if the buffer
  /// is empty.
  bool Read(T &value) {
    if (size_.load(std::memory_order::relaxed) == 0) {
      return false;
    }
    value = data_[read_ptr_];
    ++read_ptr_;
    read_ptr_ &= (N - 1);
    size_.fetch_sub(1, std::memory_order::release);
    return true;
  }

  /// Check if the ring buffer is empty (consumer side view).
  bool Empty() const { return size_.load(std::memory_order::relaxed) == 0; }

  /// Check if the ring buffer is full (producer side view).
  bool Full() const { return size_.load(std::memory_order::relaxed) == N; }

  /// Get the capacity of the ring buffer.
  static constexpr size_t kCapacity = N;
  static constexpr size_t capacity() { return N; }
  size_t Capacity() const { return N; }

  /// Get the approximate size of the ring buffer.
  size_t Size() const { return size_.load(std::memory_order::relaxed); }

 private:
  std::array<T, N> data_{};
  std::atomic<size_t> size_ = 0;
  alignas(std::hardware_destructive_interference_size) size_t write_ptr_ = 0;
  alignas(std::hardware_destructive_interference_size) size_t read_ptr_ = 0;
};

}  // namespace lockfree
}  // namespace nx

#endif  // NX_BASE_LOCKFREE_RINGBUFFER_H__
