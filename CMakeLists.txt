project (zmviewer)

SET(QT_MIN_VERSION "4.4.0")

cmake_minimum_required(VERSION 2.8.9)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)
 
# Find Qt and enable the needed features
# Default is Qt5 unless WITH_QT4 option is enabled
option(WITH_QT4 "Force the use of Qt4 libraries" OFF)
OPTION(UPDATE_TRANSLATIONS "Update translations on compile" OFF)
MESSAGE("UPDATE TRANSLATIONS: ${UPDATE_TRANSLATIONS}")

IF(NOT WITH_QT4)
    # First known not-broken Qt5 version (5.0.2 available on old ubuntus is buggy).
    FIND_PACKAGE(Qt5Widgets 5.0.3)
ENDIF()

IF(Qt5Widgets_FOUND)
    MESSAGE(STATUS "Found Qt ${Qt5Widgets_VERSION_STRING}")

    # FIX: Qt was built with -reduce-relocations
    if (Qt5_POSITION_INDEPENDENT_CODE)
        SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif()

    FIND_PACKAGE(Qt5LinguistTools)
    IF(UPDATE_TRANSLATIONS)
        IF(NOT Qt5_LUPDATE_EXECUTABLE)
            MESSAGE(WARNING "Qt's lupdate not found.")
        ENDIF()
    ENDIF()

    IF(NOT Qt5_LRELEASE_EXECUTABLE)
        MESSAGE(WARNING "Qt's lrelease not found.")
    ENDIF()

ELSE()
    FIND_PACKAGE(Qt4 4.4.0 REQUIRED)
    IF(QT4_FOUND)
        # Old FindQt4.cmake modules used the same flag with different case
        SET(Qt4_FOUND TRUE)
    ENDIF()
    IF(Qt4_FOUND)
        MESSAGE(STATUS "Found Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")

        IF(UPDATE_TRANSLATIONS)
            IF(NOT QT_LUPDATE_EXECUTABLE)
                MESSAGE(WARNING "Qt's lupdate not found.")
            ENDIF()
        ENDIF()

        IF(NOT QT_LRELEASE_EXECUTABLE)
            MESSAGE(WARNING "Qt's lrelease not found.")
        ENDIF()

    ELSE()
        MESSAGE(FATAL_ERROR "No Qt4 or Qt5 found!")
    ENDIF()
ENDIF()

set(CMAKE_AUTOMOC TRUE)

# VLC

find_package ( LIBVLC REQUIRED )

#compiler flags.
SET(CMAKE_CXX_FLAGS "-Wall")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -s -flto -std=c++0x")
SET(CMAKE_CXX_FLAGS_DEBUG  "-O1 -g -std=c++0x -Wno-variadic-macros -Wuninitialized -Wcast-align -Wredundant-decls -Wno-unknown-pragmas -fdata-sections -ffunction-sections -pedantic -Wextra -Wno-long-long -Wstrict-overflow -Woverloaded-virtual -Wsign-promo -Wabi -Wall -Werror -isystem /opt/qt-4-debug/include")

if(NOT CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]")
    set (ZMVIEWERDEF -DQT_NO_DEBUG_OUTPUT )
endif()

# Define a proper install path
if(UNIX)
    if(APPLE)
        # MacOS X
        # Due to the special bundle structure ignore
        # the prefix eventually set by the user.
        set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/release)
    else()
        # Linux / BSD
        if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
            #fix package build
            if(PREFIX)
                set(CMAKE_INSTALL_PREFIX ${PREFIX})
            else()
                set(CMAKE_INSTALL_PREFIX /usr/local)
            endif()
        endif()
    endif()
elseif(WIN32)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/release)
endif()
set(ZMVIEWER_DATA_DIR     ${ZMVIEWER_INSTALL_DIR}/share/zmviewer )
set(ZMVIEWER_STYLE_DIR    ${ZMVIEWER_DATA_DIR}/styles )

if(CMAKE_HOST_WIN32)
	set(ZMVIEWER_LIB_DIR      ${ZMVIEWER_INSTALL_DIR}/bin )
else(CMAKE_HOST_WIN32)
	set(ZMVIEWER_LIB_DIR      ${ZMVIEWER_INSTALL_DIR}/lib )
endif(CMAKE_HOST_WIN32)

configure_file(
   ${CMAKE_CURRENT_SOURCE_DIR}/config-zmviewer.h.cmake
   ${CMAKE_CURRENT_BINARY_DIR}/config-zmviewer.h
)
 
add_definitions(${QT_DEFINITIONS} ${ZMVIEWERDEF}  )

add_subdirectory ( src )
