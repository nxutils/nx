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

#include "nx/pci/cap_id.h"
#include "nx/pci/pci.h"

int main(int argc, char *argv[]) {
  if (argc != 5) {
    std::cout << "Usage:\n"
              << "  read_ecap <BDF string> <ECap ID> <Offset> <Size>\n"
              << "Example:\n"
              << "  read_ecap 0000:ab:01.0 0x1 0 4\n";
    return 1;
  }

  uint16_t ecap_id_val = std::stoul(argv[2], nullptr, 0);
  uint16_t offset = std::stoul(argv[3], nullptr, 0);
  uint16_t size = std::stoul(argv[4], nullptr, 0);

  if (size > 4 || size < 1) {
    std::cout << "Size argument should be one of {1, 2, 3, 4}\n";
    return 1;
  }

  auto device_or_error = nx::pci::Device::Load(argv[1]);
  if (!device_or_error.has_value()) {
    auto error = device_or_error.error();
    std::cout << "Error loading device: " << error.message() << '\n';
    return 1;
  }
  auto device = std::move(device_or_error.value());

  uint32_t data = 0;
  auto may_be_error = device->ReadExtendedCapabilityData(
      static_cast<nx::pci::ECapId>(ecap_id_val), offset,
      std::span<uint8_t>(reinterpret_cast<uint8_t *>(&data), size));
  if (may_be_error.has_value()) {
    auto error = may_be_error.value();
    std::cout << "Error reading extended capability data: " << error.message()
              << '\n';
    return 1;
  }

  std::cout << "Capability data: 0x" << std::hex << data << '\n';

  return 0;
}
