
function(add_configuration_type config_type)
  if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "debug;release" CACHE STRING "" FORCE)
  endif()
  if (CMAKE_CONFIGURATION_TYPES)
    list(APPEND            CMAKE_CONFIGURATION_TYPES ${config_type})
    list(REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES)
    set(
      CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}"
      CACHE STRING "Specialized configurations"
      FORCE
    )
  endif()
endfunction()

add_configuration_type(debug_trace)
add_configuration_type(release_trace)

add_configuration_type(debug_v1)
add_configuration_type(debug_v2)

# set(VIRTUAL_TRANSPORT_LIBRARY_DV1 vt-debug_v1)
# set(VIRTUAL_TRANSPORT_LIBRARY_DV2 vt-debug_v2)
# set(VIRTUAL_TRANSPORT_LIBRARY_REL vt-release)

if (${VT_DEBUG_FAST})
  set(VT_DEBUG_MODE_ON 0)
else()
  set(VT_DEBUG_MODE_ON 1)
endif()

set(
  cmake_vt_debug_modes_all
  "gen, runtime, active, term, termds, barrier, pipe,   \
   pool, reduce, rdma, rdma_channel, handler,           \
   hierlb, scatter, serial_msg, trace,                  \
   location, lb, vrt_coll, group, broadcast, flush"
)

if (${vt_detector_disabled})
  message(STATUS "Building VT with detector disabled")
  set(cmake_vt_detector " ")
else()
  set(cmake_vt_detector "detector, ")
endif()

if (${vt_lb_enabled})
  message(STATUS "Building VT with load balancing enabled")
  set(cmake_vt_lb "lblite, ")
else()
  set(cmake_vt_lb " ")
endif()

if (${vt_trace_enabled})
  message(STATUS "Building VT with tracing enabled")
  set(cmake_vt_trace "trace_enabled ")
else()
  set(cmake_vt_trace " ")
endif()

if (VT_THREADING_BACKEND STREQUAL "openmp")
  set(feature_threading "openmp, ")
elseif (VT_THREADING_BACKEND STREQUAL "stdthread")
  set(feature_threading "stdthread, ")
elseif (VT_THREADING_BACKEND STREQUAL "none")
  set(feature_threading " ")
else()

endif()

set(cmake_vt_debug_modes_debug_trace           "${cmake_vt_debug_modes_all}")
set(cmake_vt_debug_modes_release_trace         "flush")
set(cmake_vt_debug_modes_debug                 "${cmake_vt_debug_modes_all}")
set(cmake_vt_debug_modes_release               "flush")
set(cmake_vt_features_debug_trace              "${cmake_vt_detector} ${feature_threading} ${cmake_vt_lb} trace_enabled")
set(cmake_vt_features_release_trace            "${cmake_vt_detector} ${feature_threading} ${cmake_vt_lb} trace_enabled")
set(cmake_vt_features_debug_v2                 "${cmake_vt_detector} ${feature_threading} ${cmake_vt_lb} ${cmake_vt_trace}")
set(cmake_vt_features_debug_v1                 "${cmake_vt_detector} ${feature_threading} ${cmake_vt_lb} ${cmake_vt_trace}")
set(cmake_vt_features_debug                    "${cmake_vt_detector} ${feature_threading} ${cmake_vt_lb} ${cmake_vt_trace}")
set(cmake_vt_features_release                  "${cmake_vt_detector} ${feature_threading} ${cmake_vt_lb} ${cmake_vt_trace}")
set(cmake_config_debug_enabled_debug_trace     1)
set(cmake_config_debug_enabled_release_trace   0)
set(cmake_config_debug_enabled_debug_v2        1)
set(cmake_config_debug_enabled_debug_v1        1)
set(cmake_config_debug_enabled_debug           ${VT_DEBUG_MODE_ON})
set(cmake_config_debug_enabled_release         0)

#message(STATUS "build types=${CMAKE_CONFIGURATION_TYPES}")

set(build_type_list)

foreach(cur_build_type ${CMAKE_CONFIGURATION_TYPES})
  #message(STATUS "generating for build type=${cur_build_type}")

  set(
    cmake_vt_debug_modes
    ${cmake_vt_debug_modes_${cur_build_type}}
  )

  set(
    cmake_vt_features
    ${cmake_vt_features_${cur_build_type}}
  )

  set(
    cmake_config_debug_enabled
    ${cmake_config_debug_enabled_${cur_build_type}}
  )

  configure_file(
    ${PROJECT_BASE_DIR}/cmake_config.h.in
    ${PROJECT_BIN_DIR}/${cur_build_type}/cmake_config.h @ONLY
  )

  install(
    FILES            "${PROJECT_BINARY_DIR}/${cur_build_type}/cmake_config.h"
    DESTINATION      include
    CONFIGURATIONS   ${cur_build_type}
  )

  string(TOUPPER ${cur_build_type} cur_build_type_upper)

  set(CMAKE_${cur_build_type_upper}_POSTFIX "-${cur_build_type}")

  list(APPEND build_type_list ${cur_build_type})

  set_target_properties(
    ${VIRTUAL_TRANSPORT_LIBRARY}
    PROPERTIES ${cur_build_type_upper}_POSTFIX "-${cur_build_type}"
  )
endforeach()

# message(STATUS "build type list=${build_type_list}")
# message(STATUS "build type=${CMAKE_BUILD_TYPE}")

target_include_directories(
  ${VIRTUAL_TRANSPORT_LIBRARY} PUBLIC
  $<BUILD_INTERFACE:$<$<CONFIG:debug>:${PROJECT_BIN_DIR}/debug>>
  $<BUILD_INTERFACE:$<$<CONFIG:debug_v1>:${PROJECT_BIN_DIR}/debug_v1>>
  $<BUILD_INTERFACE:$<$<CONFIG:debug_v2>:${PROJECT_BIN_DIR}/debug_v2>>
  $<BUILD_INTERFACE:$<$<CONFIG:release>:${PROJECT_BIN_DIR}/release>>
  $<BUILD_INTERFACE:$<$<CONFIG:debug_trace>:${PROJECT_BIN_DIR}/debug_trace>>
  $<BUILD_INTERFACE:$<$<CONFIG:release_trace>:${PROJECT_BIN_DIR}/release_trace>>
  $<INSTALL_INTERFACE:include>
)

set(
  CXX_FLAGS_STRICT
  "-Wpedantic;"
  "-Wall;"
  "-Wabi;"
  "-Wno-unknown-pragmas;"
  "-Wno-gnu-zero-variadic-macro-arguments;"
  "-Wno-variadic-macros;"
  "-Wno-extra-semi;"
)

set(
  LINKER_FLAGS_STRING
  "-Wl;"
  "--warn-unresolved-symbols;"
  "--warn-once;"
)

target_compile_options(
  ${VIRTUAL_TRANSPORT_LIBRARY} PUBLIC
  $<$<CONFIG:debug_v2>:${CXX_FLAGS_STRICT}>
)
