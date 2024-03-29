# Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

########### bnetserver ###############

file(GLOB_RECURSE sources_rest REST/*.cpp REST/*.h)
file(GLOB_RECURSE sources_server Server/*.cpp Server/*.h)
file(GLOB_RECURSE sources_services Services/*.cpp Services/*.h)
file(GLOB sources_localdir *.cpp *.h)

if (USE_COREPCH)
  set(bnetserver_PCH_HDR PrecompiledHeaders/bnetPCH.h)
  set(bnetserver_PCH_SRC PrecompiledHeaders/bnetPCH.cpp)
endif()

set(bnetserver_SRCS
  ${bnetserver_SRCS}
  ${sources_rest}
  ${sources_server}
  ${sources_services}
  ${sources_localdir}
)

if( WIN32 )
  set(bnetserver_SRCS
    ${bnetserver_SRCS}
    ${sources_windows_Debugging}
  )
  if ( MSVC )
    set(bnetserver_SRCS
      ${bnetserver_SRCS}
      bnetserver.rc
    )
  endif ()
endif()

# GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    ${Boost_LIBRARIES}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/dep
    ${CMAKE_SOURCE_DIR}/dep/cds
    ${CMAKE_SOURCE_DIR}/dep/cppformat
    ${CMAKE_SOURCE_DIR}/dep/zlib
    ${CMAKE_SOURCE_DIR}/dep/mysqllite
    ${CMAKE_SOURCE_DIR}/dep/gsoap
    ${CMAKE_SOURCE_DIR}/dep/protobuf/src
    ${CMAKE_SOURCE_DIR}/src/server/proto
    ${CMAKE_SOURCE_DIR}/src/server/proto/Client
    ${CMAKE_SOURCE_DIR}/src/server/proto/Login
    ${CMAKE_SOURCE_DIR}/src/server/proto/RealmList
    ${CMAKE_SOURCE_DIR}/src/server/shared
    ${CMAKE_SOURCE_DIR}/src/server/shared/Configuration
    ${CMAKE_SOURCE_DIR}/src/server/shared/Database
    ${CMAKE_SOURCE_DIR}/src/server/shared/Debugging
    ${CMAKE_SOURCE_DIR}/src/server/shared/Packets
    ${CMAKE_SOURCE_DIR}/src/server/shared/Cryptography
    ${CMAKE_SOURCE_DIR}/src/server/shared/Cryptography/Authentication
    ${CMAKE_SOURCE_DIR}/src/server/shared/Logging
    ${CMAKE_SOURCE_DIR}/src/server/shared/Networking
    ${CMAKE_SOURCE_DIR}/src/server/shared/Realm
    ${CMAKE_SOURCE_DIR}/src/server/shared/Threading
    ${CMAKE_SOURCE_DIR}/src/server/shared/Utilities
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Server
    ${CMAKE_CURRENT_SOURCE_DIR}/Services
    ${CMAKE_CURRENT_SOURCE_DIR}/REST
    ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders
    ${MYSQL_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

add_executable(bnetserver
  ${bnetserver_SRCS}
  ${bnetserver_PCH_SRC}
)

add_dependencies(bnetserver revision.h)

if (NOT WIN32)
  set_target_properties(bnetserver PROPERTIES
    COMPILE_DEFINITIONS _TRINITY_BNET_CONFIG="${CONF_DIR}/bnetserver.conf"
  )
endif()

target_link_libraries(bnetserver
    shared
    gsoap
    protobuf
    proto
    rapidjson
    cppformat
    cds
    ${MYSQL_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${ZLIB_LIBRARIES}
)

# CollectIncludeDirectories(
  # ${CMAKE_CURRENT_SOURCE_DIR}
  # ${CMAKE_CURRENT_SOURCE_DIR}/REST
  # PUBLIC_INCLUDES
  # # Exclude
  # ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)


# set_target_properties(bnetserver
    # PROPERTIES
      # FOLDER
        # "server")

if (WIN32)
  if (MSVC)
    add_custom_command(TARGET bnetserver
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/bnetserver.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/bnetserver.cert.pem ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/bnetserver.key.pem ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
    )
  elseif (MINGW)
    add_custom_command(TARGET bnetserver
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/bnetserver.conf.dist ${CMAKE_BINARY_DIR}/bin/
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/bnetserver.cert.pem ${CMAKE_BINARY_DIR}/bin/
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/bnetserver.key.pem ${CMAKE_BINARY_DIR}/bin/
    )
  endif()
endif()

if (UNIX)
  install(TARGETS bnetserver DESTINATION bin)
  install(FILES bnetserver.conf.dist DESTINATION ${CONF_DIR})
  install(FILES bnetserver.cert.pem bnetserver.key.pem DESTINATION bin)
elseif (WIN32)
  install(TARGETS bnetserver DESTINATION "${CMAKE_INSTALL_PREFIX}")
  install(FILES bnetserver.conf.dist DESTINATION "${CMAKE_INSTALL_PREFIX}")
  install(FILES bnetserver.cert.pem bnetserver.key.pem DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()

# Generate precompiled header
if (USE_COREPCH)
  add_cxx_pch(bnetserver ${bnetserver_PCH_HDR} ${bnetserver_PCH_SRC})
endif()
