import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import TaskMonitor

ApplicationWindow {
    visible: true
    width: 800
    height: 500
    title: "Concurrent Task Monitor"
    color: "#171b26"

    // Tao model (class C++ TaskModel)
    TaskModel {
        id: taskModel
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
                        text: "Cancel"
                        onClicked: taskModel.cancelTask(taskId)
                    }
                }
            }
        }
    }
}