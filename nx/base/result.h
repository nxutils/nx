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

#ifndef NX_BASE_RESULT_H__
#define NX_BASE_RESULT_H__

#include <expected>
#include <string>

#include "nx/base/error.h"

namespace nx {

/// A type which is convenience over std::expected.
///
/// This is much less restricted type as compare to std::expected. We want this
/// type to be purely used as a function/method return value.
template <typename V>
class Result {
 public:
  /// Construct a successful result by copying the value.
  Result(const V &v) : val_or_error_(v) {}

  /// Construct a successful result by moving the value.
  Result(V &&v) : val_or_error_(std::move(v)) {}

  /// Construct an error result.
  Result(const Error &e) : val_or_error_(std::unexpected(e)) {}

  /// Check if the object represents a successful result.
  ///
  /// If the result is successful, then the `value` method can be used to
  /// fetch the result.
  operator bool() const { return val_or_error_.has_value(); }

  /// Check if the object contains a successful result value.
  ///
  /// This is another way to check if the result object represents successful
  /// result.
  bool has_value() const { return val_or_error_.has_value(); }

  /// Check if the object represents an error result.
  ///
  /// If `true` is returned, then the error value can be fetched using the
  /// `error` method.
  bool is_error() const { return !has_value(); }

  /// Return the successful value.
  V &value() { return val_or_error_.value(); }

  /// Return the error value.
  Error &error() { return val_or_error_.error(); }

 private:
  std::expected<V, Error> val_or_error_;
};

/// A type to return a boolean value along with string message.
///
/// There are times when returning just a `true`/`false` value is not
/// sufficient - there is need to attached a string message which explains the
/// boolean result. This type is useful for such cases.
class BoolResult {
 public:
  /// Contruct a boolean result without any associated message.
  BoolResult(bool value) : val_(value) {}

  /// Construct a boolean result with an associated message `msg`.
  BoolResult(bool value, const std::string &msg) : val_(value), msg_(msg) {}

  BoolResult(const BoolResult &) = default;
  BoolResult(BoolResult &&) = default;
  ~BoolResult() = default;

  BoolResult &operator=(const BoolResult &rhs) = default;
  BoolResult &operator=(BoolResult &&rhs) = default;

  /// Check if the object represents a `true` value.
  operator bool() const { return val_; }

  /// Return the associated message.
  const std::string &message() const { return msg_; }

 private:
  bool val_;
  std::string msg_;
};

}  // namespace nx

#endif  // NX_BASE_RESULT_H__
