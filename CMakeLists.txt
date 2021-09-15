cmake_minimum_required(VERSION 3.1)
project(xfce-py-plugin)

include(GNUInstallDirs)

set(PLUGIN_ID "py_plugin") # Please update this

set(XFCE_PLUGIN_PATH "${CMAKE_INSTALL_LIBDIR}/xfce4/panel/plugins")
set(PYTHON_PLUGIN_PATH_SUFFIX "${XFCE_PLUGIN_PATH}/python")
set(PYTHON_SEARCH_PATH "${CMAKE_INSTALL_PREFIX}/${PYTHON_PLUGIN_PATH_SUFFIX}")

configure_file(config.h.in config.h @ONLY)
configure_file(plugin.desktop.in plugin.desktop @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(PkgConfig REQUIRED)

pkg_check_modules (GTK REQUIRED gtk+-3.0)
include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})

pkg_check_modules (PYGOBJECT REQUIRED pygobject-3.0)
include_directories(${PYGOBJECT_INCLUDE_DIRS})
link_directories(${PYGOBJECT_LIBRARY_DIRS})

pkg_check_modules (LIBXFCE4_PANEL REQUIRED libxfce4panel-2.0)
include_directories(${LIBXFCE4_PANEL_INCLUDE_DIRS})
link_directories(${LIBXFCE4_PANEL_LIBRARY_DIRS})


find_package(PythonLibs 3.6 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

file(GLOB Plugin_Src "src/*.c" "src/*.h")
add_library(plugin SHARED ${Plugin_Src})
target_link_libraries(plugin ${GTK_LIBRARIES} ${PYTHON_LIBRARIES} ${PYGOBJECT_LIBRARIES} ${LIBXFCE4_PANEL_LIBRARIES})
set_target_properties(plugin PROPERTIES OUTPUT_NAME ${PLUGIN_ID})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/${PLUGIN_ID}.py DESTINATION ${PYTHON_PLUGIN_PATH_SUFFIX})
install(TARGETS plugin LIBRARY DESTINATION ${XFCE_PLUGIN_PATH})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plugin.desktop DESTINATION "share/xfce4/panel/plugins" RENAME ${PLUGIN_ID}.desktop)