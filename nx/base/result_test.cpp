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

#include "nx/base/result.h"

#include <gtest/gtest.h>

enum Error : uint32_t {
  TYPE_1,
  TYPE_2
};

class MovableType {
 public:
  explicit MovableType(int a) : a_(a) {}

  MovableType(MovableType &&other) : a_(other.a_) {
    other.a_ = 0;
  }

  int GetA() const { return a_; }

 private:
  int a_;
};

TEST(ResultTest, API) {
  nx::Result<uint64_t, Error> success(123);
  ASSERT_TRUE(static_cast<bool>(success));
  ASSERT_TRUE(success);
  ASSERT_TRUE(success.has_value());
  ASSERT_FALSE(success.is_error());
  ASSERT_EQ(success.value(), 123);

  nx::Result<uint64_t, Error> failure(Error::TYPE_1);
  ASSERT_FALSE(static_cast<bool>(failure));
  ASSERT_FALSE(failure);
  ASSERT_FALSE(failure.has_value());
  ASSERT_TRUE(failure.is_error());
  ASSERT_EQ(failure.error(), Error::TYPE_1);

  MovableType mt(321);
  nx::Result<MovableType, Error> mt_success(std::move(mt));
  ASSERT_TRUE(static_cast<bool>(mt_success));
  ASSERT_TRUE(mt_success);
  ASSERT_TRUE(mt_success.has_value());
  ASSERT_FALSE(mt_success.is_error());
  ASSERT_EQ(mt_success.value().GetA(), 321);
  // Original object was moved into the result value.
  ASSERT_EQ(mt.GetA(), 0);
}

TEST(BoolResultTest, API) {
  nx::BoolResult success(true);
  ASSERT_TRUE(static_cast<bool>(success));
  ASSERT_TRUE(success);

  const std::string failure_msg = "failed it";
  nx::BoolResult failure(false, failure_msg);
  ASSERT_FALSE(static_cast<bool>(failure));
  ASSERT_FALSE(failure);
  ASSERT_EQ(failure.message(), failure_msg);
}
