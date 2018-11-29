/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.9
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import QtQuick.Controls.Styles 1.4
import QtMultimedia 5.9

FocusScope {
    property Camera camera
    property bool previewAvailable : false
    property string serverurl

    function setServerUrl(_url) {
        serverurl = _url;
    }

    //property int buttonsPanelWidth: buttonPaneShadow.width

    signal previewSelected
    signal videoModeSelected
    signal urlUpdated

    id : captureControls

    Column {
        anchors {
            right: parent.right
            top: parent.top
            margins: 2
        }

        id: buttonsColumn
        spacing: 2

        FocusButton {
            camera: captureControls.camera
            visible: camera.cameraStatus == Camera.ActiveStatus && camera.focus.isFocusModeSupported(Camera.FocusAuto)
        }

        CameraButton {
            text: "Identify"
            color: "#88EE00"
            visible: camera.imageCapture.ready
            onClicked: camera.imageCapture.capture()
        }

        /*CameraPropertyButton {
            id : wbModesButton
            value: CameraImageProcessing.WhiteBalanceAuto
            model: ListModel {
                ListElement {
                    icon: "images/camera_auto_mode.png"
                    value: CameraImageProcessing.WhiteBalanceAuto
                    text: "Auto"
                }
                ListElement {
                    icon: "images/camera_white_balance_sunny.png"
                    value: CameraImageProcessing.WhiteBalanceSunlight
                    text: "Sunlight"
                }
                ListElement {
                    icon: "images/camera_white_balance_cloudy.png"
                    value: CameraImageProcessing.WhiteBalanceCloudy
                    text: "Cloudy"
                }
                ListElement {
                    icon: "images/camera_white_balance_incandescent.png"
                    value: CameraImageProcessing.WhiteBalanceTungsten
                    text: "Tungsten"
                }
                ListElement {
                    icon: "images/camera_white_balance_flourescent.png"
                    value: CameraImageProcessing.WhiteBalanceFluorescent
                    text: "Fluorescent"
                }
            }
            onValueChanged: captureControls.camera.imageProcessing.whiteBalanceMode = wbModesButton.value
        }*/

        CameraButton {
            text: "Results"
            onClicked: captureControls.previewSelected()
            visible: captureControls.previewAvailable
        }
    }

    Column {
        anchors {
            bottom: parent.bottom
            right: parent.right
            margins: 2
        }

        id: bottomColumn
        spacing: 2

        /*CameraButton {
            text: "Switch to Video"
            onClicked: captureControls.videoModeSelected()
        }*/

        CameraButton {
            id: quitButton
            text: "Exit"
            color: "#EE0000"
            onClicked: Qt.quit()
        }
    }

    Dialog {
        id: serverDialog

        // Following two lines should be commented when build for Android
        //width: 500
        //height: 100

        contentItem: Rectangle {
            color: "#333"

            width: 600
            height: 200
            Column {               
                anchors {
                    top: parent.top
                    left: parent.left
                    margins: 10
                }
                spacing: anchors.margins
                width: parent.width - 2*anchors.margins
                Label {                    
                    color: "white"
                    id: dialogLabel
                    text: "URL of the identification server:"
                }
                TextField {
                    id: urlText
                    width: parent.width
                    text: serverurl
                }
            }
            Row {
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    margins: 10
                }
                spacing: anchors.margins
                Button {
                    style: ButtonStyle {
                        background: Rectangle {
                                    implicitWidth: 100
                                    implicitHeight: 25
                                    border.width: control.activeFocus ? 2 : 1
                                    border.color: "#aaa"
                                    radius: 2
                                    gradient: Gradient {
                                        GradientStop { position: 0 ; color: control.pressed ? "#444" : "#555" }
                                        GradientStop { position: 1 ; color: control.pressed ? "#333" : "#333" }
                                    }
                        }
                        label: Label {
                            color: "white"
                            text:  "Apply"
                            horizontalAlignment: Qt.AlignHCenter
                        }
                    }
                    onClicked: serverDialog.accept();
                }
                Button { 
                    style: ButtonStyle {
                        background: Rectangle {
                                    implicitWidth: 100
                                    implicitHeight: 25
                                    border.width: control.activeFocus ? 2 : 1
                                    border.color: "#aaa"
                                    radius: 2
                                    gradient: Gradient {
                                        GradientStop { position: 0 ; color: control.pressed ? "#444" : "#555" }
                                        GradientStop { position: 1 ; color: control.pressed ? "#333" : "#333" }
                                    }
                        }
                        label: Label {
                            color: "white"
                            text:  "Cancel"
                            horizontalAlignment: Qt.AlignHCenter
                        }
                    }
                    onClicked: serverDialog.reject();
                }                
            }
        }

        onAccepted: {
            console.log("Accepted")
            serverurl = urlText.text
            captureControls.urlUpdated()
        }

        onRejected: {
            urlText.text = serverurl
        }
    }

    Column {
        anchors {
            top: parent.top
            left: parent.left
            margins: 2
        }

        id: camlistColumn
        spacing: 2

        CameraListButton {
            model: QtMultimedia.availableCameras
            onValueChanged: captureControls.camera.deviceId = value
        }

        CameraButton {
            id: serverButton
            text: "Server"
            onClicked: serverDialog.open()
        }
    }


    ZoomControl {
        x : 0
        y : 0
        width : 100
        height: parent.height

        currentZoom: camera.digitalZoom
        maximumZoom: Math.min(4.0, camera.maximumDigitalZoom)
        onZoomTo: camera.setDigitalZoom(value)
    }
}
