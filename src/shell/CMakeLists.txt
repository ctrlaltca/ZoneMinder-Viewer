set ( shell_SRCS
      about.cpp
      main.cpp
      mainwindow.cpp
      cameraviewer.cpp
      layoutbar.cpp
      controlbar.cpp
      layoutbutton.cpp
      sliderlayoutbutton.cpp
      adminpanel.cpp
      styles/styledialog.cpp
      styles/style.cpp
      config/pageswidget.cpp
      config/configdialog.cpp
      config/configdialogpage.cpp
      config/cameraconfigwidget.cpp
      config/languageconfigwidget.cpp
      preferencesdialog.cpp
      translator.cpp
    )

set ( shell_UI
      ui/cameraconfigwidget.ui
      ui/languageconfigwidget.ui
)

set(ZMVIEWER_LIBS)

# Qt4 stuff
if(Qt4_FOUND)

    SET(QT_USE_QTXML TRUE)
    SET(QT_USE_QTWEBKIT TRUE)

    # Include directories
    INCLUDE(${QT_USE_FILE})
    INCLUDE_DIRECTORIES(${QT_INCLUDES})
    LIST(APPEND ZMVIEWER_LIBS ${QT_QTMAIN_LIBRARY})
    LIST(APPEND ZMVIEWER_LIBS ${QT_LIBRARIES})

    QT4_ADD_RESOURCES ( shell_SRCS shell_resources.qrc )
    QT4_ADD_RESOURCES ( shell_SRCS ${CMAKE_SOURCE_DIR}/translations.qrc )

    QT4_WRAP_UI( shell_SRCS ${shell_UI} )
endif()

# qt5 stuff
if(Qt5Widgets_FOUND)
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    list(APPEND ZMVIEWER_LIBS Widgets)

    # QtXml
    find_package(Qt5Xml)
    if(Qt5Xml_FOUND)
        include_directories(${Qt5Xml_INCLUDE_DIRS})
        list(APPEND ZMVIEWER_LIBS Xml)
    endif()

    QT5_ADD_RESOURCES ( shell_SRCS shell_resources.qrc )
    QT5_ADD_RESOURCES ( shell_SRCS ${CMAKE_SOURCE_DIR}/translations.qrc )

    QT5_WRAP_UI( shell_SRCS ${shell_UI} )

    # guess plugins and libraries directory
    set(QT_PLUGINS_DIR "${Qt5Widgets_DIR}/../../../plugins")
    get_target_property(QT_LIBRARY_DIR Qt5::Core LOCATION)
    get_filename_component(QT_LIBRARY_DIR ${QT_LIBRARY_DIR} PATH)
endif()


# Declare path variables
set(ICONDIR share/icons CACHE STRING "icon dir")
set(DESKTOPDIR share/applications CACHE STRING "desktop file destination")

# Include directories
include_directories ( ${CMAKE_CURRENT_BINARY_DIR}
                      ${CMAKE_CURRENT_SOURCE_DIR} 
                      ${CMAKE_SOURCE_DIR}/src/core
                      ${CMAKE_BINARY_DIR}/src/core
                      ${CMAKE_BINARY_DIR}
             )

# Build zmviewer binary and link it
ADD_EXECUTABLE(zmviewer WIN32 MACOSX_BUNDLE ${shell_SRCS} ${shell_MOC_SRCS})

if(Qt4_FOUND)
    if(MSVC) 
        set(QT_USE_QTMAIN true) 
    endif() 
    target_link_libraries(zmviewer zmviewercore ${ZMVIEWER_LIBS})
endif()
if(Qt5Widgets_FOUND)
    if(MSVC)
        TARGET_LINK_LIBRARIES(zmviewer zmviewercore Qt5::WinMain)
    else()
        TARGET_LINK_LIBRARIES(zmviewer zmviewercore)
    endif()
    qt5_use_modules(zmviewer ${ZMVIEWER_LIBS})
endif()

if(UNIX)
    if(APPLE)
        set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME}")
        set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.zmviewer.${PROJECT_NAME}")
        set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME}-${PROJECT_VERSION}")
        set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
        set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
        set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})

        INSTALL(TARGETS zmviewer BUNDLE DESTINATION ./)
    else()
        # Assume linux
        INSTALL(TARGETS zmviewer RUNTIME DESTINATION bin/)
    endif()
elseif(WIN32)
        INSTALL(TARGETS zmviewer RUNTIME DESTINATION ./)
endif()

# install ( DIRECTORY styles/examples/blackstyle DESTINATION ${ZMVIEWER_STYLE_DIR} PATTERN ".svn" EXCLUDE )
# install ( DIRECTORY styles/examples/whitestyle DESTINATION ${ZMVIEWER_STYLE_DIR} PATTERN ".svn" EXCLUDE )

if(APPLE)
    get_filename_component(LIBVLC_LIBRARY_DIR "${LIBVLC_LIBRARY}/../../" REALPATH)

    install(CODE "
        EXECUTE_PROCESS(
          COMMAND ${CMAKE_SOURCE_DIR}/cmake/Modules/FixBundle.sh ${CMAKE_INSTALL_PREFIX}/zmviewer.app/Contents/MacOS/ ${LIBVLC_LIBRARY_DIR}
        )
    " COMPONENT Runtime)

    # these needs to be relative to CMAKE_INSTALL_PREFIX
    set(plugin_dest_dir zmviewer.app/Contents/Plugins)
    set(qtconf_dest_dir zmviewer.app/Contents/Resources)
    list(APPEND libSearchDirs ${QT_LIBRARY_DIR})
    list(APPEND libSearchDirs ${CMAKE_BINARY_DIR}/src/core)
    list(APPEND libSearchDirs ${CMAKE_INSTALL_PREFIX}/zmviewer.app/Contents/MacOS/)

    install(DIRECTORY "${QT_PLUGINS_DIR}/" DESTINATION ${plugin_dest_dir} COMPONENT Runtime
        FILES_MATCHING REGEX "(audio|codecs|iconengines|imageformats|sqldrivers|platforms)/.*\\.dylib"
        REGEX ".*_debug\\.dylib" EXCLUDE)

    install(CODE "
        file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"[Paths]
Plugins = Plugins
Translations = Resources/translations
Data = Resources\")
    " COMPONENT Runtime)

    install(CODE "
        file(GLOB_RECURSE QTPLUGINS
            \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/*.dylib\")
        set(BU_CHMOD_BUNDLE_ITEMS ON)
        include(BundleUtilities)
        fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/zmviewer.app\" \"\${QTPLUGINS}\" \"${libSearchDirs}\")
        " COMPONENT Runtime)
endif()