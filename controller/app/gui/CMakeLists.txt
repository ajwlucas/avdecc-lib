cmake_minimum_required (VERSION 2.8) 
project (controller)
enable_testing()

set (CMAKE_PREFIX_PATH "/Users/andy/Qt5.2.1/5.2.1/clang_64")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

include_directories( src ../../lib/include ../../lib/src ../../../jdksavdecc-c/include )

file(GLOB_RECURSE GUI_INCLUDES "src/*.h" )

file(GLOB_RECURSE GUI_SRC "src/*.cpp" )

file(GLOB_RECURSE GUI_UI "src/*.ui" )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)

qt5_wrap_ui(avdeccgui_FORMS_HEADERS ${GUI_UI})

if(APPLE)
  add_executable (avdeccgui ${GUI_INCLUDES} ${GUI_SRC} ${avdeccgui_FORMS_HEADERS})
  set(PCAP_NAME pcap)
  set(READLINE_NAME edit)
elseif(UNIX)
  add_executable (avdeccgui ${GUI_INCLUDES} ${GUI_SRC} ${avdeccgui_FORMS_HEADERS})
  set(PCAP_NAME pcap)
  set(READLINE_NAME readline)
  target_link_libraries(avdeccgui rt)
elseif(WIN32)
  set(PCAP_NAME wpcap)
  include_directories( src src/msvc ../../lib/include ../../../jdksavdecc-c/include )
  file(GLOB_RECURSE SYSTEM_SPEC_SRC "src/msvc/*.c")

  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    link_directories($ENV{WPCAP_DIR}/Lib/x64)
  elseif( CMAKE_SIZEOF_VOID_P EQUAL 4 )
    link_directories($ENV{WPCAP_DIR}/Lib)
  endif()

  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_executable (avdeccgui ${GUI_INCLUDES} ${GUI_SRC} ${SYSTEM_SPEC_SRC} ${avdeccgui_FORMS_HEADERS})
endif()

# Use the Widgets module from Qt 5.
qt5_use_modules(avdeccgui Widgets)

target_link_libraries(avdeccgui controller)

target_link_libraries(avdeccgui ${PCAP_NAME})

target_link_libraries(avdeccgui ${READLINE_NAME})