project (kwin-skulpture)
add_definitions(-DQT_PLUGIN)

set (kwin_skulpture_SOURCES kwin_skulpture.cpp)
add_library (kwin3_skulpture ${kwin_skulpture_SOURCES})
target_link_libraries (kwin3_skulpture)
install (TARGETS kwin3_skulpture DESTINATION ${PLUGIN_INSTALL_DIR})
install (FILES skulpture.desktop DESTINATION ${DATA_INSTALL_DIR}/kwin)

add_subdirectory (config)

