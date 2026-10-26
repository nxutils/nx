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

namespace nx {

enum class ErrorCode : uint32_t {
  FILE_NOT_FOUND,

  // PCI related errors.
  PCI_DEVICE_NOT_FOUND,
  PCI_INVALID_BDF_STRING,
  PCI_INVALID_BUS_NUMBER,
  PCI_INVALID_DEVICE_NUMBER,
  PCI_INVALID_DOMAIN_NUMBER,
  PCI_INVALID_FUNCTION_NUMBER,
  PCI_ERROR_READING_CONFIG_FILE,
  PCI_ERROR_WRITING_CONFIG,
  PCI_EXTENDED_CAP_NOT_FOUND,
};

class Error {
 public:
  Error(ErrorCode code) : code_(code) {}

  Error(ErrorCode code, const std::string &msg) : code_(code), msg_(msg) {}

  ErrorCode code() const { return code_; }

  const std::string &message() const { return msg_; }

 private:
  ErrorCode code_;
  std::string msg_;
};

}  // namespace nx

#endif  // NX_BASE_ERROR_H__
