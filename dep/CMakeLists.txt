# Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if( CMAKE_COMPILER_IS_GNUCXX )
  add_definitions(--no-warnings)
elseif( MSVC )
  add_definitions(/W0)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  if(SERVERS)
    if(USE_MYSQL_SOURCES)
      add_subdirectory(mysqllite)
    endif()
  endif()
  if(TOOLS)
    add_subdirectory(bzip2)
  endif()
  add_subdirectory(zlib)
endif()

add_subdirectory(g3dlite)
add_subdirectory(recastnavigation)
add_subdirectory(cppformat)

if(SERVERS)
  add_subdirectory(gsoap)
  add_subdirectory(rapidjson)
endif()

add_subdirectory(protobuf)
add_subdirectory(cds)
# add_subdirectory(protobuf_decompiler)
# add_subdirectory(protoc_bnet)

  
if(TOOLS)
  add_subdirectory(CascLib)
endif()
