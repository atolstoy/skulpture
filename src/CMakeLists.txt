set(skulpture_SRCS
    sk_animations.cpp 
    sk_arrows.cpp 
    sk_buttons.cpp 
    sk_cache.cpp 
    sk_color.cpp 
    sk_combobox.cpp 
    sk_complex.cpp 
    sk_dial.cpp 
    sk_dock.cpp 
    sk_factory.cpp 
    sk_frames.cpp 
    sk_gradients.cpp 
    sk_header.cpp 
    sk_icons.cpp 
    sk_layout.cpp 
    sk_mdi.cpp 
    sk_menu.cpp 
    sk_misc.cpp 
    sk_progressbar.cpp 
    sk_scrollbar.cpp 
    sk_shadows.cpp 
    sk_shapes.cpp 
    sk_shortcuts.cpp 
    sk_slider.cpp 
    sk_spinbox.cpp 
    sk_tabs.cpp
    sk_text.cpp 
    sk_toolbar.cpp 
    skulpture.cpp 
    skulpture_p.cpp 
    skulpturestyleplugin.cpp
)

add_definitions(-DQT_PLUGIN)


if (MSVC)
    add_definitions(-D_USE_MATH_DEFINES) # Needed for M_PI on MSVC
endif()

set(LIBRARY_NAME "skulpture")

add_library(${LIBRARY_NAME} MODULE ${skulpture_SRCS})
target_link_libraries(${LIBRARY_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
message("Qt version - ${QT_VERSION_MAJOR}")
if (NOT APPLE AND NOT WIN32)
    set_target_properties(${LIBRARY_NAME} PROPERTIES
        LINK_FLAGS "-Wl,--no-undefined"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        OUTPUT_NAME "skulpture"
        PREFIX "")
endif()

if (USE_GUI_CONFIG)
    add_subdirectory(config)
endif()


install(TARGETS ${LIBRARY_NAME} DESTINATION "${QT_PLUGINS_DIR}/styles")

