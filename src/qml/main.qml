/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import com.georgefb.rakki

Window {
    id: window

    property string file: ctxFile
    property int preFullScreenVisibility
    property int maximumImageWidth: 2000
    property int imageSpacing: 25
    property bool upscaleImages: true
    property bool showScrollBar: true


    title: file
    visible: true
    width: 1500
    height: 1000

    onVisibilityChanged: (visibility) => {
        if (!window.isFullScreen()) {
            preFullScreenVisibility = visibility
        }
    }

    Settings {
        id: settings
        property alias maximumImageWidth: window.maximumImageWidth
        property alias imageSpacing: window.imageSpacing
        property alias upscaleImages: window.upscaleImages
        property alias showScrollBar: window.showScrollBar
    }

    Item {
        z: 50
        anchors.fill: parent
        visible: window.file === ""

        ColumnLayout {
            anchors.centerIn: parent

            Button {
                id: selectFileButton

                text: "Open file"
                onClicked: fileDialog.open()

                Layout.alignment: Qt.AlignCenter
            }
            Button {
                id: selectFolderButton

                text: "Open folder"
                onClicked: folderDialog.open()

                Layout.alignment: Qt.AlignCenter
            }
        }
    }

    ToolButton {
        z: 100
        icon.name: "configure"
        onClicked: settingsPopup.open()

        Popup {
            id: settingsPopup

            width: 700
            height: 400

            ColumnLayout {
                RowLayout {
                    Label {
                        text: "Maximum image width"
                    }
                    SpinBox {
                        from: 100
                        to: 9000
                        stepSize: 10
                        value: settings.maximumImageWidth
                        onValueChanged: settings.maximumImageWidth = value
                    }
                }
                RowLayout {
                    Label {
                        text: "Image spacing"
                    }
                    SpinBox {
                        from: 0
                        to: 1000
                        value: settings.imageSpacing
                        onValueChanged: settings.imageSpacing = value
                    }
                }
                RowLayout {
                    Label {
                        text: "Upscale images"
                    }
                    CheckBox {
                        checked: settings.upscaleImages
                        onCheckedChanged: settings.upscaleImages = checked
                    }
                }
                RowLayout {
                    Label {
                        text: "Show scrollbar"
                    }
                    CheckBox {
                        checked: settings.showScrollBar
                        onCheckedChanged: settings.showScrollBar = checked
                    }
                }
            }
        }
    }

    Loader {
        id: mainComponentLoader

        active: window.visible
        asynchronous: true
        anchors.fill: parent
        sourceComponent: ScrollView {
            ScrollBar.vertical: ScrollBar {
                visible: window.showScrollBar
                anchors.top: view.top
                anchors.bottom: view.bottom
                anchors.left: view.right
            }
            ListView {
                id: view

                anchors.fill: parent
                anchors.rightMargin: window.showScrollBar ? parent.ScrollBar.vertical.width : 0
                model: MangaImagesModel {
                    id: mangaImagesModel

                    path: window.file
                }
                spacing: window.imageSpacing
                reuseItems: true
                transformOrigin: Item.Top
                boundsBehavior: Flickable.StopAtBounds
                flickableDirection: Flickable.HorizontalAndVerticalFlick
                delegate: Item {
                    id: delegate

                    height: img.height
                    width: Math.max(view.width, img.width)

                    Image {
                        id: img

                        property int originalWidth: model.width
                        property int originalHeight: model.height

                        anchors.centerIn: parent

                        source: "image://manga/" + model.path
                        width: view.scaledWidth(Qt.size(originalWidth, originalHeight))
                        height: view.scaledHeight(Qt.size(originalWidth, originalHeight))
                        sourceSize.width: window.maximumImageWidth
                        sourceSize.height: window.maximumImageWidth
                        asynchronous: true
                        cache: false
                    }
                }

                TapHandler {
                    acceptedButtons: Qt.LeftButton
                    onDoubleTapped: function(eventPoint, button) {
                        toggleFullScreen()
                    }
                }

                Shortcut {
                    sequence: StandardKey.MoveToStartOfDocument
                    onActivated: view.positionViewAtBeginning()
                }

                Shortcut {
                    sequence: StandardKey.MoveToEndOfDocument
                    onActivated: view.positionViewAtEnd()
                }

                function calculateRatio(size) {
                    let ratio = 1.0
                    let widthToFit = Math.min(view.width, window.maximumImageWidth)
                    ratio = widthToFit / size.width

                    if (ratio > 1.0 && !window.upscaleImages) {
                        ratio = 1.0;
                    }

                    return ratio
                }

                function scaledWidth(size) {
                    let ratio = calculateRatio(size)

                    return size.width * ratio
                }

                function scaledHeight(size) {
                    let ratio = calculateRatio(size)

                    return size.height * ratio
                }

            } // ListView
        }
    }

    FileDialog {
        id: fileDialog

        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        onSelectedFileChanged: {
            window.file = selectedFile
        }
    }

    FolderDialog {
        id: folderDialog

        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        onSelectedFolderChanged: {
            window.file = selectedFolder
        }
    }

    Shortcut {
        sequence: "f"
        onActivated: toggleFullScreen()
    }

    Shortcut {
        sequence: "1"
        onActivated: fileDialog.open()
    }

    Shortcut {
        sequence: "2"
        onActivated: folderDialog.open()
    }

    function isFullScreen() {
        return window.visibility === Window.FullScreen
    }

    function toggleFullScreen() {
        if (!window.isFullScreen()) {
            window.showFullScreen()
        } else {
            if (window.preFullScreenVisibility === Window.Windowed) {
                window.showNormal()
            }
            if (window.preFullScreenVisibility == Window.Maximized) {
                window.show()
                window.showMaximized()
            }
        }
    }
}
