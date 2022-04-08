string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
set(TARGET_SUFFIX ${ALKIMIA_LIB_SUFFIX})
configure_file(entities.ent.cmake ${CMAKE_CURRENT_SOURCE_DIR}/entities.ent @only)
add_subdirectory(onlinequoteseditor)
