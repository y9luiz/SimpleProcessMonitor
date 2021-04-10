cmake_minimum_required(VERSION 3.14)
project(teste)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt5 COMPONENTS Widgets Core REQUIRED)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(PkgConfig REQUIRED)


pkg_search_module(LIBPROCPS REQUIRED libprocps)
macro(log_pkg_found PKG_NAME PKG_FOUND PKG_VERSION NOTFOUND_LEVEL NOTFOUND_EXTRA)
    if("${PKG_FOUND}" AND "${PKG_VERSION}" STREQUAL "")
        message(STATUS "${PKG_NAME} found")
    elseif(${PKG_FOUND})
        message(STATUS "${PKG_NAME} version: ${PKG_VERSION}")
    else()
        message(${NOTFOUND_LEVEL} "${PKG_NAME} not found${NOTFOUND_EXTRA}")
    endif()
endmacro()


log_pkg_found(libprocps "${LIBPROCPS_FOUND}" "${LIBPROCPS_VERSION}" WARNING "")


if(Qt5Widgets_FOUND)
    message(${Qt5Widgets_INCLUDE_DIRS})
endif()

add_executable(run main.cpp qprocesswindow.hpp qprocesswindow.cpp) 
target_link_libraries(run  Qt5::Widgets Qt5::Core ${LIBPROCPS_LIBRARIES})
target_include_directories(run PUBLIC ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS})