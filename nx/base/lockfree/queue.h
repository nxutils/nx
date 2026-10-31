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

#ifndef NX_BASE_LOCKFREE_QUEUE_H__
#define NX_BASE_LOCKFREE_QUEUE_H__

#include <cstddef>

namespace nx {
namespace lockfree {

template <typename T, size_t C>
class Queue {
 public:
  Queue() = default;
  ~Queue() = default;

  Queue(const Queue&) = delete;
  Queue& operator=(const Queue&) = delete;
  Queue(Queue&&) = delete;
  Queue& operator=(Queue&&) = delete;

 private:
  std::array<T, C> data_{};
};

}  // namespace lockfree
}  // namespace nx

#endif  // NX_BASE_LOCKFREE_QUEUE_H__
