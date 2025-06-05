cmake_minimum_required(VERSION 3.16)

project(Platform_app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Znajdź Qt z wymaganymi komponentami
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Linguist LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets
    SerialPort
    3DCore
    3DRender
    3DExtras
    3DInput
    Charts
    Linguist
    LinguistTools
)

# Ustawienia tłumaczeń
set(TRANSLATIONS_DIR ${CMAKE_SOURCE_DIR}/translations)
file(MAKE_DIRECTORY ${TRANSLATIONS_DIR})

set(TS_FILES
    ${TRANSLATIONS_DIR}/app_pl.ts
    ${TRANSLATIONS_DIR}/app_en.ts
)





# Źródła projektu
set(SOURCES
    platformviewer.cpp
    imudisplay.cpp
    ImuGForce.cpp
    hexagon.cpp
    ImuErrorPlotWidget.cpp
    mainwindow.cpp
    main.cpp
)

set(HEADERS
    platformviewer.h
    imudisplay.h
    ImuGForce.h
    hexagon.h
    ImuErrorPlotWidget.h
    mainwindow.h
)

# Tworzenie wykonywalnego pliku
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Platform_app
        MANUAL_FINALIZATION
        ${SOURCES}
        ${HEADERS}
    )
else()
    if(ANDROID)
        add_library(Platform_app SHARED ${SOURCES})
    else()
        add_executable(Platform_app ${SOURCES})
    endif()
endif()

# Linkowanie i zasoby
if(QM_FILES)
    qt_add_resources(translations_resources
        PREFIX "/"
        FILES ${QM_FILES}
    )
    target_link_libraries(Platform_app PRIVATE translations_resources)
endif()

target_link_libraries(Platform_app PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::3DCore
    Qt${QT_VERSION_MAJOR}::3DRender
    Qt${QT_VERSION_MAJOR}::3DExtras
    Qt${QT_VERSION_MAJOR}::3DInput
    Qt${QT_VERSION_MAJOR}::Charts
)

# Ustawienia dla macOS
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Platform_app)
endif()

set_target_properties(Platform_app PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Instalacja
include(GNUInstallDirs)
install(TARGETS Platform_app
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)




if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Platform_app)
endif()

if(TARGET Qt${QT_VERSION_MAJOR}::lupdate)
    qt_add_lupdate(${PROJECT_NAME}
        TS_FILES ${TS_FILES}
        SOURCES ${SOURCES} ${HEADERS}
    )
endif()


if(TARGET Qt${QT_VERSION_MAJOR}::lrelease)
    qt_add_lrelease(${PROJECT_NAME}
        TS_FILES ${TS_FILES}
        QM_FILES_OUTPUT_VARIABLE QM_FILES
        OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/translations
    )
endif()
