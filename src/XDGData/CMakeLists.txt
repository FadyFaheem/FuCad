include(GNUInstallDirs)

if(NOT DEFINED APPDATA_RELEASE_DATE)
    if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
        execute_process(COMMAND git log -1 --pretty=%cd --date=short
            OUTPUT_VARIABLE APPDATA_RELEASE_DATE
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    else()
        string(TIMESTAMP APPDATA_RELEASE_DATE "%Y-%m-%d")
    endif()
endif()

configure_file(
    org.fucad.FuCad.metainfo.xml.in
    ${CMAKE_BINARY_DIR}/org.fucad.FuCad.metainfo.xml
)
install(
    FILES ${CMAKE_BINARY_DIR}/org.fucad.FuCad.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo
)

install(
    FILES org.fucad.FuCad.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
)

install(
    FILES org.fucad.FuCad.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/mime/packages
)

configure_file(
    FuCad.thumbnailer.in
    ${CMAKE_BINARY_DIR}/FuCad.thumbnailer
)
install(
    FILES ${CMAKE_BINARY_DIR}/FuCad.thumbnailer
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/thumbnailers
)

configure_file(
    ${CMAKE_SOURCE_DIR}/src/Tools/fucad-thumbnailer.in
    ${CMAKE_BINARY_DIR}/fucad-thumbnailer
)
install(
    PROGRAMS ${CMAKE_BINARY_DIR}/fucad-thumbnailer
    TYPE BIN
)
