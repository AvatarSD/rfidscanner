import QtQuick 2.9
import QtQuick.Controls 1.5


Rectangle{   
    signal connectSignal()
    signal disconnectSignal()
    property real btnsWidth: 100
    property real btnsHeight: 25
    property bool reconnReq: false
    
    color: "#ffdf80"
    radius: 6
        
    Column{
        id: col
        spacing: 12
        padding: spacing/2
        Button {
            id: connectBtn
            width: disconnectBtn.width
            height: disconnectBtn.height
            text: qsTr("Connect")
            onClicked: connectSignal()
        }
        Button {
            id: disconnectBtn
            width: btnsWidth
            height: btnsHeight
            text: qsTr("Disconnect")
            onClicked: disconnectSignal()
        }
    }
    
    Text{
        id: reconnMsg
        width: disconnectBtn.width
        text: qsTr("reconnect to apply")
        anchors.centerIn: col
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 7
        font.letterSpacing: -0.5
        padding: 2
        visible: reconnReq
    }
    height: col.height
    width: col.width
    z: -1
}
