#
# SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause
#

add_executable(rakki)

target_sources(rakki
    PRIVATE
        main.cpp
        extractor.h extractor.cpp
        mangaimagesmodel.h mangaimagesmodel.cpp
        mangaimageprovider.h mangaimageprovider.cpp
        mangaloader.h mangaloader.cpp
        backend.h backend.cpp
)

qt_policy(SET QTP0001 NEW)

qt_add_qml_module(rakki
    URI com.georgefb.rakki
    VERSION 1.0
    QML_FILES
        qml/main.qml
)

if (KArchive_HAVE_LZMA)
    target_compile_definitions(rakki PRIVATE -DWITH_K7ZIP=1)
endif()

target_compile_definitions(rakki
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(rakki
    PRIVATE
        Qt6::Core
        Qt6::QuickControls2

        KF6::Archive
        KF6::Kirigami
)

install(TARGETS rakki DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
