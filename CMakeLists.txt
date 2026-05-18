cmake_minimum_required(VERSION 3.16)

project(Limit VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets WebEngineWidgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

# target_sources(Limit
#   PRIVATE
#     formulawidget.h formulawidget.cpp
# )
qt_add_executable(Limit
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        theorywindow.h theorywindow.cpp theorywindow.ui
        res.qrc
        progressmanager.h progressmanager.cpp
        topicmanager.h topicmanager.cpp
        theorycontentmanager.h theorycontentmanager.cpp
        theorycontentwindow.h theorycontentwindow.cpp theorycontentwindow.ui
        formulawidget.h formulawidget.cpp
        practicemanager.h practicemanager.cpp
        practiceaftertheorywindow.h practiceaftertheorywindow.cpp practiceaftertheorywindow.ui
        practiceselectdialog.h practiceselectdialog.cpp practiceselectdialog.ui
        practicesessionwindow.h practicesessionwindow.cpp practicesessionwindow.ui
        variantbuilderdialog.h variantbuilderdialog.cpp variantbuilderdialog.ui
        variantsessionwindow.h variantsessionwindow.cpp variantsessionwindow.ui


    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Limit APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Limit SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Limit
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(Limit PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::WebEngineWidgets)
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Limit)
endif()
set_target_properties(Limit PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Limit
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Limit)
endif()
