# Rules for repo wide code style and idioms

## Build system

Bazel is the build system used by everything in the repo without exceptions.
See https://bazel.build/ for information about bazel. Execute all bazel commands
from repo root directory.

## License

The source files should have the following license header adjusted for the
comment style suitable for the programming language in use:

```
/******************************************************************************/
/*                                                                            */
/*  Copyright {YYYY} NX Utils Foundation                                      */
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
```

The `{YYYY}` string should be replaced by the year in which the file is created.

## Code style

The repo has source code in various languages. The style employed for each
language is as follows:

* C++ - Use Google C++ style: https://google.github.io/styleguide/cppguide.html.
  There is one exception: For C++ components defined in the `nx/base` directory,
  the fully qualified namespace should not include the nested `base` component.
  To ensure that a C++ source file follows the C++ style, run the following:
  ```
  bazel build @llvm_toolchain//:clang-format
  ./bazel-bin/external/toolchains_llvm++llvm+llvm_toolchain/clang-format -i -style Google relative/path/to/file
  ```
* Python - Use PEP 8 style with one exception.
  Link to PEP 8: https://peps.python.org/pep-0008/. Exception: Limit line
  lengths to 80 chars instead of the 79 required by PEP 8.
* Markdown - Only rule is to limit line length to 80 chars. Exceptions to this
  rule are allowed only for listing links/URLs and shell commands.

## C++ idioms

* Do not use exceptions. If there is dependency on code that uses exceptions,
  then calls to functions/methods which can throw exceptions should be wrapped
  with try-catch blocks. The error reported by the exception should be
  immediately converted to `nx::Result` value.
* Prefer using early return over nested conditionals.
* Return values from functions instead of using output arguments. See
  https://abseil.io/tips/176 for more information.
* Use the `Result` type from `nx/base/result.h` to return success values or
  errors.
* Do not use `std::shared_ptr` values unless there is no other alternate. If
  `std::shared_ptr` has to be used, then clearly explain why in the form of
  inline code comments and in the commit messages.

## Commit message style

A git commit message should include various items which have to be formatted
and listed in the following order.

* One line summary prefixed with `[<area>]` where `area` is the name of the
  most nested directory touched by the change. If there are multiple directories
  touched by a change, pick the name for the directory which contains the most
  important or the core of the change. The one line summary, including the
  prefix `[<area>]`, should be at most 80 chars long. It should be as short as
  possible while not compromising the information it is trying to convey.
* Blank line.
* A brief description of the change in no more than five sentences.
* Blank line.
* A section titled "TESTED" under which each test command that was run to test
  the change should be listed.
* Blank line.
* An optional section titled "STYLE EXCEPTIONS". This section should be listed
  only if the change does not adhere to the required style or deviates from
  allowed idioms.
