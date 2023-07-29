set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(KF_MIN_VERSION "5.104.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(KF5 ${KF_MIN_VERSION} REQUIRED
    CoreAddons
    Completion
    ConfigWidgets
    GuiAddons
    TextWidgets
    WidgetsAddons
    XmlGui
)

include(KDEInstallDirs)

set (kstyle_skulpture_config_SOURCES sk_config.cpp sk_dialog.ui sk_preview1.ui sk_preview2.ui)

add_library(kstyle_skulpture_config MODULE ${kstyle_skulpture_config_SOURCES})
set_target_properties(kstyle_skulpture_config PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" OUTPUT_NAME "kstyle_skulpture_config")
set_target_properties(kstyle_skulpture_config PROPERTIES PREFIX "")

target_link_libraries(kstyle_skulpture_config
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    KF5::CoreAddons
    KF5::Completion
    KF5::ConfigWidgets
    KF5::GuiAddons
    KF5::TextWidgets
    KF5::WidgetsAddons
    KF5::XmlGui
)

install(TARGETS kstyle_skulpture_config DESTINATION ${QT_PLUGINS_DIR})
install(FILES skulptureui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/skulpture)
install(FILES ../skulpture.themerc DESTINATION ${KDE_INSTALL_DATADIR}/kstyle/themes)
