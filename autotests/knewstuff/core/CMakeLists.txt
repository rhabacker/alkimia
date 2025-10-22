
# SPDX-FileCopyrightText: Alexander Lohnau <alexander.lohnau@gmx.de>
# SPDX-License-Identifier: BSD-2-Clause

if (WIN32)
set(DATA_DIR "/${CMAKE_CURRENT_BINARY_DIR}/data/")
else()
set(DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/data/")
endif()
set(ENTRY4_VERSION "2.1git2")
configure_file(data/entry.xml.in data/entry.xml)
configure_file(data/provider.xml.in data/provider.xml)
configure_file(data/enginetest.knsrc.in data/enginetest.knsrc)
configure_file(data/installationtest.knsrc.in data/installationtest.knsrc)
configure_file(data/testfile.txt data/testfile.txt)

set(ENTRY4_VERSION "3.1git2")
set(DATA_DIR_SAVE ${DATA_DIR})
string(APPEND DATA_DIR "update/")
configure_file(data/entry.xml.in data/update/entry.xml)
configure_file(data/provider.xml.in data/update/provider.xml)
configure_file(data/enginetest.knsrc.in data/update/enginetest.knsrc)
configure_file(data/installationtest.knsrc.in data/installationtest.knsrc)
configure_file(data/testfile.txt data/update/testfile.txt)
set(DATA_DIR ${DATA_DIR_SAVE})

knewstuff_unit_tests(
        knewstuffauthortest.cpp
        knewstuffenginetest.cpp
        installationtest.cpp
)
