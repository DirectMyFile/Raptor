set(
  LLVM_TOOLCHAIN
  ""
  CACHE STRING
  "LLVM Toolchain"
)

if(NOT LLVM_TOOLCHAIN STREQUAL "")
  set(LLVM_FULL_PREFIX "${LLVM_TOOLCHAIN}/bin/")
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER "${LLVM_FULL_PREFIX}clang")
set(CMAKE_CXX_COMPILER "${LLVM_FULL_PREFIX}clang++")

set(
  CMAKE_LINKER
  "${LLVM_FULL_PREFIX}clang"
  CACHE FILEPATH "Executable Linker" FORCE
)

set(
  CMAKE_EXE_LINKER
  "${LLVM_FULL_PREFIX}clang"
  CACHE FILEPATH "Executable Linker" FORCE
)

set(
  CMAKE_EXE_LINKER_FLAGS
  "-B${LLVM_FULL_PREFIX} -fuse-ld=lld"
  CACHE STRING "Linker Flags" FORCE
)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
