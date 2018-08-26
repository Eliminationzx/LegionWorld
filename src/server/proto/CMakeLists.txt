# Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if (MSVC)
  add_definitions(/W0)
else()
  add_definitions(-w)
endif()

CollectSourceFiles(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE_SOURCES
  # Exclude
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)

if (USE_COREPCH)
  set(PRIVATE_PCH_HEADER PrecompiledHeaders/protoPCH.h)
  set(PRIVATE_PCH_SOURCE PrecompiledHeaders/protoPCH.cpp)
endif()

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/dep
    ${CMAKE_SOURCE_DIR}/dep/protobuf/src
    ${CMAKE_SOURCE_DIR}/src/server/shared
    ${CMAKE_SOURCE_DIR}/src/server/shared/Logging
    ${CMAKE_SOURCE_DIR}/src/server/shared/Utilities
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders
    ${CMAKE_CURRENT_SOURCE_DIR}/RealmList
    ${CMAKE_CURRENT_SOURCE_DIR}/Login
    ${CMAKE_CURRENT_SOURCE_DIR}/Client
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(proto STATIC
  ${PRIVATE_SOURCES}
  ${PRIVATE_PCH_SOURCE}
)

target_link_libraries(proto
    shared
    protobuf
    rapidjson
    cppformat
    )

CollectIncludeDirectories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_INCLUDES
  # Exclude
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)

set_target_properties(proto
    PROPERTIES
      FOLDER
        "server")

# Generate precompiled header
if (USE_COREPCH)
  add_cxx_pch(proto ${PRIVATE_PCH_HEADER} ${PRIVATE_PCH_SOURCE})
endif()
