import QtQuick 2.9
import QtQuick.Controls 1.5


Rectangle{   
    signal connectSignal()
    signal disconnectSignal()
    property bool reconnReq: false
    
    property int btnsWidth: 100
    property int btnsHeight: 25
    color: "#ffdf80"
    radius: 6
    
    property string connBtnTxt: qsTr("Connect")
    property string disconnBtnTxt: qsTr("Disconnect")
        
    Column{
        id: col
        spacing: 12
        padding: spacing/2
        Button {
            id: connectBtn
            width: disconnectBtn.width
            height: disconnectBtn.height
            text: connBtnTxt
            onClicked: connectSignal()
        }
        Button {
            id: disconnectBtn
            width: btnsWidth
            height: btnsHeight
            text: disconnBtnTxt
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
