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

#ifndef NX_BASE_ERROR_H__
#define NX_BASE_ERROR_H__

#include <string>
#include <type_traits>

namespace nx {

/// An error with an error code of type |ErrorType| and an optional message.
///
/// |ErrorType| should be an enum type describing the possible error codes.
template <typename ErrorType>
class Error {
  static_assert(std::is_enum_v<ErrorType>, "ErrorType should be an enum type");

 public:
  Error(ErrorType code) : code_(code) {}

  Error(ErrorType code, const std::string& msg) : code_(code), msg_(msg) {}

  ErrorType code() const { return code_; }

  const std::string& message() const { return msg_; }

 private:
  ErrorType code_;
  std::string msg_;
};

}  // namespace nx

#endif  // NX_BASE_ERROR_H__
