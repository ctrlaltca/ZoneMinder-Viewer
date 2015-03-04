set ( core_SRCS
      stream.cpp
      camerawidgettoolbar.cpp
      cameraadddialog.cpp
      camerawidget.cpp
      framewidget.cpp
      connectionmanager.cpp
      fullscreencamera.cpp
      databasesession.cpp
      monitors.cpp
      connectionwidget.cpp
      cameraevents.cpp
      auth.cpp
      authdialog.cpp
      qmultipartreader.cpp
      cameralayout.cpp
      cameraviewerpage.cpp
      maincamerawidget.cpp
      doublecamerawidget.cpp
      blackwidget.cpp
      commandlistener.cpp
      models/basemodel.cpp
      models/eventmodel.cpp
      abstractplayer.cpp
      vlcplayer.cpp
      mjpegplayer.cpp
    )

set ( core_UIS
      ui/cameraadddialog.ui
      ui/authdialogwidget.ui
    )

set(ZMCORE_LIBS)

# Qt4 stuff
if(Qt4_FOUND)
    SET(QT_USE_QTCORE TRUE)
    SET(QT_USE_QTGUI TRUE)
    SET(QT_USE_QTNETWORK TRUE)
    SET(QT_USE_QTSQL TRUE)

    # Include directories
    INCLUDE(${QT_USE_FILE})
    include_directories(${QT_INCLUDES})
    LIST(APPEND ZMCORE_LIBS ${QT_LIBRARIES})

    QT4_ADD_RESOURCES ( core_SRCS resources.qrc )
    QT4_WRAP_UI( core_SRCS ${core_UIS} )
endif()

# qt5 stuff
if(Qt5Widgets_FOUND)
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    list(APPEND ZMCORE_LIBS Widgets)

    # QtNetwork
    find_package(Qt5Network)
    if(Qt5Network_FOUND)
        include_directories(${Qt5Network_INCLUDE_DIRS})
        list(APPEND ZMCORE_LIBS Network)
    endif()

    # QtSql
    find_package(Qt5Sql)
    if(Qt5Sql_FOUND)
        include_directories(${Qt5Sql_INCLUDE_DIRS})
        list(APPEND ZMCORE_LIBS Sql)
    endif()

    QT5_ADD_RESOURCES ( core_SRCS resources.qrc )
    QT5_WRAP_UI( core_SRCS ${core_UIS} )
endif()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

include_directories ( ${CMAKE_CURRENT_BINARY_DIR}
                      ${CMAKE_CURRENT_SOURCE_DIR} 
                      ${LIBVLC_INCLUDE_DIR}
		     )

add_library (zmviewercore SHARED ${core_SRCS} ${core_MOC_SRCS})

if(Qt4_FOUND)
    target_link_libraries(zmviewercore ${ZMCORE_LIBS} ${LIBVLC_LIBRARY})
endif()
if(Qt5Widgets_FOUND)
    TARGET_LINK_LIBRARIES(zmviewercore ${LIBVLC_LIBRARY})
    qt5_use_modules(zmviewercore ${ZMCORE_LIBS})
endif()
