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

import org.kde.kirigami as Kirigami

import com.georgefb.rakki

Window {
    id: window

    property string file: ctxFile
    property string fileDialogLocation: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
    property string folderDialogLocation: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
    property int preFullScreenVisibility
    property int maximumImageWidth: 2000
    property int imageSpacing: 25
    property int scrollStepSize: 150
    property bool upscaleImages: true
    property bool showScrollBar: true


    title: file
    visible: true
    width: 700
    height: 900

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
        property alias scrollStepSize: window.scrollStepSize
        property alias showScrollBar: window.showScrollBar
        property alias fileDialogLocation: window.fileDialogLocation
        property alias folderDialogLocation: window.folderDialogLocation
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

                RowLayout {
                    Label {
                        text: "Scroll step size"
                    }
                    SpinBox {
                        from: 10
                        to: 1000
                        stepSize: 20
                        value: settings.scrollStepSize
                        onValueChanged: settings.scrollStepSize = value
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
        sourceComponent: ListView {
                id: view

                anchors.fill: parent
                anchors.rightMargin: window.showScrollBar ? ScrollBar.vertical.width : 0
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
                        sourceSize.width: width
                        sourceSize.height: height
                        asynchronous: true
                        cache: false
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    visible: window.showScrollBar
                    anchors.top: view.top
                    anchors.bottom: view.bottom
                    anchors.left: view.right
                }

                Kirigami.WheelHandler {
                    id: wheelHandler

                    target: view
                    verticalStepSize: window.scrollStepSize
                }

                TapHandler {
                    onDoubleTapped: toggleFullScreen()
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

    FileDialog {
        id: fileDialog

        currentFolder: settings.fileDialogLocation
        onAccepted: {
            window.file = selectedFile
            settings.fileDialogLocation = Backend.parentFolder(selectedFile)
        }
    }

    FolderDialog {
        id: folderDialog

        currentFolder: settings.folderDialogLocation
        onAccepted: {
            window.file = selectedFolder
            settings.folderDialogLocation = Backend.parentFolder(selectedFolder)
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
