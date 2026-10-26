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

#ifndef NX_PCI_CLASS_CODE_H__
#define NX_PCI_CLASS_CODE_H__

#include <string_view>

namespace nx::pci {

enum class ClassCode : uint8_t {
  UNCLASSIFIED_DEVICE = 0x00,
  MASS_STORAGE_CONTROLLER = 0x01,
  NETWORK_CONTROLLER = 0x02,
  DISPLAY_CONTROLLER = 0x03,
  MULTIMEDIA_CONTROLLER = 0x04,
  MEMORY_CONTROLLER = 0x05,
  BRIDGE = 0x06,
  COMMUNICATION_CONTROLLER = 0x07,
  GENERIC_SYSTEM_PERIPHERAL = 0x08,
  INPUT_DEVICE_CONTROLLER = 0x09,
  DOCKING_STATION = 0x0A,
  PROCESSOR = 0x0B,
  SERIAL_BUS_CONTROLLER = 0x0C,
  WIRELESS_CONTROLLER = 0x0D,
  INTELLIGENT_CONTROLLER = 0x0E,
  SATELLITE_COMMUNICATIONS_CONTROLLER = 0x0F,
  ENCRYPTION_CONTROLLER = 0x10,
  SIGNAL_PROCESSING_CONTROLLER = 0x11,
  PROCESSING_ACCELERATORS = 0x12,
  NON_ESSENTIAL_INSTRUMENTATION = 0x13,
  COPROCESSOR = 0x40,
  UNASSIGNED_CLASS = 0xFF,
};

struct ClassCodeAndDescription {
  ClassCode code;
  const char *description;
};

constexpr ClassCodeAndDescription CLASS_CODE_DESCRIPTION[] = {
    {ClassCode::UNCLASSIFIED_DEVICE, "Unclassified Device"},
    {ClassCode::MASS_STORAGE_CONTROLLER, "Mass Storage Controller"},
    {ClassCode::NETWORK_CONTROLLER, "Network Controller"},
    {ClassCode::DISPLAY_CONTROLLER, "Display Controller"},
    {ClassCode::MULTIMEDIA_CONTROLLER, "Multimedia Controller"},
    {ClassCode::MEMORY_CONTROLLER, "Memory Controller"},
    {ClassCode::BRIDGE, "Bridge"},
    {ClassCode::COMMUNICATION_CONTROLLER, "Communication Controller"},
    {ClassCode::GENERIC_SYSTEM_PERIPHERAL, "Generic System Peripheral"},
    {ClassCode::INPUT_DEVICE_CONTROLLER, "Input Device Controller"},
    {ClassCode::DOCKING_STATION, "Docking Station"},
    {ClassCode::PROCESSOR, "Processor"},
    {ClassCode::SERIAL_BUS_CONTROLLER, "Serial Bus Controller"},
    {ClassCode::WIRELESS_CONTROLLER, "Wireless Controller"},
    {ClassCode::INTELLIGENT_CONTROLLER, "Intelligent Controller"},
    {ClassCode::SATELLITE_COMMUNICATIONS_CONTROLLER,
     "Satellite Communications Controller"},
    {ClassCode::ENCRYPTION_CONTROLLER, "Encryption Controller"},
    {ClassCode::SIGNAL_PROCESSING_CONTROLLER, "Signal Processing Controller"},
    {ClassCode::PROCESSING_ACCELERATORS, "Processing Accelerators"},
    {ClassCode::NON_ESSENTIAL_INSTRUMENTATION, "Non-essential Instrumentation"},
    {ClassCode::COPROCESSOR, "Coprocessor"},
    {ClassCode::UNASSIGNED_CLASS, "Unassigned Class"},
};

constexpr std::string_view GetClassCodeDescription(ClassCode code) {
  for (auto &code_and_description : CLASS_CODE_DESCRIPTION) {
    if (code_and_description.code == code) {
      return code_and_description.description;
    }
  }
  return "Unknown Class Code";
}

}  // namespace nx::pci

#endif  // NX_PCI_CLASS_CODE_H__
