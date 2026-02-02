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

#include "nx/pci/pci.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <memory>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <string_view>

#include "nx/base/result.h"
#include "nx/pci/cap_id.h"
#include "nx/pci/class_code.h"

namespace {

// Byte offsets of the different fields in the config header.
enum class ConfigByteOffset : uint16_t {
  VENDOR_ID = 0,
  DEVICE_ID = 2,
  COMMAND = 4,
  STATUS = 6,
  REVISION = 8,
  PROGRAM_IF = 9,
  SUBCLASS = 10,
  CLASS_CODE = 11,
  HEADER_TYPE = 14,
  CAPABILITIES_POINTER = 0x34,
  EXTENDED_CAPABILITIES = 0x100,
};

// The mask to get the bit which indicates the presence of capabilities
// list in the device config.
constexpr uint16_t STATUS_CAPABILITIES_MASK = 0x10;

std::string GetBdfString(
    uint32_t domain, uint8_t bus, uint8_t device, uint8_t func) {
  uint8_t dom_width = domain > 0xFFFF ? 5 : 4;
  std::ostringstream bdf;
  bdf << std::hex << std::setfill('0') << std::setw(dom_width) << domain << ":"
      << std::setw(2) << static_cast<uint32_t>(bus) << ":"
      << std::setw(2) << static_cast<uint32_t>(device) << "."
      << std::setw(1) << static_cast<uint32_t>(func);
  return bdf.str();
}

std::filesystem::path GetPciDevicesPath() {
  const std::filesystem::path root = "/";
  return root / "sys" / "bus" / "pci" / "devices";
}

std::filesystem::path GetDevicePath(
    uint32_t domain, uint8_t bus, uint8_t device, uint8_t func) {
  return GetPciDevicesPath() / GetBdfString(domain, bus, device, func);
}

std::filesystem::path GetConfigFilePath(
    uint32_t domain, uint8_t bus, uint8_t device, uint8_t func) {
  return GetDevicePath(domain, bus, device, func) / "config";
}

} // Anonymous namespace

namespace nx::pci {

nx::Result<std::unique_ptr<Device>> Device::Load(const std::string &bdf) {
  size_t first_colon = bdf.find(":");
  if (first_colon < 4 || first_colon > 5) {
    return Error{ErrorCode::PCI_INVALID_DOMAIN_NUMBER,
                 "Invalid domain number in " + bdf};
  }
  if (bdf.size() != first_colon + 8) {
    return Error{ErrorCode::PCI_INVALID_BDF_STRING,
                 "Invalid BDF string " + bdf};
  }
  if (bdf[first_colon + 3] != ':') {
    return Error{ErrorCode::PCI_INVALID_BUS_NUMBER,
                 "Invalid bus number in " + bdf};
  }
  if (bdf[first_colon + 6] != '.') {
    return Error{ErrorCode::PCI_INVALID_FUNCTION_NUMBER,
                 "Invalid function number in " + bdf};
  }

  std::string domain_str = bdf.substr(0, first_colon);
  uint32_t domain = std::stoul(domain_str, nullptr, 16);

  std::string bus_str = bdf.substr(first_colon + 1, 2);
  uint8_t bus = std::stoul(bus_str, nullptr, 16);

  std::string device_str = bdf.substr(first_colon + 4, 2);
  uint8_t device = std::stoul(device_str, nullptr, 16);

  std::string func_str = bdf.substr(bdf.size() - 1, 1);
  uint8_t func = std::stoul(func_str, nullptr, 16);

  return Load(domain, bus, device, func);
}

nx::Result<std::unique_ptr<Device>> Device::Load(uint32_t domain,
                                                 uint8_t bus,
                                                 uint8_t device,
                                                 uint8_t func) {
  // Domain value should fit in 5-nibbles (20 bits).
  if (domain > 0xFFFFF) {
    std::ostringstream msgstream;
    msgstream << "Domain value " << std::hex << domain
              << " does not fit in 20 bits";
    return Error{ErrorCode::PCI_INVALID_DOMAIN_NUMBER, msgstream.str()};
  }

  if (func > 0xF) {
    std::ostringstream msgstream;
    msgstream << "Device function value " << std::hex << func
              << " does not fit in 4 bits";
    return Error{ErrorCode::PCI_INVALID_FUNCTION_NUMBER, msgstream.str()};
  }

  const auto device_path = GetDevicePath(domain, bus, device, func);
  const auto bdf_string = ::GetBdfString(domain, bus, device, func);
  std::error_code err;
  if (!std::filesystem::exists(device_path, err)) {
    auto pci_devices_path = GetPciDevicesPath().native();
    return Error{
        ErrorCode::PCI_DEVICE_NOT_FOUND,
        "PCI device " + bdf_string + " not found in " + pci_devices_path};
  }

  std::unique_ptr<Device> pci_device(new Device(domain, bus, device, func));

  std::ifstream config(GetConfigFilePath(domain, bus, device, func),
                       std::ios::binary | std::ios::in);
  if (!config.is_open()) {
    return Error{
        ErrorCode::PCI_ERROR_READING_CONFIG_FILE,
        "Error opening config file for " + bdf_string};
  }
  // We read the first 16 bytes from the config file - This is enough to gather
  // all of the information we need at this point.
  uint8_t config_data[16];
  config.read(reinterpret_cast<char *>(config_data), sizeof(config_data));
  if (config.fail()) {
    return Error{
        ErrorCode::PCI_ERROR_READING_CONFIG_FILE,
        "Error reading config file for " + bdf_string};
  }
  config.close();

  pci_device->vendor_id_ = *reinterpret_cast<uint16_t *>(
      config_data + static_cast<int>(ConfigByteOffset::VENDOR_ID));
  pci_device->device_id_ = *reinterpret_cast<uint16_t *>(
      config_data + static_cast<int>(ConfigByteOffset::DEVICE_ID));
  pci_device->status_ = *reinterpret_cast<uint16_t *>(
      config_data + static_cast<int>(ConfigByteOffset::STATUS));
  pci_device->revision_ = *reinterpret_cast<uint8_t *>(
      config_data + static_cast<int>(ConfigByteOffset::REVISION));
  pci_device->program_if_ = *reinterpret_cast<uint8_t *>(
      config_data + static_cast<int>(ConfigByteOffset::PROGRAM_IF));
  pci_device->subclass_ = *reinterpret_cast<uint8_t *>(
      config_data + static_cast<int>(ConfigByteOffset::SUBCLASS));
  pci_device->class_code_ = static_cast<ClassCode>(*reinterpret_cast<uint8_t *>(
      config_data + static_cast<int>(ConfigByteOffset::CLASS_CODE)));

  return pci_device;
}

std::string Device::GetBdfString() const {
  return ::GetBdfString(domain_, bus_, device_, func_);
}

bool Device::HasCapabilitiesList() const {
  return (STATUS_CAPABILITIES_MASK & status_) != 0;
}

nx::Result<std::vector<Capability>> Device::GetCapabilities() const {
  auto config_path = GetConfigFilePath(domain_, bus_, device_, func_);
  std::ifstream config(config_path, std::ios::binary | std::ios::in);
  if (!config.is_open()) {
    return Error{
        ErrorCode::PCI_ERROR_READING_CONFIG_FILE,
        "Error opening config file " + config_path.native()};
  }

  config.seekg(int(ConfigByteOffset::CAPABILITIES_POINTER));
  if (config.fail()) {
    return Error{
        ErrorCode::PCI_ERROR_READING_CONFIG_FILE,
        "Error seeking to the capabilities pointer in " + config_path.native()};
  }

  uint8_t next_pointer;
  config.read(reinterpret_cast<char *>(&next_pointer), 1);
  if (config.fail()) {
    return Error{
        ErrorCode::PCI_ERROR_READING_CONFIG_FILE,
        "Error reading capabilities pointer in " + config_path.native()};
  }

  std::vector<Capability> capabilities;
  while (next_pointer != 0) {
    config.seekg(next_pointer);
    if (config.fail()) {
      std::ostringstream msgstream;
      msgstream << "Error seeking to the next capability at "
                << std::hex << "0x" << static_cast<uint32_t>(next_pointer)
                << " in " << config_path.native();
      return Error{ErrorCode::PCI_ERROR_READING_CONFIG_FILE, msgstream.str()};
    }

    CapId cap_id;
    config.read(reinterpret_cast<char *>(&cap_id), 1);
    if (config.fail()) {
      std::ostringstream msgstream;
      msgstream << "Error reading capability ID from offset "
                << std::hex << "0x" << static_cast<uint32_t>(next_pointer)
                << " in " << config_path.native();
      return Error{ErrorCode::PCI_ERROR_READING_CONFIG_FILE, msgstream.str()};
    }

    Capability cap;
    cap.id = cap_id;
    cap.offset = next_pointer;
    // The capability data is after the header - header is two bytes, one byte
    // for the capability ID (read into cap_id above), and one byte for the next
    // capability pointer (read into next_pointer below). 
    //
    // If represented as a C data structure, the extended capability header will
    // have the following layout.
    //
    // struct {
    //   uint8_t cap_id;
    //   uint8_t next_pointer;
    // };
    cap.data_offset = next_pointer + 2;
    capabilities.push_back(std::move(cap));
    config.read(reinterpret_cast<char *>(&next_pointer), 1);
    if (config.fail()) {
      std::ostringstream msgstream;
      msgstream << "Error reading next pointer of capability ID "
                << std::hex << "0x" << static_cast<uint32_t>(cap.id) << " ("
                << GetCapIdDescription(cap.id) << ") in "
                << config_path.native();
      return Error{ErrorCode::PCI_ERROR_READING_CONFIG_FILE, msgstream.str()};
    }
  }
  return capabilities;
}

nx::Result<std::vector<ExtendedCapability>>
Device::GetExtendedCapabilities() const {
  auto config_path = GetConfigFilePath(domain_, bus_, device_, func_);
  std::ifstream config(config_path, std::ios::binary | std::ios::in);
  if (!config.is_open()) {
    return Error{
        ErrorCode::PCI_ERROR_READING_CONFIG_FILE,
        "Error opening config file " + config_path.native()};
  }

  uint16_t next_pointer = static_cast<uint16_t>(
      ConfigByteOffset::EXTENDED_CAPABILITIES);
  std::vector<ExtendedCapability> capabilities;
  while (next_pointer != 0) {
    config.seekg(next_pointer);
    if (config.fail()) {
      std::ostringstream msgstream;
      msgstream << "Error seeking to the next extended capability at "
                << std::hex << "0x" << static_cast<uint32_t>(next_pointer)
                << " in " << config_path.native();
      return Error{ErrorCode::PCI_ERROR_READING_CONFIG_FILE, msgstream.str()};
    }

    ECapId cap_id;
    config.read(reinterpret_cast<char *>(&cap_id), 2);
    if (config.fail()) {
      std::ostringstream msgstream;
      msgstream << "Error reading extended capability ID from offset "
                << std::hex << "0x" << static_cast<uint32_t>(next_pointer)
                << " in " << config_path.native();
      return Error{ErrorCode::PCI_ERROR_READING_CONFIG_FILE, msgstream.str()};
    }

    uint16_t version_and_next_pointer;
    config.read(reinterpret_cast<char *>(&version_and_next_pointer), 2);
    if (config.fail()) {
      std::ostringstream msgstream;
      msgstream << "Error reading next pointer of capability ID "
                << std::hex << "0x" << static_cast<uint32_t>(cap_id) << " ("
                << GetECapIdDescription(cap_id) << ") in "
                << config_path.native();
      return Error{ErrorCode::PCI_ERROR_READING_CONFIG_FILE, msgstream.str()};
    }
    ExtendedCapability cap;
    cap.id = cap_id;
    cap.offset = next_pointer;
    // Capability data is after the capability header. For extended capabilities
    // the header is 4 bytes:
    // 1. Two bytes for the capability ID
    // 2. Two bytes for version (first 4 bits) and next capability offset
    //    (12 bits).
    //
    // If represented as a C data structure, the extended capability header will
    // have the following layout.
    //
    // struct {
    //   uint16_t cap_id;
    //   uint8_t version: 4;
    //   uint16_t next_pointer: 12;
    // };
    cap.data_offset = next_pointer + 4;
    cap.version = static_cast<uint8_t>(version_and_next_pointer & 0xF);
    capabilities.push_back(std::move(cap));

    next_pointer = version_and_next_pointer >> 4;
  }
  return capabilities;
}

std::optional<nx::Error> Device::ReadExtendedCapabilityData(
    ECapId cap_id, uint16_t offset, std::span<uint8_t> data) const {
  auto config_path = GetConfigFilePath(domain_, bus_, device_, func_);
  std::ifstream config(config_path, std::ios::binary | std::ios::in);
  if (!config.is_open()) {
    return Error{
        ErrorCode::PCI_ERROR_READING_CONFIG_FILE,
        "Error opening config file " + config_path.native()};
  }

  uint16_t next_pointer = static_cast<uint16_t>(
      ConfigByteOffset::EXTENDED_CAPABILITIES);
  while (next_pointer != 0) {
    config.seekg(next_pointer);
    if (config.fail()) {
      std::ostringstream msgstream;
      msgstream << "Error seeking to the next extended capability at "
                << std::hex << "0x" << static_cast<uint32_t>(next_pointer)
                << " in " << config_path.native();
      return Error{ErrorCode::PCI_ERROR_READING_CONFIG_FILE, msgstream.str()};
    }

    ECapId current_cap_id;
    config.read(reinterpret_cast<char *>(&current_cap_id), 2);
    if (config.fail()) {
      std::ostringstream msgstream;
      msgstream << "Error reading extended capability ID from offset "
                << std::hex << "0x" << static_cast<uint32_t>(next_pointer)
                << " in " << config_path.native();
      return Error{ErrorCode::PCI_ERROR_READING_CONFIG_FILE, msgstream.str()};
    }

    uint16_t version_and_next_pointer;
    config.read(reinterpret_cast<char *>(&version_and_next_pointer), 2);
    if (config.fail()) {
      std::ostringstream msgstream;
      msgstream << "Error reading next pointer of capability ID "
                << std::hex << "0x" << static_cast<uint32_t>(current_cap_id)
                << " ("  << GetECapIdDescription(cap_id) << ") in "
                << config_path.native();
      return Error{ErrorCode::PCI_ERROR_READING_CONFIG_FILE, msgstream.str()};
    }

    if (cap_id == current_cap_id) {
      if (offset > 0) {
        config.seekg(next_pointer
                     + 4 // Go past the header (cap_id, version, next_pointer)
                     + offset);
        if (config.fail()) {
          std::ostringstream msgstream;
          msgstream << "Error seeking to specified offset of capability ID "
                    << std::hex << "0x" << static_cast<uint32_t>(current_cap_id)
                    << " ("  << GetECapIdDescription(cap_id) << ") in "
                    << config_path.native();
         return Error{ErrorCode::PCI_ERROR_READING_CONFIG_FILE,
                      msgstream.str()};
        }
      }
      
      config.read(reinterpret_cast<char *>(data.data()), data.size());
      if (config.fail()) {
        std::ostringstream msgstream;
        msgstream << "Error reading data of capability ID "
                  << std::hex << "0x" << static_cast<uint32_t>(current_cap_id)
                  << " ("  << GetECapIdDescription(cap_id) << ") in "
                  << config_path.native();
       return Error{ErrorCode::PCI_ERROR_READING_CONFIG_FILE,
                    msgstream.str()};
      }
      return std::nullopt;
    }

    next_pointer = version_and_next_pointer >> 4;
  }

  std::ostringstream msgstream;
  msgstream << "Extended capability 0x" << std::hex
            << static_cast<uint32_t>(cap_id) << " (" 
            << GetECapIdDescription(cap_id) << ") not found in "
            << config_path.native();
  return Error{ErrorCode::PCI_EXTENDED_CAP_NOT_FOUND, msgstream.str()};
}

nx::Result<ExtendedCapability> Device::GetExtendedCapability(
    ECapId cap_id) const {
  auto caps_or_error = GetExtendedCapabilities();
  if (!caps_or_error.has_value()) {
    return caps_or_error.error();
  }
  auto &caps = caps_or_error.value();

  for (auto &ecap : caps) {
    if (ecap.id == cap_id) {
      return ecap;
    }
  }

  std::ostringstream msgstream;
  msgstream << "Extended capability 0x" << std::hex
            << static_cast<uint32_t>(cap_id) << " (" 
            << GetECapIdDescription(cap_id) << ") not found";
  return Error{ErrorCode::PCI_EXTENDED_CAP_NOT_FOUND, msgstream.str()};
}

std::optional<nx::Error> Device::WriteExtendedCapabilityData(
    ECapId cap_id, uint16_t offset, std::span<uint8_t> data) const {
  auto cap_or_error = GetExtendedCapability(cap_id);
  if (!cap_or_error.has_value()) {
    return cap_or_error.error();
  }
  auto &ecap = cap_or_error.value();

  auto config_path = GetConfigFilePath(domain_, bus_, device_, func_);
  std::ofstream config(config_path, std::ios::binary | std::ios::out);
  if (!config.is_open()) {
    return Error{
        ErrorCode::PCI_ERROR_WRITING_CONFIG,
        "Error opening config file " + config_path.native()};
  }

  uint16_t data_offset = ecap.data_offset + offset;
  config.seekp(data_offset);
  if (config.fail()) {
    std::ostringstream msgstream;
    msgstream << "Error seeking to the extended capability data at "
              << std::hex << "0x" << data_offset
              << " in " << config_path.native();
    return Error{ErrorCode::PCI_ERROR_WRITING_CONFIG, msgstream.str()};
  }

  config.write(reinterpret_cast<char *>(data.data()), data.size());
  if (config.fail()) {
    std::ostringstream msgstream;
    msgstream << "Error writing to the extended capability data at "
              << std::hex << "0x" << data_offset
              << " in " << config_path.native();
    return Error{ErrorCode::PCI_ERROR_WRITING_CONFIG, msgstream.str()};
  }

  config.close();
  return std::nullopt;
}

} // namespace nx::pci
