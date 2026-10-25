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

#ifndef NX_PCI_PCI_H__
#define NX_PCI_PCI_H__

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "nx/base/error.h"
#include "nx/base/result.h"

#include "nx/pci/cap_id.h"
#include "nx/pci/class_code.h"

namespace nx::pci {

/// Structure capturing a normal PCI capability information.
struct Capability {
  /// The capability ID.
  CapId id;

  /// The offset of the capability in the PCI config space.
  ///
  /// This is the offset at which the capability header is present.
  uint16_t offset;

  /// The offset of the capability data in the PCI config space.
  ///
  /// This is the offset of the first byte after the capability header.
  uint16_t data_offset;
};

/// Structure capturing an extended PCI capability information.
struct ExtendedCapability {
  /// The capability ID.
  ECapId id;

  /// The version of the capability.
  uint8_t version;

  /// The offset of the capability in the PCI config space.
  ///
  /// This is the offset at which the capability header is present.
  uint16_t offset;

  /// The offset of the capability data in the PCI config space.
  ///
  /// This is the offset of the first byte after the capability header.
  uint16_t data_offset;
};

class Device {
 public:
  /// Load a device using the string representation of the BDF.
  ///
  /// The expected format is:
  ///
  ///   dddd[d]:BB:[DD].F
  ///
  /// where "dddd[d]" is a 16-bit or 20-bit of domain number, BB is a 8-bit
  /// bus number, DD is a 8-bit device number and F is a 4-bit function
  /// number all specified in hexadecimal digits.
  static nx::Result<std::unique_ptr<Device>> Load(const std::string &bdf);

  /// Load device using the given domain, bus, device and function numbers.
  static nx::Result<std::unique_ptr<Device>> Load(uint32_t domain,
                                                  uint8_t bus,
                                                  uint8_t device,
                                                  uint8_t func);

  /// Return the domain number.
  uint32_t domain() const { return domain_; }

  /// Return the bus number.
  uint8_t bus() const { return bus_; }

  /// Return the device number.
  uint8_t device() const { return device_; }

  /// Return the function number.
  uint8_t function() const { return func_; }

  /// Return the vendor ID.
  uint16_t vendor_id() const { return vendor_id_; }

  /// Return the device ID.
  uint16_t device_id() const { return device_id_; }

  /// Return the class code.
  ClassCode class_code() const { return class_code_; }

  /// Return the subclass code.
  uint8_t subclass() const { return subclass_; }

  /// Return the program interface code.
  uint8_t program_if() const { return program_if_; }

  /// Return the revision number.
  uint8_t revision() const { return revision_; }

  /// Return the BDF string for this device.
  std::string GetBdfString() const;

  /// Return true if this device has the normal capabilities list.
  bool HasCapabilitiesList() const;

  /// Return the list of normal capabilities supported by the device.
  nx::Result<std::vector<Capability>> GetCapabilities() const;

  /// Return the list of extended capabilities supported by the device.
  nx::Result<std::vector<ExtendedCapability>> GetExtendedCapabilities() const;

  /// Return extended capability information for capability |cap_id|.
  nx::Result<ExtendedCapability> GetExtendedCapability(ECapId cap_id) const;

  /// Return |size| bytes of data of the extended capability |cap_id|.
  ///
  /// The data is read from byte offset |offset| within the capability data.
  /// The offset size should not include the capability header - which means
  /// that the offset of the first byte after the capability header is 0.
  std::optional<nx::Error> ReadExtendedCapabilityData(
      ECapId cap_id, uint16_t offset, std::span<uint8_t> data) const;

  /// Write |data| to an extended capability register at offset |offset|.
  ///
  /// The data is written at an offset |offset| after the capability header.
  std::optional<nx::Error> WriteExtendedCapabilityData(
      ECapId cap_id, uint16_t offset, std::span<uint8_t> data) const;

 private:
  Device(uint32_t domain, uint8_t bus, uint8_t device, uint8_t func)
      : domain_(domain), bus_(bus), device_(device), func_(func) {}

  uint32_t domain_;
  uint8_t bus_;
  uint8_t device_;
  uint8_t func_;

  uint16_t vendor_id_;
  uint16_t device_id_;
  uint16_t status_;
  uint8_t revision_;
  ClassCode class_code_;
  uint8_t subclass_;
  uint8_t program_if_;
};

} // namespace nx::pci

#endif // NX_PCI_PCI_H__
