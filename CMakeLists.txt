cmake_minimum_required(VERSION 3.16)

project(RTMoney VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Charts Widgets)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(appRTMoney
    main.cpp
)

qt_add_qml_module(appRTMoney
    URI RTMoney
    QML_FILES
        Main.qml
        SOURCES currencymanager.h currencymanager.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appRTMoney PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appRTMoney
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appRTMoney PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Charts
    Qt6::Widgets
)

install(TARGETS appRTMoney
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
