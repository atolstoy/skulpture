set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(KF_MIN_VERSION "5.104.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(KF5 ${KF_MIN_VERSION} REQUIRED
    CoreAddons
    Completion
    GuiAddons
    KDELibs4Support
    WidgetsAddons
    XmlGui
)

#set (kstyle_skulpture_config_SOURCES sk_config.cpp sk_dialog.ui sk_preview1.ui sk_preview2.ui)
set (kstyle_skulpture_config_SOURCES sk_config.cpp sk_dialog.ui sk_preview1.ui sk_preview2.ui)

add_library(kstyle_skulpture_config MODULE ${kstyle_skulpture_config_SOURCES})

target_link_libraries(kstyle_skulpture_config
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    KF5::CoreAddons
    KF5::Completion
    KF5::GuiAddons
    KF5::KDELibs4Support
    KF5::WidgetsAddons
    KF5::XmlGui
)

install(TARGETS kstyle_skulpture_config DESTINATION ${QT_PLUGINS_DIR})
#install( FILES skulptureui.rc DESTINATION ${DATA_INSTALL_DIR}/skulpture)
#add_executable(kstyle_skulpture_config_tool ${kstyle_skulpture_config_SOURCES})
#install (FILES skulpture.png DESTINATION ${DATA_INSTALL_DIR}/skulpture/pics)
#install( FILES skulptureui.rc DESTINATION ${DATA_INSTALL_DIR}/skulpture)
#install (TARGETS kstyle_skulpture_config DESTINATION ${PLUGIN_INSTALL_DIR})
