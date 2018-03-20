# Building instruction

Requirements
* cmake
* git
* cmake supported C++ compiler including linker
* Qt4 or Qt5 development package
* libMPIR or libGMP

Fetch alkimia source package from git repo and unpack into a subdirectory with

    cd <root-dir>
    git clone <alkimia-git-repo-url>


For building with Qt5 run the following shell commands

    mkdir <root-dir>/alkimia-build
    cd <root-dir>/alkimia-build
    cmake <root-dir>/alkimia
    make
    sudo make install


In case you are building for Qt4 add -DBUILD_QT4=1 to the cmake command line e.g.

    cmake -DBUILD_QT4=1 <root-dir>/alkimia


To use alkimia in client applications or libraries with cmake and Qt5 add the following line to the top level CMakeLists.txt

    find_package(LibAlkimia5)


for application or libraries with Qt4 add the following line to the top level CMakeLists.txt

    find_package(LibAlkimia)


On creating targets add the library with

    add_executable(<target> ...)
    target_link_libraries(<target>  Alkimia::alkimia)


In source code include header file for example with

    #include <alkimia/alkvalue.h>

    ...

Normally, adding the Alkimia library to a CMake target is enough to add all the required includes
for compilation. In cases where libraries are defined without library dependencies, such as when
creating static libraries, by default the location of the Alkimia include files is not added for
compilation, resulting in compile errors. This can be fixed by adding the following line:

    include_directories(${LibAlkimia[5]_INCLUDE_DIR})

For compatibility with older alkimia releases the related cmake variable LIBALKIMIA_INCLUDE_DIR
is also available. Please note that this variable is deprecated and may be removed in future.
