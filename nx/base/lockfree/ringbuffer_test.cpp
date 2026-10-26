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

#include "nx/base/lockfree/ringbuffer.h"

#include <gtest/gtest.h>

#include <string>

TEST(RingBufferTest, Construct) {
  nx::lockfree::RingBuffer<int, 8> rb;
  EXPECT_EQ(rb.Capacity(), 8u);
  EXPECT_TRUE(rb.Empty());
  EXPECT_FALSE(rb.Full());
  EXPECT_EQ(rb.Size(), 0u);
}

TEST(RingBufferTest, WriteReadSingle) {
  nx::lockfree::RingBuffer<int, 4> rb;
  int out = 0;
  EXPECT_FALSE(rb.Read(out));
  EXPECT_TRUE(rb.Empty());

  EXPECT_TRUE(rb.Write(42));
  EXPECT_FALSE(rb.Empty());
  EXPECT_EQ(rb.Size(), 1u);

  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, 42);
  EXPECT_TRUE(rb.Empty());
  EXPECT_EQ(rb.Size(), 0u);
}

TEST(RingBufferTest, WriteReadMultiple) {
  nx::lockfree::RingBuffer<int, 8> rb;
  for (int i = 1; i <= 5; ++i) {
    EXPECT_TRUE(rb.Write(i));
  }
  EXPECT_EQ(rb.Size(), 5u);
  EXPECT_FALSE(rb.Full());

  int out = 0;
  for (int i = 1; i <= 5; ++i) {
    EXPECT_TRUE(rb.Read(out));
    EXPECT_EQ(out, i);
  }
  EXPECT_TRUE(rb.Empty());
  EXPECT_EQ(rb.Size(), 0u);
}

TEST(RingBufferTest, Full) {
  nx::lockfree::RingBuffer<int, 4> rb;
  EXPECT_TRUE(rb.Write(1));
  EXPECT_TRUE(rb.Write(2));
  EXPECT_TRUE(rb.Write(3));
  EXPECT_TRUE(rb.Write(4));
  EXPECT_EQ(rb.Size(), 4u);
  EXPECT_TRUE(rb.Full());
  EXPECT_FALSE(rb.Write(5));  // should fail when full

  int out = 0;
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, 1);
  EXPECT_FALSE(rb.Full());
  EXPECT_EQ(rb.Size(), 3u);
  EXPECT_TRUE(rb.Write(5));
  EXPECT_TRUE(rb.Full());
}

TEST(RingBufferTest, WrapAround) {
  nx::lockfree::RingBuffer<int, 4> rb;
  EXPECT_TRUE(rb.Write(1));
  EXPECT_TRUE(rb.Write(2));
  EXPECT_TRUE(rb.Write(3));
  EXPECT_TRUE(rb.Write(4));
  EXPECT_TRUE(rb.Full());

  int out = 0;
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, 1);
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, 2);
  EXPECT_FALSE(rb.Full());

  EXPECT_TRUE(rb.Write(5));
  EXPECT_TRUE(rb.Write(6));
  EXPECT_TRUE(rb.Full());

  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, 3);
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, 4);
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, 5);
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, 6);
  EXPECT_TRUE(rb.Empty());
}

TEST(RingBufferTest, WithStrings) {
  nx::lockfree::RingBuffer<std::string, 4> rb;
  EXPECT_TRUE(rb.Write(std::string("hello")));
  EXPECT_TRUE(rb.Write(std::string("world")));
  EXPECT_TRUE(rb.Write(std::string("test")));
  EXPECT_TRUE(rb.Write(std::string("x")));
  EXPECT_FALSE(rb.Write(std::string("fail")));

  std::string out;
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, "hello");
  EXPECT_TRUE(rb.Write(std::string("ok")));
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, "world");
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, "test");
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, "x");
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, "ok");
  EXPECT_FALSE(rb.Read(out));
}

TEST(RingBufferTest, MoveWrite) {
  nx::lockfree::RingBuffer<std::string, 4> rb;
  std::string s1 = "move";
  EXPECT_TRUE(rb.Write(std::move(s1)));
  // After move, s1 may be empty
  EXPECT_TRUE(rb.Write(std::string("value")));
  EXPECT_TRUE(rb.Write(std::string("a")));
  EXPECT_TRUE(rb.Write(std::string("b")));
  std::string out;
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, "move");
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, "value");
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, "a");
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, "b");
}

TEST(RingBufferTest, ReadOnEmpty) {
  nx::lockfree::RingBuffer<double, 8> rb;
  double out = 123.45;
  EXPECT_FALSE(rb.Read(out));
  // out unchanged
  EXPECT_DOUBLE_EQ(out, 123.45);
  EXPECT_TRUE(rb.Empty());
}

TEST(RingBufferTest, SizeConsistency) {
  nx::lockfree::RingBuffer<int, 16> rb;
  EXPECT_EQ(rb.Size(), 0u);
  for (int i = 0; i < 7; ++i) {
    EXPECT_TRUE(rb.Write(i));
    EXPECT_EQ(rb.Size(), static_cast<size_t>(i + 1));
  }
  for (int i = 0; i < 7; ++i) {
    int out;
    EXPECT_TRUE(rb.Read(out));
    EXPECT_EQ(rb.Size(), static_cast<size_t>(7 - i - 1));
  }
  EXPECT_EQ(rb.Size(), 0u);
}

TEST(RingBufferTest, FillAndDrain) {
  nx::lockfree::RingBuffer<int, 4> rb;
  // Fill
  EXPECT_TRUE(rb.Write(10));
  EXPECT_TRUE(rb.Write(20));
  EXPECT_TRUE(rb.Write(30));
  EXPECT_TRUE(rb.Write(40));
  EXPECT_TRUE(rb.Full());
  // Drain one
  int out;
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, 10);
  // Fill again
  EXPECT_TRUE(rb.Write(50));
  EXPECT_TRUE(rb.Full());
  // Drain rest
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, 20);
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, 30);
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, 40);
  EXPECT_TRUE(rb.Read(out));
  EXPECT_EQ(out, 50);
  EXPECT_TRUE(rb.Empty());
}
