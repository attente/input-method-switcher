cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GIO REQUIRED gio-2.0)

add_subdirectory(qt5)

add_custom_command(OUTPUT xml.h
                   COMMAND echo \#define INTROSPECTION_XML \\ > xml.h
                   COMMAND sed -e s/\"/\\\\\"/g < ${CMAKE_BINARY_DIR}/data/com.ubuntu.InputMethodSwitcher.xml | sed s/\\\(.*\\\)/\ \ \"\\1\"\ \\\\/ >> xml.h
                   COMMAND echo \ \ \"\" >> xml.h
                   MAIN_DEPENDENCY ${CMAKE_BINARY_DIR}/data/com.ubuntu.InputMethodSwitcher.xml
                   COMMENT "Generating header from introspection xml"
                   VERBATIM)

add_definitions(-DSCRIPT_DIR="${CMAKE_INSTALL_PREFIX}/share/input-method-switcher/frameworks")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${GIO_INCLUDE_DIRS})

add_executable(switcher main.c xml.h)

target_link_libraries(switcher ${GIO_LIBRARIES})

set_target_properties(switcher PROPERTIES OUTPUT_NAME input-method-switcher)

install(TARGETS switcher RUNTIME DESTINATION bin)
