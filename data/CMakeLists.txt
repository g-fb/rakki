#
# SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause
#

ecm_install_icons(
    ICONS sc-apps-rakki.svg
    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME hicolor
)

install(FILES com.georgefb.rakki.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES com.georgefb.rakki.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
