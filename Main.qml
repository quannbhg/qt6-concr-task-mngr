import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import TaskMonitor

ApplicationWindow {
    visible: true
    width: 800
    height: 650
    title: "Concurrent Task Monitor"
    color: "#171b26"

    /* Create Task Model */
    TaskModel {
        id: taskModel
    }

    /* Create Log Model */
    LogModel {
        id: logModel
    }

    /* Whenever TaskModel sends a signal logMessage, record it in LogModel */
    Connections {
        target: taskModel
        function onLogMessage(text, isError) {
            logModel.addLog(text, isError)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        // Hang tren: nhap ten + nut Add
        RowLayout {
            Layout.fillWidth: true

            TextField {
                id: nameInput
                Layout.fillWidth: true
                placeholderText: "Ten task (go 'fail' de test loi)"
                onAccepted: addButton.clicked()   // nhan Enter cung la Add
            }

            Button {
                id: addButton
                text: "Add task"
                onClicked: {
                    taskModel.addTask(nameInput.text)
                    nameInput.text = ""
                }
            }
            Button {
                id: clearButton
                text: "Clear finished"
                onClicked: taskModel.clearFinished()
            }
        }

        // Danh sach task
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 8
            model: taskModel

            delegate: Rectangle {
                width: ListView.view.width
                height: 80
                radius: 6
                color: "#232a3b"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 12

                    // Ten + trang thai
                    ColumnLayout {
                        Layout.preferredWidth: 180

                        Label {
                            text: "#" + taskId + "  " + name
                            color: "white"
                            font.bold: true
                        }
                        Label {
                            text: statusText
                            color: "#b9c3d8"
                        }
                        Label {
                            text: errorMsg
                            color: "#ff6b6b"
                            visible: errorMsg !== ""
                        }
                    }

                    // Thanh tien trinh chay tu 0 den 100
                    ProgressBar {
                        Layout.fillWidth: true
                        from: 0
                        to: 100
                        value: progress
                    }

                    Label {
                        text: progress + "%"
                        color: "white"
                        Layout.preferredWidth: 40
                    }
                    Button {
                        text: statusText === "Paused" ? "Resume" : "Pause"
                        enabled: statusText === "Running" || statusText === "Paused"
                        onClicked: {
                            if (statusText === "Paused") {
                                taskModel.resumeTask(taskId)
                            } else {
                                taskModel.pauseTask(taskId)
                            }
                        }
                    }

                    Button {
                        text: "Cancel"
                        enabled: statusText == "Pending" || statusText == "Running" || statusText == "Paused"
                        onClicked: taskModel.cancelTask(taskId)
                    }
                }
            }
        }
        /* Log Frame */
        RowLayout {
            Layout.fillWidth: true

            Label {
                text: "Log"
                color: "white"
                font.bold: true
                Layout.fillWidth: true
            }

            Button {
                text: "Clear log"
                onClicked: logModel.clearLog()
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 160
            radius: 6
            color: "#0f121a"

            ListView {
                id: logView
                anchors.fill: parent
                anchors.margins: 8
                clip: true
                model: logModel

                // Co dong log moi thi tu cuon xuong cuoi
                onCountChanged: positionViewAtEnd()

                delegate: Label {
                    width: ListView.view.width
                    wrapMode: Text.WordWrap
                    text: "[" + logTime + "] " + message
                    color: isError ? "#ff6b6b" : "#b9c3d8"
                }
            }
        }
    }
}