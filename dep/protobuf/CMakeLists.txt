# Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

set(protobuf_STAT_SRCS
  # src/google/protobuf/compiler/code_generator.cc
  # src/google/protobuf/compiler/command_line_interface.h
  src/google/protobuf/compiler/importer.cc
  src/google/protobuf/compiler/parser.h
  # src/google/protobuf/compiler/plugin.h
  # src/google/protobuf/compiler/subprocess.h
  # src/google/protobuf/compiler/zip_writer.cc
  # src/google/protobuf/compiler/code_generator.h
  src/google/protobuf/compiler/importer.h
  # src/google/protobuf/compiler/main.cc
  # src/google/protobuf/compiler/package_info.h
  # src/google/protobuf/compiler/plugin.pb.cc
  # src/google/protobuf/compiler/zip_writer.h
  # src/google/protobuf/compiler/command_line_interface.cc
  src/google/protobuf/compiler/parser.cc
  # src/google/protobuf/compiler/plugin.cc
  # src/google/protobuf/compiler/plugin.pb.h
  # src/google/protobuf/compiler/plugin.proto
  # src/google/protobuf/compiler/subprocess.cc
  # src/google/protobuf/compiler/cpp/cpp_enum_field.cc
  # src/google/protobuf/compiler/cpp/cpp_extension.h
  # src/google/protobuf/compiler/cpp/cpp_file.cc
  # src/google/protobuf/compiler/cpp/cpp_generator.h
  # src/google/protobuf/compiler/cpp/cpp_message.cc
  # src/google/protobuf/compiler/cpp/cpp_message_field.h
  # src/google/protobuf/compiler/cpp/cpp_primitive_field.cc
  # src/google/protobuf/compiler/cpp/cpp_service.h
  # src/google/protobuf/compiler/cpp/cpp_enum.cc
  # src/google/protobuf/compiler/cpp/cpp_enum_field.h
  # src/google/protobuf/compiler/cpp/cpp_field.cc
  # src/google/protobuf/compiler/cpp/cpp_file.h
  # src/google/protobuf/compiler/cpp/cpp_helpers.cc
  # src/google/protobuf/compiler/cpp/cpp_message.h
  # src/google/protobuf/compiler/cpp/cpp_options.h
  # src/google/protobuf/compiler/cpp/cpp_primitive_field.h
  # src/google/protobuf/compiler/cpp/cpp_string_field.cc
  # src/google/protobuf/compiler/cpp/cpp_enum.h
  # src/google/protobuf/compiler/cpp/cpp_extension.cc
  # src/google/protobuf/compiler/cpp/cpp_field.h
  # src/google/protobuf/compiler/cpp/cpp_generator.cc
  # src/google/protobuf/compiler/cpp/cpp_helpers.h
  # src/google/protobuf/compiler/cpp/cpp_message_field.cc
  # src/google/protobuf/compiler/cpp/cpp_service.cc
  # src/google/protobuf/compiler/cpp/cpp_string_field.h
  # src/google/protobuf/descriptor.proto
  src/google/protobuf/descriptor.cc
  src/google/protobuf/descriptor.pb.cc
  src/google/protobuf/descriptor_database.cc
  src/google/protobuf/dynamic_message.cc
  src/google/protobuf/extension_set.cc
  src/google/protobuf/extension_set_heavy.cc
  src/google/protobuf/generated_message_reflection.cc
  src/google/protobuf/generated_message_util.cc
  src/google/protobuf/io/coded_stream.cc
  src/google/protobuf/io/gzip_stream.cc
  src/google/protobuf/io/printer.cc
  src/google/protobuf/io/strtod.cc
  src/google/protobuf/io/tokenizer.cc
  src/google/protobuf/io/zero_copy_stream.cc
  src/google/protobuf/io/zero_copy_stream_impl.cc
  src/google/protobuf/io/zero_copy_stream_impl_lite.cc
  src/google/protobuf/message.cc
  src/google/protobuf/message_lite.cc
  src/google/protobuf/reflection_ops.cc
  src/google/protobuf/repeated_field.cc
  src/google/protobuf/service.cc
  src/google/protobuf/stubs/common.cc
  src/google/protobuf/stubs/once.cc
  src/google/protobuf/stubs/stringprintf.cc
  src/google/protobuf/stubs/structurally_valid.cc
  src/google/protobuf/stubs/strutil.cc
  src/google/protobuf/stubs/substitute.cc
  src/google/protobuf/text_format.cc
  src/google/protobuf/unknown_field_set.cc
  src/google/protobuf/wire_format.cc
  src/google/protobuf/wire_format_lite.cc
)

SET(HEADERS
  src/google/protobuf/compiler/importer.h
  src/google/protobuf/compiler/parser.h
  src/google/protobuf/descriptor.pb.h
  src/google/protobuf/dynamic_message.h
  src/google/protobuf/generated_enum_reflection.h
  src/google/protobuf/generated_message_util.h
  src/google/protobuf/message_lite.h
  src/google/protobuf/repeated_field.h
  src/google/protobuf/unknown_field_set.h
  src/google/protobuf/wire_format_lite.h
  src/google/protobuf/descriptor.h
  src/google/protobuf/descriptor_database.h
  src/google/protobuf/extension_set.h
  src/google/protobuf/generated_message_reflection.h
  src/google/protobuf/wire_format_lite_inl.h
  src/google/protobuf/message.h
  src/google/protobuf/reflection_ops.h
  src/google/protobuf/service.h
  src/google/protobuf/text_format.h
  src/google/protobuf/wire_format.h
  src/google/protobuf/io/coded_stream.h
  src/google/protobuf/io/gzip_stream.h
  src/google/protobuf/io/coded_stream_inl.h
  src/google/protobuf/io/printer.h
  src/google/protobuf/io/strtod.h
  src/google/protobuf/io/tokenizer.h
  src/google/protobuf/io/zero_copy_stream.h
  src/google/protobuf/io/zero_copy_stream_impl.h
  src/google/protobuf/io/zero_copy_stream_impl_lite.h
  src/google/protobuf/stubs/atomicops.h
  src/google/protobuf/stubs/atomicops_internals_atomicword_compat.h
  src/google/protobuf/stubs/atomicops_internals_pnacl.h
  src/google/protobuf/stubs/atomicops_internals_x86_gcc.h
  src/google/protobuf/stubs/common.h
  src/google/protobuf/stubs/once.h
  src/google/protobuf/stubs/stringprintf.h
  src/google/protobuf/stubs/atomicops_internals_arm64_gcc.h
  src/google/protobuf/stubs/atomicops_internals_generic_gcc.h
  src/google/protobuf/stubs/atomicops_internals_solaris.h
  src/google/protobuf/stubs/hash.h
  src/google/protobuf/stubs/platform_macros.h
  src/google/protobuf/stubs/substitute.h
  src/google/protobuf/stubs/atomicops_internals_arm_gcc.h
  src/google/protobuf/stubs/atomicops_internals_macosx.h
  src/google/protobuf/stubs/atomicops_internals_tsan.h
  src/google/protobuf/stubs/atomicops_internals_x86_msvc.h
  src/google/protobuf/stubs/map_util.h
  src/google/protobuf/stubs/stl_util.h
  src/google/protobuf/stubs/template_util.h
  src/google/protobuf/stubs/atomicops_internals_arm_qnx.h
  src/google/protobuf/stubs/atomicops_internals_mips_gcc.h
  src/google/protobuf/stubs/strutil.h
  src/google/protobuf/stubs/type_traits.h
)

if (MSVC)
  set(protobuf_STAT_SRCS
    ${protobuf_STAT_SRCS}
    src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc
  )
  add_compile_options(/wd4800)
else()
  set(protobuf_STAT_SRCS
    ${protobuf_STAT_SRCS}
    src/google/protobuf/stubs/atomicops_internals_x86_gcc.cc
  )
endif()

include_directories(
	${CMAKE_SOURCE_DIR}/dep
    ${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf
	${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/compiler
	${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/io
	${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/stubs
    ${CMAKE_CURRENT_SOURCE_DIR}/tc_custom
	)

add_library(protobuf STATIC ${protobuf_STAT_SRCS} ${HEADERS})

set_target_properties(protobuf
    PROPERTIES
      FOLDER
        "dep")
