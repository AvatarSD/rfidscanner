import QtQuick 2.9
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4



Rectangle{
    id: authFields
    
    property string addr: qsTr("localhost")
    property int port: 80
    property string user
    property string pass
    property bool addrErr: false
    property bool portErr: false
    property bool userErr: false
    property bool passErr: false
    
    property real warningBorderWidth: 3
    property real warningBorderRadius: 2
    
    color: "#66b814"
    radius: 6
    
    Grid{
        id: grid
        columns: 2
        spacing: 8
        padding: 8
        flow:Flow.TopToBottom
        
        TextField {
            id: addrField
            placeholderText: qsTr("ip addr")
            text: authFields.addr
            onLengthChanged: authFields.addr = this.text
            Rectangle {
                color: "#00000000"
                radius: warningBorderRadius
                border.color: "#b30000"
                border.width: warningBorderWidth
                visible: addrErr
                anchors.fill: parent
            }
        }
        SpinBox {
            id: portField
            minimumValue: 1
            maximumValue: 65535
            value: authFields.port
            onValueChanged: authFields.port = this.value
            Rectangle {
                color: "#00000000"
                radius: warningBorderRadius
                border.color: "#b30000"
                border.width: warningBorderWidth
                visible: portErr
                anchors.fill: parent
            }
        }
        TextField {
            id: userField
            placeholderText: qsTr("username")
            text: authFields.user
            onLengthChanged: authFields.user = this.text 
            Rectangle {
                color: "#00000000"
                radius: warningBorderRadius
                border.color: "#e6e600"
                border.width: warningBorderWidth
                visible: userErr
                anchors.fill: parent
            }
        }
        TextField {
            id: passField
            placeholderText: qsTr("password")
            text: authFields.pass
            onLengthChanged: authFields.pass = this.text
            Rectangle {
                color: "#00000000"
                radius: warningBorderRadius
                border.color: "#e6e600"
                border.width: warningBorderWidth
                visible: passErr
                anchors.fill: parent
            }
        }
    }
    height: grid.height
    width: grid.width
    z: -1
}
