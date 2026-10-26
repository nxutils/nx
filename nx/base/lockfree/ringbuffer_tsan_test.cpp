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

#include <gtest/gtest.h>

#include <thread>

#include "nx/base/lockfree/ringbuffer.h"

TEST(RingBufferTsanTest, RaceTest) {
  const size_t count = 1000000000ULL;
  nx::lockfree::RingBuffer<uint64_t, 128> rb;

  std::thread producer([&rb]() {
    for (uint64_t i = 1; i <= count; ++i) {
      while (!rb.Write(i)) {
        // busy wait
      }
    }
  });

  std::thread consumer([&rb]() {
    uint64_t expected = 1;
    uint64_t value = 0;
    while (expected <= count) {
      if (rb.Read(value)) {
        ++expected;
      }
      // else busy wait
    }
  });

  producer.join();
  consumer.join();
}
