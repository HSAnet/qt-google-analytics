import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

ApplicationWindow {
    id: root
    title: "QtQuick App"

    width: mainLayout.implicitWidth + 2 * margin
    height: mainLayout.implicitWidth + 2 * margin
    minimumWidth: mainLayout.Layout.minimumWidth + 2 * margin
    minimumHeight: mainLayout.Layout.minimumHeight + 2 * margin
    visible: true

    property int margin: 11

    Component.onDestruction: {
        tracker.endSession()
    }

    menuBar: MenuBar {
        Menu {
            title: "File"
            MenuItem {
                text: "Quit"
                onTriggered: Qt.quit()
            }
        }
    }

    statusBar: StatusBar {
        RowLayout {
            Label {
                text: "Sending data ..."
                visible: tracker.isSending
            }
        }
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: margin

        GroupBox {
            Layout.fillWidth: true
            title: "General information"

            ColumnLayout {
                id: rowLayout
                anchors.fill: parent
                anchors.margins: margin

                Label {
                    text: qsTr("Tracking ID: %1").arg(tracker.trackingID)
                }

                Label {
                    text: qsTr("Viewport Size: %1").arg(tracker.viewportSize)
                }

                Label {
                    text: qsTr("Language: %1").arg(tracker.language)
                }

                Label {
                    text: qsTr("Send Interval: %1").arg(tracker.sendInterval)
                }
            }
        }

        GroupBox {
            Layout.fillWidth: true
            title: "Fun box"

            ColumnLayout {
                anchors.fill: parent

                RowLayout {
                    Layout.fillWidth: true

                    Button {
                        text: "Trigger event"
                        onClicked: tracker.sendEvent("ui_event", "button_press", text)
                    }

                    Button {
                        text: "Trigger exception"
                        onClicked: tracker.sendException("some exception", "whatever")
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Button {
                        text: "Change current screen name to:"
                    }

                    TextField {
                        Layout.fillWidth: true
                        text: "MainWindow"
                    }
                }
            }
        }
    }
}
