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

#include <iomanip>
#include <iostream>
#include <span>

#include "nx/pci/cap_id.h"
#include "nx/pci/pci.h"

int main(int argc, char *argv[]) {
  if (argc != 5) {
    std::cout << "Usage:\n"
              << "  write_ecap32 <BDF string> <ECap ID> <Offset> <Value>\n"
              << "Example:\n"
              << "  write_ecap32 0000:ab:01.0 0x1 0 0xffff\n";
    return 1;
  }

  auto device_or_error = nx::pci::Device::Load(argv[1]);
  if (!device_or_error.has_value()) {
    auto error = device_or_error.error();
    std::cout << "Error loading device: " << error.message() << '\n';
    return 1;
  }
  auto device = std::move(device_or_error.value());

  uint16_t ecap_id_val = std::stoul(argv[2], nullptr, 0);
  uint16_t offset = std::stoul(argv[3], nullptr, 0);
  uint32_t value = std::stoul(argv[4], nullptr, 0);

  std::span<uint8_t> data(reinterpret_cast<uint8_t *>(&value), sizeof(value));

  auto may_be_error = device->WriteExtendedCapabilityData(
      static_cast<nx::pci::ECapId>(ecap_id_val), offset, data);
  if (may_be_error.has_value()) {
    auto error = may_be_error.value();
    std::cout << "Error reading extended capability data: " << error.message()
              << '\n';
    return 1;
  }

  return 0;
}
