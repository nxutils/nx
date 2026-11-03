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

#include <errno.h>
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <system_error>
#include <unistd.h>

#include "nx/base/mapped_file.h"

namespace nx {

MappedFile::~MappedFile() {
  UnMap();
}

void MappedFile::UnMap() {
  if (data_ == nullptr) {
    return;
  }
  munmap(data_, size_);
}

Result<MappedFile, std::string> MappedFile::Load(
    const std::filesystem::path& path) {
  std::error_code ec;

  if (!std::filesystem::exists(path, ec)) {
    return "File " + path.native() + " does not exist" +
           (ec.value() ? ": " + ec.message() : "");
  }

  size_t size = std::filesystem::file_size(path, ec);
  if (ec) {
    return "Error fetching the size of the file " + path.native() + ": " + ec.message();
  }

  std::string path_str = path.native();
  int fd = open(path_str.c_str(), O_RDONLY);
  if (fd < 0) {
    auto error_code = errno;
    errno = 0;
    return "Error opening file " + path_str + ": '" + strerrorname_np(error_code) + "' "
        + strerror(error_code);
  }

  void *res = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);

  if (res == MAP_FAILED) {
    auto error_code = errno;
    errno = 0;
    return "Error mapping file " + path_str + ": '" + strerrorname_np(error_code) + "' "
        + strerror(error_code);
  }

  return MappedFile(reinterpret_cast<uint8_t*>(res), size);
}

}  // namespace nx
