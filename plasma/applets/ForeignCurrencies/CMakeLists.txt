set(APPLET_NAME org.wincak.foreigncurrencies2)

plasma_install_package(qt${QT_MAJOR_VERSION} ${APPLET_NAME})

set(TRACE "QML_IMPORT_TRACE=1")

add_custom_target(foreigncurrencies-test
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/qt${QT_MAJOR_VERSION} ${CMAKE_CURRENT_BINARY_DIR}/${APPLET_NAME}
    COMMAND ${CMAKE_COMMAND} -E
    env "QML2_IMPORT_PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}:${CMAKE_CURRENT_BINARY_DIR} ${TRACE}"
    plasmoidviewer --applet ${APPLET_NAME}
)
