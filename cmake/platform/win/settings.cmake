# Platform-specfic options
option(USE_MYSQL_SOURCES "Use included MySQL-sources to build libraries" 1)

# Package overloads
set(BZIP2_LIBRARIES "bzip2")
set(ZLIB_LIBRARIES "zlib")

if( USE_MYSQL_SOURCES )
  set(MYSQL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dep/mysqllite/include)
  set(MYSQL_LIBRARY "libmysql")
  set( MYSQL_FOUND 1 )
  message(STATUS "Using supplied MySQL sources")
endif()

# check the CMake preload parameters (commented out by default)

# overload CMAKE_INSTALL_PREFIX if not being set properly
#if( WIN32 )
#  if( NOT CYGWIN )
#    if( NOT CMAKE_INSTALL_PREFIX )
#      set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/bin")
#    endif()
#  endif()
#endif()

message(STATUS "Using compiler: ${CMAKE_CXX_COMPILER_ID}")

if (CMAKE_CXX_PLATFORM_ID MATCHES "MinGW")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/mingw/settings.cmake)
else()
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/msvc/settings.cmake)
endif()
