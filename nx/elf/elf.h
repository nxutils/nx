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

#ifndef NX_ELF_ELF_H__
#define NX_ELF_ELF_H__

#include <cstdint>
#include <filesystem>
#include <string>

#include "nx/elf/result.h"

namespace nx::elf {

constexpr uint16_t kNumBaseElfTypes = 5;
constexpr uint64_t kOsSpecificElfTypeStart = 0xfe00;
constexpr uint64_t kOsSpecificElfTypeEnd = 0xfeff;
constexpr uint64_t kProcSpecificElfTypeStart = 0xff00;
constexpr uint64_t kProcSpecificElfTypeEnd = 0xffff;

constexpr uint32_t kElfVersionInvalid = 0x0;
constexpr uint32_t kElfVersionCurrent = 0x1;

constexpr uint8_t kHeaderVersionCurrent = 0x1;

constexpr uint8_t kMagic[] = { '0x7f', 'E', 'L', 'F'};

enum class ElfClass : uint8_t {
  /// 32-bit ELF object
  k32 = 1,

  /// 64-bit ELF object
  k64 = 2,
};

enum class Type : uint16_t {
  /// No file type
  kNone = 0,

  /// Relocatable object file
  kRel = 1,

  /// Executable file
  kExec = 2,

  /// Shared object file
  kDyn = 3,

  /// Core file
  kCore = 4,
};

enum class Endianness : uint8_t {
  kLittle = 1,
  kBig = 2,
};

enum class OsAbi : uint8_t {
  kSystemV = 0x00,
  kLinux = 0x03,
};

enum class Machine : uint16_t {
  kNone = 0,  // No machine
  kM32 = 1,  // AT&T WE 32100
  kSparc = 2,  // SUN SPARC
  k386 = 3,  // Intel 80386
  k68k = 4,  // Motorola m68k family
  k88k = 5,  // Motorola m88k family
  kIaMcu = 6,  // Intel MCU
  k860 = 7,  // Intel 80860
  kMips = 8,  // MIPS R3000 big-endian
  kS370 = 9,  // IBM System/370
  kMipsRs3Le = 10,  // MIPS R3000 little-endian

  kPaRisc = 15,  // HPPA

  kVpp500 = 17,  // Fujitsu VPP500
  kSparc32plus = 18,  // Sun's "v8plus"
  k960 = 19,  // Intel 80960
  kPpc = 20,  // PowerPC
  kPpc64 = 21,  // PowerPC 64-bit
  kS390 = 22,  // IBM S390
  kSpu = 23,  // IBM SPU/SPC

  kV800 = 36,  // NEC V800 series
  kFr20 = 37,  // Fujitsu FR20
  kRh32 = 38,  // TRW RH-32
  kRce = 39,  // Motorola RCE
  kArm = 40,  // ARM
  kFakeAlpha = 41,  // Digital Alpha
  kSh = 42,  // Hitachi SH
  kSparcv9 = 43,  // SPARC v9 64-bit
  kTricore = 44,  // Siemens Tricore
  kArc = 45,  // Argonaut RISC Core
  kH8300 = 46,  // Hitachi H8/300
  kH8300h = 47,  // Hitachi H8/300H
  kH8s = 48,  // Hitachi H8S
  kH8500 = 49,  // Hitachi H8/500
  kIa64 = 50,  // Intel Merced
  kMipsX = 51,  // Stanford MIPS-X
  kColdfire = 52,  // Motorola Coldfire
  k68hc12 = 53,  // Motorola M68HC12
  kMma = 54,  // Fujitsu MMA Multimedia Accelerator
  kPcp = 55,  // Siemens PCP
  kNcpu = 56,  // Sony nCPU embeeded RISC
  kNdr1 = 57,  // Denso NDR1 microprocessor
  kStarcore = 58,  // Motorola Start*Core processor
  kMe16 = 59,  // Toyota ME16 processor
  kSt100 = 60,  // STMicroelectronic ST100 processor
  kTinyj = 61,  // Advanced Logic Corp. Tinyj emb.fam
  kX86_64 = 62,  // AMD x86-64 architecture
  kPdsp = 63,  // Sony DSP Processor
  kPdp10 = 64,  // Digital PDP-10
  kPdp11 = 65,  // Digital PDP-11
  kFx66 = 66,  // Siemens FX66 microcontroller
  kSt9plus = 67,  // STMicroelectronics ST9+ 8/16 mc
  kSt7 = 68,  // STmicroelectronics ST7 8 bit mc
  k68hc16 = 69,  // Motorola MC68HC16 microcontroller
  k68hc11 = 70,  // Motorola MC68HC11 microcontroller
  k68hc08 = 71,  // Motorola MC68HC08 microcontroller
  k68hc05 = 72,  // Motorola MC68HC05 microcontroller
  kSvX = 73,  // Silicon Graphics SVx
  kSt19 = 74,  // STMicroelectronics ST19 8 bit mc
  kVax = 75,  // Digital VAX
  kCris = 76,  // Axis Communications 32-bit embedded proc
  kJavelin = 77,  // Infineon Technologies 32-bit embedded proc
  kFirepath = 78,  // Element 14 64-bit DSP Processor
  kZsp = 79,  // LSI Logic 16-bit DSP Processor
  kMmix = 80,  // Donald Knuth's educational 64-bit proc
  kHuany = 81,  // Harvard University machine-independent object files
  kPrism = 82,  // SiTera Prism
  kAvr = 83,  // Atmel AVR 8-bit microcontroller
  kFr30 = 84,  // Fujitsu FR30
  kD10v = 85,  // Mitsubishi D10V
  kD30v = 86,  // Mitsubishi D30V
  kV850 = 87,  // NEC v850
  kM32r = 88,  // Mitsubishi M32R
  kMn10300 = 89,  // Matsushita MN10300
  kMn10200 = 90,  // Matsushita MN10200
  kPj = 91,  // picoJava
  kOpenrisc = 92,  // OpenRISC 32-bit embedded processor
  kArcCompact = 93,  // ARC International ARCompact
  kXtensa = 94,  // Tensilica Xtensa Architecture
  kVideocore = 95,  // Alphamosaic VideoCore
  kTmmGpp = 96,  // Thompson Multimedia General Purpose Proc
  kNs32k = 97,  // National Semi. 32000
  kTpc = 98,  // Tenor Network TPC
  kSnp1k = 99,  // Trebia SNP 1000
  kSt200 = 100,  // STMicroelectronics ST200
  kIp2k = 101,  // Ubicom IP2xxx
  kMax = 102,  // MAX processor
  kCr = 103,  // National Semi. CompactRISC
  kF2mc16 = 104,  // Fujitsu F2MC16
  kMsp430 = 105,  // Texas Instruments msp430
  kBlackfin = 106,  // Analog Devices Blackfin DSP
  kSeC33 = 107,  // Seiko Epson S1C33 family
  kSep = 108,  // Sharp embedded microprocessor
  kArca = 109,  // Arca RISC
  kUnicore = 110,  // PKU-Unity & MPRC Peking Uni. mc series
  kExcess = 111,  // eXcess configurable cpu
  kDxp = 112,  // Icera Semi. Deep Execution Processor
  kAlteraNiOs2 = 113,  // Altera NI OS II
  kCrx = 114,  // National CompactRISC CRX
  kXgate = 115,  // Motorola XGATE
  kC166 = 116,  // Infineon C16x/XC16x
  kM16C = 117,  // Renesas M16C
  kDsPic30F = 118,  // Microchip Technology dsPIC30F
  kCe = 119,  // Freescale Communication Engine RISC
  kM32C = 120,  // Renesas M32C

  kTsk3000 = 131,  // Altium TSK3000
  kRs08 = 132,  // Freescale RS08
  kSharc = 133,  // Analog Devices SHARC family
  kECog2 = 134,  // Cyan Technology eCOG2
  kScore7 = 135,  // Sunplus S+core7 RISC
  kDsp24 = 136,  // New Japan Radio DSP 24-bit
  kVideoCore3 = 137,  // Broadcom VideoCore III
  kLatticeMico32 = 138,  // RISC for Lattice FPGA
  kSeC17 = 139,  // Seiko Epson C17
  kTiC6000 = 140,  // Texas Instruments TMS320C6000 DSP
  kTiC2000 = 141,  // Texas Instruments TMS320C2000 DSP
  kTiC5500 = 142,  // Texas Instruments TMS320C55x DSP
  kTiArp32 = 143,  // Texas Instruments Application Specific RISC
  kTiPru = 144,  // Texas Instruments Programmable Realtime Unit

  kMmDspPlus = 160,  // STMicroelectronics 64bit VLIW DSP
  kCypressM8C = 161,  // Cypress M8C
  kR32C = 162,  // Renesas R32C
  kTriMedia = 163,  // NXP TriMedia
  kQDsp6 = 164,  // QUALCOMM DSP6
  k8051 = 165,  // Intel 8051 and variants
  kStxP7x = 166,  // STMicroelectronics STxP7x
  kNds32 = 167,  // Andes compact code embedded RISC
  kECog1X = 168,  // Cyan Technology eCOG1X
  kMaxQ30 = 169,  // Dallas MAXQ30 mc
  kXimo16 = 170,  // New Japan Radio 16-bit DSP
  kManik = 171,  // M2000 Reconfigurable RISC
  kCrayNv2 = 172,  // Cray NV2 vector architecture
  kRx = 173,  // Renesas RX
  kMetaG = 174,  // Imagination tech META
  kMcstElbrus = 175,  // MCST Elbrus
  kECog16 = 176,  // Cyan Technology eCOG16
  kCr16 = 177,  // National Semi. CompactRISC CR16
  kEtpu = 178,  // Freescale Extended Time Processing Unit
  kSle9X = 179,  // Infineon Tech. SLE9X
  kL10M = 180,  // Intel L10M
  kK10M = 181,  // Intel K10M

  kAarch64 = 183,  // ARM AARCH64

  kAvr32 = 185,  // Amtel 32-bit microprocessor
  kStm8 = 186,  // STMicroelectronics STM8
  kTile64 = 187,  // Tileta TILE64
  kTilePro = 188,  // Tilera TILEPro
  kMicroblaze = 189,  // Xilinx MicroBlaze
  kCuda = 190,  // NVIDIA CUDA
  kTileGx = 191,  // Tilera TILE-Gx
  kCloudshield = 192,  // CloudShield
  kCoreA1st = 193,  // KIPO-KAIST Core-A 1st generation
  kCoreA2nd = 194,  // KIPO-KAIST Core-A 2nd generation
  kArcCompact2 = 195,  // Synopsys ARCompact V2
  kOpen8 = 196,  // Open8 RISC
  kRl78 = 197,  // Renesas RL78
  kVideoCore5 = 198,  // Broadcom VideoCore V
  k78Kor = 199,  // Renesas 78KOR
  k56800Ex = 200,  // Freescale 56800EX DSC
  kBa1 = 201,  // Beyond BA1
  kBa2 = 202,  // Beyond BA2
  kXCore = 203,  // XMOS xCORE
  kMchpPic = 204,  // Microchip 8-bit PIC(r)

  kKm32 = 210,  // KM211 KM32
  kKmx32 = 211,  // KM211 KMX32
  kEmx16 = 212,  // KM211 KMX16
  kEmx8 = 213,  // KM211 KMX8
  kKvarc = 214,  // KM211 KVARC
  kCdp = 215,  // Paneve CDP
  kCoge = 216,  // Cognitive Smart Memory Processor
  kCool = 217,  // Bluechip CoolEngine
  kNorc = 218,  // Nanoradio Optimized RISC
  kCsrKalimba = 219,  // CSR Kalimba
  kZ80 = 220,  // Zilog Z80
  kVisium = 221,  // Controls and Data Services VISIUMcore
  kFt32 = 222,  // FTDI Chip FT32
  kMoxie = 223,  // Moxie processor
  kAmdGpu = 224,  // AMD GPU

  kRiscv = 243,  // RISC-V

  kBpf = 247,  // Linux BPF
  kCSky = 252,  // C-SKY
  kNum = 253,

  kArcA5 = kArcCompact,

  kAlpha = 0x9026,
};

/// Deserialized data structure representing the ELF header.
/// It does not reflect the layout of the header in disk.
struct Hdr {
  /// ELF object class
  ElfClass elf_class;

  /// Endianness of the data in the ELF file.
  Endianness endiannes;

  /// Heder version.
  uint8_t header_version;

  /// OS ABI
  OsAbi os_abi;

  /// Object file type
  Type type;

  /// Architecture
  Machine machine;

  /// ELF file version
  uint32_t version;

  /// Entry point virtual address
  uint64_t entry;

  /// Program header table file offset
  uint64_t phoff;

  /// Section header table file offset
  uint64_t shoff;

  /// Processor-specific flags
  uint32_t flags;

  /// ELF header size in bytes
  uint16_t ehsize;

  /// Program header table entry size
  uint16_t phentsize;

  /// Program header table entry count
  uint16_t phnum;

  /// Section header table entry size
  uint16_t shentsize;

  /// Section header table entry count
  uint16_t shnum;

  /// Section header string table index
  uint16_t shstrndx;
};

enum class SectionType : uint32_t {
  /// Section header table entry unused
  kNull = 0,

  /// Program data
  kProgBits = 1,

  /// Symbol table
  kSymTab = 2,

  /// String table
  kStrTab = 3,

  /// Relocation entries with addends
  kRela = 4,

  /// Symbol hash table
  kHash = 5,

  /// Dynamic linking information
  kDynamic = 6,

  /// Notes
  kNote = 7,

  /// Program space with no data (typically the .bss seciton)
  kNoBits = 8,

  /// Relocation entries (with no addends)
  kRel = 9,

  kShLib = 10,

  /// Dynamic linker symbol table
  kDynSym = 11,

  /// Array of constructors
  kInitArray = 14,

  /// Array of destructors
  kFiniArray = 15,

  /// Array of pre-constructors
  kPreInitArray = 16,

  /// Section group
  kGroup = 17,

  /// Extended section indeces
  kSymTabShNdx = 18,

  /// OS-specific section types start with value 0x6000'0000

  /// Object attributes.
  kGnuAttributes = 0x6ffffff5,

  /// GNU-style hash table.
  kGnuHash = 0x6ffffff6,

  /// Prelink library list
  kGnuLibList = 0x6ffffff7,

  /// Checksum for DSO content.
  kChecksum = 0x6ffffff8,

  /// Sun-specific
  kSunwMove = 0x6ffffffa,
  kSunwComdat = 0x6ffffffb,
  kSunwSymInfo = 0x6ffffffc,

  /// Version definition section.
  kGnuVersionDef = 0x6ffffffd,

  /// Version needs section.
  kGnuVersionNeeds = 0x6ffffffe,

  /// Version symbol table.
  kGnuVersionSym = 0x6fffffff,

  /// OS-specific section types end with value 0x6fff'ffff

  /// Processor-specific section types start with 0x7000'0000

  /// Processor-specific section types end with 0x7fff'ffff

  /// Application-specific section types start with 0x8000'0000

  /// Application-specific section types end with 0x8fff'ffff
};

struct SectionHdr {
  /// Section name (string tbl index)
  uint32_t name;

  /// Section type
  SectionType type;

  /// Section flags
  uint32_t flags;

  /// Section virtual addr at execution
  uint64_t addr;

  /// Section file offset
  uint64_t offset;

  /// Section size in bytes
  uint32_t size;

  /// Link to another section
  uint32_t link;

  /// Additional section information
  uint32_t info;

  /// Section alignment
  uint32_t addralign;

  /// Entry size if section holds table
  uint32_t entsize;
};

class Elf {
 public:
  Elf(const Elf&) = delete;
  Elf(Elf&&) = delete;
  Elf& operator=(const Elf&) = delete;
  Elf& operator=(Elf&&) = delete;

  static Result<Elf> Load(const std::string& path);
  static Result<Elf> Load(const std::filesystem::path& path);

 private:
  Elf() = default;
};

inline Result<Elf> Elf::Load(const std::string& path) {
  return Load(std::filesystem::path(path));
}

inline Result<Elf> Elf::Load(const std::filesystem::path& path) {
  if (!std::filesystem::exists(path)) {
    return Error(ErrorCode::kFileNotFound);
  }
  return Elf();
}

}  // namespace nx::elf

#endif  // NX_ELF_ELF_H__
