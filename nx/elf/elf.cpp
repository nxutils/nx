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

#include "nx/elf/elf.h"

#include <sys/mman.h>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <system_error>

#include "nx/base/mapped_file.h"
#include "nx/elf/result.h"

namespace nx::elf {

Result<Elf> Elf::Load(const std::filesystem::path& path) {
  std::string path_str = path.native();
  auto const kInvalidElfError =
      Error{ErrorCode::kInvalidElfFile, path_str + " is not a valid ELF file"};

  std::error_code ec;

  if (!std::filesystem::exists(path, ec)) {
    return Error{ErrorCode::kFileNotFound,
                 "File " + path.native() + " not found" +
                     (ec.value() ? ": " + ec.message() : "")};
  }

  auto mapped_file_or_error = MappedFile::Load(path);
  if (!mapped_file_or_error.has_value()) {
    auto& error = mapped_file_or_error.error();
    return Error{ErrorCode::kFileLoadFailed,
                 "Loading " + path_str + " failed: " + error};
  }
  auto mapped_file = std::move(mapped_file_or_error.value());

  auto data = mapped_file.data();
  if (data.size() < sizeof(kMagic)) {
    return Error{ErrorCode::kNotElfFile, path_str + " is not an ELF file"};
  }
  auto magic_data = data.subspan<0, sizeof(kMagic)>();
  if (std::memcmp(magic_data.data(), kMagic, sizeof(kMagic)) != 0) {
    return Error{ErrorCode::kNotElfFile, path_str + " is not an ELF file"};
  }

  data = data.subspan<sizeof(kMagic)>();
  constexpr size_t kIdentSize = 12;
  if (data.size() < kIdentSize) {
    return kInvalidElfError;
  }

  Hdr elf_hdr;

  elf_hdr.elf_class = static_cast<ElfClass>(data[0]);
  elf_hdr.endianness = static_cast<Endianness>(data[1]);
  elf_hdr.header_version = data[2];
  elf_hdr.os_abi = static_cast<OsAbi>(data[3]);

  data = data.subspan<kIdentSize>();
  if (data.size() < sizeof(Type)) {
    return kInvalidElfError;
  }
  elf_hdr.type = *reinterpret_cast<Type*>(data.data());

  data = data.subspan(sizeof(Type));
  if (data.size() < sizeof(Machine)) {
    return kInvalidElfError;
  }
  elf_hdr.machine = *reinterpret_cast<Machine*>(data.data());

  data = data.subspan(sizeof(Machine));
  if (data.size() < sizeof(uint32_t)) {
    return kInvalidElfError;
  }
  elf_hdr.version = *reinterpret_cast<uint32_t*>(data.data());

  data = data.subspan<sizeof(uint32_t)>();
  if (elf_hdr.elf_class == ElfClass::k32) {
    if (data.size() < sizeof(uint32_t) * 3) {
      return kInvalidElfError;
    }

    elf_hdr.entry = *reinterpret_cast<uint32_t*>(data.data());
    data = data.subspan<sizeof(uint32_t)>();

    elf_hdr.phoff = *reinterpret_cast<uint32_t*>(data.data());
    data = data.subspan<sizeof(uint32_t)>();

    elf_hdr.shoff = *reinterpret_cast<uint32_t*>(data.data());
    data = data.subspan<sizeof(uint32_t)>();
  } else if (elf_hdr.elf_class == ElfClass::k64) {
    if (data.size() < sizeof(uint64_t) * 3) {
      return kInvalidElfError;
    }

    elf_hdr.entry = *reinterpret_cast<uint64_t*>(data.data());
    data = data.subspan<sizeof(uint64_t)>();

    elf_hdr.phoff = *reinterpret_cast<uint64_t*>(data.data());
    data = data.subspan<sizeof(uint64_t)>();

    elf_hdr.shoff = *reinterpret_cast<uint64_t*>(data.data());
    data = data.subspan<sizeof(uint64_t)>();
  } else {
    return Error{ErrorCode::kInvalidElfFile,
                 "Invalid ELF class; Expected Elf32 or Elf64"};
  }

  // The remaining fields in the Hdr are sized exactly like how they
  // would exist on disk for both ELF32 and ELF64. Hence, check for the
  // size once and read them all in one go.

  if (data.size() < (6 * sizeof(uint16_t) + sizeof(uint32_t))) {
    // One uint32_t field:
    //   flags
    // Six uint16_t fields:
    //   ehsize
    //   phentsize
    //   phnum
    //   shentsize
    //   shnum
    //   shstrndx
    return kInvalidElfError;
  }

  elf_hdr.flags = *reinterpret_cast<uint32_t*>(data.data());
  data = data.subspan<sizeof(uint32_t)>();

  elf_hdr.ehsize = *reinterpret_cast<uint16_t*>(data.data());
  data = data.subspan<sizeof(uint16_t)>();

  elf_hdr.phentsize = *reinterpret_cast<uint16_t*>(data.data());
  data = data.subspan<sizeof(uint16_t)>();

  elf_hdr.phnum = *reinterpret_cast<uint16_t*>(data.data());
  data = data.subspan<sizeof(uint16_t)>();

  elf_hdr.shentsize = *reinterpret_cast<uint16_t*>(data.data());
  data = data.subspan<sizeof(uint16_t)>();

  elf_hdr.shnum = *reinterpret_cast<uint16_t*>(data.data());
  data = data.subspan<sizeof(uint16_t)>();

  elf_hdr.shstrndx = *reinterpret_cast<uint16_t*>(data.data());
  data = data.subspan<sizeof(uint16_t)>();

  return Elf(std::move(elf_hdr), std::move(mapped_file));
}

}  // namespace nx::elf
