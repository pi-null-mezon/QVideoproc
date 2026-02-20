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

import QtMultimedia 5.9
import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.4

import PiNullMezon 1.0

Window {
    id: appwindow
    visible: true
    width: 640
    height: 480


    function setAppTitle(_title) {
        appwindow.title = _title
    }

    signal frameCaptured(string filename)

    signal replyReady(string _reply)
    onReplyReady: photoPreview.showText(_reply)


    Rectangle {
        id : cameraUI
        anchors.fill: parent

        color: "black"
        state: "PhotoCapture"

        states: [
            State {
                name: "PhotoCapture"
                StateChangeScript {
                    script: {
                        camera.captureMode = Camera.CaptureStillImage
                        camera.start()
                    }
                }
            },
            State {
                name: "PhotoPreview"
            },
            State {
                name: "VideoCapture"
                StateChangeScript {
                    script: {
                        camera.captureMode = Camera.CaptureVideo
                        camera.start()
                    }
                }
            },
            State {
                name: "VideoPreview"
                StateChangeScript {
                    script: {
                        camera.stop()
                    }
                }
            }
        ]

        Camera {
            id: camera
            captureMode: Camera.Capt1ureStillImage

            imageCapture {
                onImageCaptured: {
                    photoPreview.source = preview
                    stillControls.previewAvailable = true
                    cameraUI.state = "PhotoPreview"
                    photoPreview.showText("Please, wait server reply...")
                }

                onImageSaved: {
                    frameCaptured(path)
                }
            }

            viewfinder.resolution: "640x480"

            videoRecorder {
                 resolution: "640x360"
                 frameRate: 30
            }
        }

        PhotoPreview {
            id : photoPreview
            anchors.fill : parent
            onClosed: cameraUI.state = "PhotoCapture"
            visible: cameraUI.state == "PhotoPreview"
            focus: visible
        }

        VideoPreview {
            id : videoPreview
            anchors.fill : parent
            onClosed: cameraUI.state = "VideoCapture"
            visible: cameraUI.state == "VideoPreview"
            focus: visible

            //don't load recorded video if preview is invisible
            source: visible ? camera.videoRecorder.actualLocation : ""
        }

        CustomFilter {
            id: videofilter
        }

        VideoOutput {
            id: viewfinder
            visible: cameraUI.state == "PhotoCapture" || cameraUI.state == "VideoCapture"

            anchors.fill: parent
            width: parent.width - stillControls.buttonsPanelWidth
            height: parent.height

            source: camera
            filters: [videofilter]
            autoOrientation: true
            fillMode: VideoOutput.PreserveAspectCrop
        }

        PhotoCaptureControls {
            id: stillControls
            anchors.fill: parent
            camera: camera
            visible: cameraUI.state == "PhotoCapture"
            onPreviewSelected: cameraUI.state = "PhotoPreview"
            onVideoModeSelected: cameraUI.state = "VideoCapture"
            onUrlUpdated: cameraUI.urlUpdated(stillControls.serverurl)
        }

        VideoCaptureControls {
            id: videoControls
            anchors.fill: parent
            camera: camera
            visible: cameraUI.state == "VideoCapture"
            onPreviewSelected: cameraUI.state = "VideoPreview"
            onPhotoModeSelected: cameraUI.state = "PhotoCapture"
        }
    }

    Dialog {
        id: serverDialog
        modal: true
        padding: 0
        width: 300
        height: dialogLayoutC.height

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        Column {
            id: dialogLayoutC
            width: parent.width

            Pane {
                width: parent.width
                TextField {
                    id: serverUrlTF
                    width: parent.width
                }
            }

            Pane {
                background: Rectangle {
                    color: "light gray"
                }
                width: parent.width
                Row {
                    spacing: 10
                    anchors.right: parent.right
                    Button {
                        text: "Accept"
                        onClicked: serverDialog.accept()
                    }
                    Button {
                        text: "Reject"
                        onClicked: serverDialog.reject()
                    }
                }
            }
        }
        onOpened: {
            serverUrlTF.text = settings.apiurl
        }
        onAccepted: {
            settings.apiurl = serverUrlTF.text
        }
    }
}
