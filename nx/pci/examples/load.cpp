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

#include <iostream>

#include "nx/pci/cap_id.h"
#include "nx/pci/class_code.h"
#include "nx/pci/pci.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage:\n"
              << "  load <BDF string>\n"
              << "Example:\n"
              << "  load 0000:ab:01.0\n";
    return 1;
  }

  auto device_or_error = nx::pci::Device::Load(argv[1]);
  if (!device_or_error.has_value()) {
    auto error = device_or_error.error();
    std::cout << "Error loading device: " << error.message() << '\n';
    return 1;
  }

  auto device = std::move(device_or_error.value());
  auto class_code = device->class_code();
  std::cout << "!!!! Successfully loaded PCI device !!!!\n"
            << std::hex << "  Vendor ID: 0x" << device->vendor_id() << '\n'
            << "  Device ID: 0x" << device->device_id() << '\n'
            << " Class Code: 0x" << static_cast<uint32_t>(class_code) << " - "
            << nx::pci::GetClassCodeDescription(class_code) << '\n'
            << "   Subclass: 0x" << int(device->subclass()) << '\n'
            << "Capabilities List: "
            << (device->HasCapabilitiesList() ? "Yes" : "No") << '\n';

  if (device->HasCapabilitiesList()) {
    auto caps_or_error = device->GetCapabilities();
    if (!caps_or_error.has_value()) {
      auto &error = caps_or_error.error();
      std::cout << "Error reading capabilities: " << error.message() << '\n';
      return 1;
    }
    std::cout << "List of normal capabilities:\n";
    auto &capabilities = caps_or_error.value();
    for (auto &cap : capabilities) {
      std::cout << std::hex << "Offset: 0x" << cap.offset << " ID: 0x"
                << static_cast<uint32_t>(cap.id) << " - "
                << GetCapIdDescription(cap.id) << '\n';
    }
  }

  auto extended_caps_or_error = device->GetExtendedCapabilities();
  if (!extended_caps_or_error.has_value()) {
    auto &error = extended_caps_or_error.error();
    std::cout << "Error reading capabilities: " << error.message() << '\n';
    return 1;
  }

  std::cout << "List of extended capabilities:\n";
  auto &extended_capabilities = extended_caps_or_error.value();
  for (auto &ecap : extended_capabilities) {
    std::cout << std::hex << "Offset: 0x" << ecap.offset << " ID: 0x"
              << static_cast<uint32_t>(ecap.id) << " [v" << std::dec
              << static_cast<uint32_t>(ecap.version) << "] - "
              << GetECapIdDescription(ecap.id) << '\n';
  }

  return 0;
}
