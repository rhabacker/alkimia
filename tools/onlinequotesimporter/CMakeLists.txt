include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src
)

set(SOURCES
  main.cpp
)

set(HEADERS
)

set(UI
)

add_definitions(-DCMAKE_QUOTES_SOURCE_DIR="${CMAKE_SOURCE_DIR}/data/quotes")

ecm_add_executable(onlinequotesimporter ${SOURCES} ${HEADERS})
if(BUILD_QT4)
    set(LIBS ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_USE_LIBSPREFIX}Core ${QT_USE_LIBSPREFIX}Network ${QT_USE_LIBSPREFIX}WebKit)
else()
    set(LIBS Qt5::Widgets Qt5::WebKitWidgets KF5::KDELibs4Support)
endif()

target_link_libraries(onlinequotesimporter
    alkimia
    alkimia-internal
    ${LIBS}
)
