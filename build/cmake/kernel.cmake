set(KERNEL_DIR "${RAPTOR_DIR}/kernel")
set(KERNEL_COMMON_TYPES "")

file(GLOB KERNEL_SUBDIRS RELATIVE
  "${KERNEL_DIR}"
  "${KERNEL_DIR}/*"
)

foreach(SUBDIR ${KERNEL_SUBDIRS})
  if (IS_DIRECTORY "${KERNEL_DIR}/${SUBDIR}")
    if(NOT SUBDIR STREQUAL "arch")
      list(APPEND KERNEL_COMMON_TYPES ${SUBDIR})
    endif()
  endif()
endforeach()

file(GLOB KERNEL_COMMON_SRC
  "${KERNEL_DIR}/*.c"
  "${KERNEL_DIR}/*.h"
  "${KERNEL_DIR}/*.cpp"
  "${KERNEL_DIR}/*.hpp"
)

foreach(COMMON_TYPE ${KERNEL_COMMON_TYPES})
  file(GLOB_RECURSE COMMON_TYPE_SRC
    "${KERNEL_DIR}/${COMMON_TYPE}/*.c"
    "${KERNEL_DIR}/${COMMON_TYPE}/*.h"
    "${KERNEL_DIR}/${COMMON_TYPE}/*.s"
    "${KERNEL_DIR}/${COMMON_TYPE}/*.cpp"
    "${KERNEL_DIR}/${COMMON_TYPE}/*.hpp"
  )

  list(APPEND KERNEL_COMMON_SRC ${COMMON_TYPE_SRC})
endforeach()

option(DEBUG_KERNEL_LOCKS "Enable Kernel Lock Debugging" OFF)
if(DEBUG_KERNEL_LOCKS)
  add_definitions(
    -DDEBUG_SPINLOCKS
  )
endif()

option(DEBUG_RKMALLOC_DISABLE_MAGIC "Disable Allocation Magic Usage" OFF)
if(DEBUG_RKMALLOC_DISABLE_MAGIC)
  add_definitions(
    -DRKMALLOC_DISABLE_MAGIC
  )
endif()

option(DEBUG_RKMALLOC_DISABLE_SITTING "Disable Allocation Sitting" OFF)
if(DEBUG_RKMALLOC_DISABLE_SITTING)
  add_definitions(
    -DRKMALLOC_DISABLE_SITTING
  )
endif()
