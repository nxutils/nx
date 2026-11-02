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

#ifndef NX_PCI_RESULT_H__
#define NX_PCI_RESULT_H__

#include <cstdint>

#include "nx/base/error.h"
#include "nx/base/result.h"

namespace nx::pci {

/// Error codes for the PCI library.
enum class ErrorCode : uint32_t {
  kDeviceNotFound,
  kInvalidBdfString,
  kInvalidBusNumber,
  kInvalidDeviceNumber,
  kInvalidDomainNumber,
  kInvalidFunctionNumber,
  kErrorReadingConfigFile,
  kErrorWritingConfig,
  kExtendedCapNotFound,
};

/// Error type for the PCI library.
using Error = nx::Error<ErrorCode>;

/// Result type for the PCI library.
template <typename V>
using Result = nx::Result<V, ErrorCode>;

}  // namespace nx::pci

#endif  // NX_PCI_RESULT_H__
