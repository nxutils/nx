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

#ifndef NX_H__
#define NX_H__

#include <string_view>

namespace nx::pci {

enum class CapId : uint8_t {
  /// Null
  NUL_ID = 0x00,

  /// Power Management
  PM = 0x01,

  /// AGP (Accelerated Graphics Port)
  AGP = 0x02,

  /// Vital Product Data
  VPD = 0x03,

  /// Slot ID
  SLOTID = 0x04,

  /// Message Signalled Interrupts
  MSI = 0x05,

  /// Compact Host Swap
  CHSWP = 0x06,

  /// PCI-X
  PCIX = 0x07,

  /// Hyper Transport
  HT = 0x08,

  /// Vendor specific
  VNDR = 0x09,

  /// Debug port
  DBG = 0x0A,

  /// Compact Central Resource Control
  CCRC = 0x0B,

  /// PCI Hot-Plug
  HOTPLUG = 0x0C,

  /// Subsystem Vendor ID
  SSVID = 0x0D,

  /// AGP 8x
  AGP3 = 0x0E,

  /// Secure
  SECURE = 0x0F,

  /// PCI Express
  EXP = 0x10,

  /// MSI-X
  MSIX = 0x11,

  /// SATA
  SATA = 0x12,

  /// Advanced Root Complex Features
  AF = 0x13,

  /// Enhanced Allocation
  EA = 0x14,

  /// Flattenning Portal Bridge
  FPB = 0x15,

  /// Used to catch unknown capability IDs.
  CAP_ID_MAX = 0x16
};

constexpr const char *CAP_ID_DESCRIPTION[] = {
    "Null",
    "Power Management",
    "AGP (Accelerated Graphics Port)",
    "Vital Product Data",
    "Slot ID",
    "Message Signaled Interrupts",
    "Compact Host Swap",
    "PCI-X",
    "Hyper Transport",
    "Vendor specific",
    "Debug port",
    "Compact Central Resource Control",
    "PCI Hot-Plug",
    "Subsystem Vendor ID",
    "AGP 8x",
    "Secure",
    "PCI Express",
    "MSI-X",
    "SATA",
    "Advanced Root Complex Features",
    "Enhanced Allocation",
    "Flattenning Portal Bridge",
};

constexpr std::string_view GetCapIdDescription(CapId cap_id) {
  if (cap_id >= CapId::CAP_ID_MAX) {
    return "Unknown Capability ID";
  }
  return CAP_ID_DESCRIPTION[static_cast<size_t>(cap_id)];
}

enum class ECapId : uint16_t {
  NULL_ID = 0x00,
  AER = 0x01,
  VC = 0x02,
  DSN = 0x03,
  PB = 0x04,
  RCLINK = 0x05,
  RCILINK = 0x06,
  RCEC = 0x07,
  MFVC = 0x08,
  VC2 = 0x09,
  RCRB = 0x0a,
  VNDR = 0x0b,
  ACS = 0x0d,
  ARI = 0x0e,
  ATS = 0x0f,
  SRIOV = 0x10,
  MRIOV = 0x11,
  MCAST = 0x12,
  PRI = 0x13,
  REBAR = 0x15,
  DPA = 0x16,
  TPH = 0x17,
  LTR = 0x18,
  SECPCI = 0x19,
  PMUX = 0x1a,
  PASID = 0x1b,
  LNR = 0x1c,
  DPC = 0x1d,
  L1PM = 0x1e,
  PTM = 0x1f,
  M_PCIE = 0x20,
  FRS = 0x21,
  RTR = 0x22,
  DVSEC = 0x23,
  VF_REBAR = 0x24,
  DLNK = 0x25,
  PHYSICAL_LAYER_16GT = 0x26,
  LMR = 0x27,
  HIER_ID = 0x28,
  NPEM = 0x29,
  PHYSICAL_LAYER_32GT = 0x2a,
  ALT_PROT = 0x2b,
  SFI = 0x2c,
  DOE = 0x2e,
  DEV3 = 0x2f,
  IDE = 0x30,
  PHYSICAL_LAYER_64GT = 0x31,
  FLIT_LOG = 0x32,
  FLIT_PM = 0x33,
  FLIT_EI = 0x34,
  SVC = 0x35,
  MMIO_RBL = 0x36,
  NOP_FLIT = 0x37,
  SIOV = 0x38,
  PHYSICAL_LAYER_128GT = 0x39,
  CAPT_D = 0x3a,
  ECAP_ID_MAX = 0x3b,
};

constexpr const char *EXT_CAP_ID_DESCRIPTION[] = {
    "Null Capability",
    "Advanced Error Reporting",
    "Virtual Channel",
    "Device Serial Number",
    "Power Budgeting",
    "Root Complex Link Declaration",
    "Root Complex Internal Link Declaration",
    "Root Complex Event Collector",
    "Multi-Function Virtual Channel",
    "Virtual Channel (2nd ID)",
    "Root Complex Register Block",
    "Vendor Specific",
    "Access Controls",
    "Alternative Routing-ID Interpretation",
    "Address Translation Service",
    "Single Root I/O Virtualization",
    "Multi-Root I/O Virtualization",
    "Multicast",
    "Page Request Interface",
    "Unknown Extented Capability",  // 0x14
    "Resizable BAR",
    "Dynamic Power Allocation",
    "Transaction Processing Hints",
    "Latency Tolerance Reporting",
    "Secondary PCI Express",
    "Protocol Multiplexing",
    "Process Address Space ID",
    "LN Requester",
    "Downstream Port Containment",
    "L1 PM Substates",
    "Precision Time Measurement",
    "PCIe Over M-PHY",
    "FRS Queuing",
    "Readiness Time Reporting",
    "Designated Vendor-Specific",
    "VF Resizable BAR",
    "Data Link Feature",
    "Physical Layer 16.0 GT/s",
    "Lane Margining at Receiver",
    "Hierarchy ID",
    "Native PCIe Enclosure Management",
    "Physical Layer 32.0 GT/s",
    "Alternate Protocol",
    "System Firmware Intermediary",
    "Uknown Extended Capability",  // 0x2d
    "Data Object Exchange",
    "Device 3",
    "Integrity and Data Encryption",
    "Physical Layer 64.0 GT/s",
    "Flit Logging",
    "Flit Performance Measurement",
    "Flit Error Injection",
    "Streamlined Virtual Channel",
    "MMIO Register Block Locator",
    "NOP Flit Extended Capability",
    "Scalable I/O Virtualization",
    "Physical Layer 128.0 GT/s",
    "Captured Data",
};

constexpr std::string_view GetECapIdDescription(ECapId ecap_id) {
  if (ecap_id >= ECapId::ECAP_ID_MAX) {
    return "Unknown Capability ID";
  }
  return EXT_CAP_ID_DESCRIPTION[static_cast<size_t>(ecap_id)];
}

}  // namespace nx::pci

#endif  // NX_H__
