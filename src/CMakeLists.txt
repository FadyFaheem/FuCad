# SPDX-License-Identifier: LGPL-2.1-or-later

# App::Branding reads this from <AppHomePath>/bin, so it has to sit next to the
# executables in both the build output tree and the install tree.
set(Branding_Files
    branding.xml
)

add_custom_target(Branding_data ALL
    SOURCES ${Branding_Files}
)

fc_copy_sources(Branding_data "${CMAKE_BINARY_DIR}/bin" ${Branding_Files})

install(
    FILES
        ${Branding_Files}
    DESTINATION
        ${CMAKE_INSTALL_BINDIR}
)

add_subdirectory(Build)
add_subdirectory(3rdParty)
add_subdirectory(Base)
add_subdirectory(App)
add_subdirectory(Main)
add_subdirectory(Mod)
add_subdirectory(Ext)
add_subdirectory(Doc)

if(BUILD_GUI)
    add_subdirectory(Gui)
    if(UNIX AND NOT APPLE)
        add_subdirectory(XDGData)
    endif()
endif(BUILD_GUI)

if(BUILD_TEMPLATE)
    add_subdirectory(Tools/_TEMPLATE_)
endif(BUILD_TEMPLATE)

# "if" clause moved into local CMakeLists.txt file to support Conda and Homebrew builds
add_subdirectory(MacAppBundle)

