#
# SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.15)

project(rakki VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM 5.100 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FeatureSummary)
include(KDEInstallDirs)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.hpp)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

include(KDEGitCommitHooks)
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

include(ECMSetupVersion)
ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX RAKKI
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/rakki-version.h"
)

find_package(Qt6Core)
set_package_properties(Qt6Core PROPERTIES TYPE REQUIRED)

find_package(Qt6QuickControls2)
set_package_properties(Qt6QuickControls2 PROPERTIES TYPE REQUIRED)

find_package(Qt6Widgets)
set_package_properties(Qt6Widgets PROPERTIES TYPE REQUIRED)

find_package(KF6Archive)
set_package_properties(KF6Archive PROPERTIES
    TYPE REQUIRED URL "https://api.kde.org/frameworks/karchive/html/index.html")

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_subdirectory(data)
add_subdirectory(src)
