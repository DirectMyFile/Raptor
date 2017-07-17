option(TESTS "Enable Tests" OFF)

function(cflags)
  set(ARGLIST "")
  foreach(ARG ${ARGV})
    set(ARGLIST "${ARGLIST} ${ARG}")
  endforeach()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARGLIST}" PARENT_SCOPE)
  set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${ARGLIST}" PARENT_SCOPE)
endfunction()

function(ldflags)
  set(ARGLIST "")
  foreach(ARG ${ARGV})
    set(ARGLIST "${ARGLIST} ${ARG}")
  endforeach()
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ARGLIST}" PARENT_SCOPE)
endfunction()

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(CLANG ON)
endif()

cflags(-DRAPTOR)

include(${RAPTOR_DIR}/build/cmake/config.cmake)
