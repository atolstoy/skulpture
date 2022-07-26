set (kstyle_skulpture_config_SOURCES sk_config.cpp)
set (kstyle_skulpture_config_SOURCES sk_dialog.ui sk_preview1.ui sk_preview2.ui)

add_library(kstyle_skulpture_config SHARED ${kstyle_skulpture_config_SOURCES})
install (FILES skulpture.png DESTINATION ${DATA_INSTALL_DIR}/skulpture/pics)
install( FILES skulptureui.rc DESTINATION ${DATA_INSTALL_DIR}/skulpture)
install (TARGETS kstyle_skulpture_config DESTINATION ${PLUGIN_INSTALL_DIR})