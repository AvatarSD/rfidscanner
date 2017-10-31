import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.5
//import QtQuick.Layouts 1.3

import ScannerMainElements 1.0

/********* require ******** 
  1. ScannerFacade access by setContextProperty
  
*/

//todo: netSettStat, isReconnReq, clientType, SocketType, 
//      Server, port, msgBound, user, pass, workMode, 
//      timings, logfile

Window {
    id: window
    visible: true
    width: 640
    height: 480
    title: qsTr("EPC Gen2 RFID Node")
    
    Rectangle{
        id: networkFields
        
        // property alias anchors.bottom: bottomline
        
        border.width: 2
        border.color: "#110e0e"
        anchors.margins: 10
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        radius: 22
        visible: true
        
        
        Text {
            id: netStateEnum
            x: 428
            text: facade.netState
            anchors.right: parent.right
            anchors.margins: 15
            anchors.top: parent.top
            anchors.rightMargin: 20
            anchors.left: portField.right
            padding: 10
            font.bold: true
            fontSizeMode: Text.Fit
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
            onTextChanged:  {
                var state = facade.netState;
                if(state == NetState.DISCONNECTED)
                    netStateEnum.color = "red";
                else if(state == NetState.CONNECTED)
                    netStateEnum.color = "green";
                else
                    netStateEnum.color = "#B1C220";
            }
            
        }
        Text {
            id: netStatusMsg
            text: facade.netStateMsg
            fontSizeMode: Text.HorizontalFit
            textFormat: Text.PlainText
            wrapMode: Text.WordWrap
            elide: Text.ElideNone
            padding: 8
            anchors.margins: 15
            anchors.bottomMargin: 0
            font.letterSpacing: 0
            font.wordSpacing: 0
            font.pixelSize: 10
            verticalAlignment: Text.AlignTop
            horizontalAlignment: Text.AlignLeft
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: disconnectBtn.bottom
            
            onYChanged: fitSize()
            onHeightChanged: fitSize()
                 
            
            function fitSize(){
                parent.height = y + height + anchors.bottomMargin - parent.y + connectBtn.anchors.topMargin
            }

            Rectangle{
                anchors.centerIn: parent
                height: parent.height
                color: "#f2f6d2"
                width: parent.width
                
                border.color: "green"
                border.width: 1
                radius: 8
                z: -1
                
            }
        }
        
        TextField {
            id: addrField
            height: 25
            placeholderText: qsTr("Host or IP")
            text: facade.server
            anchors.left: connectBtn.right
            anchors.leftMargin: 20
            anchors.top: connectBtn.top
            anchors.topMargin: 0
            onLengthChanged: facade.server = this.text
        }
        SpinBox {
            id: portField
            y: 28
            width: 163
            height: 31
            value: 65535
            font.letterSpacing: 0
            font.bold: true
            anchors.left: addrField.right
            anchors.leftMargin: 20
            anchors.verticalCenter: addrField.verticalCenter
            antialiasing: true
            minimumValue: 1
            maximumValue: 65535
            onValueChanged: facade.port = this.value
        }
        
        TextField {
            id: userField
            height: 25
            placeholderText: qsTr("Username")
            text: facade.username
            anchors.left: addrField.left
            anchors.leftMargin: 0
            anchors.top: addrField.bottom
            anchors.topMargin: 15
            onLengthChanged: facade.username = this.text 
        }
        TextField {
            id: passField
            y: 93
            height: 25
            placeholderText: qsTr("Password")
            text: facade.password
            anchors.left: userField.right
            anchors.leftMargin: 20
            anchors.verticalCenter: userField.verticalCenter
            onLengthChanged: facade.password = this.text
        }
        
        Button {
            id: connectBtn
            width: 110
            height: 32
            text: qsTr("Connect")
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            checkable: false
            
            onClicked: facade.connectToServer();
        }
        Button {
            id: disconnectBtn
            width: 110
            height: 32
            text: qsTr("Disconnect")
            anchors.left: connectBtn.left
            anchors.leftMargin: 0
            anchors.top: connectBtn.bottom
            anchors.topMargin: 15
            checkable: false
            
            onClicked: facade.disconnectFromServer();
        }
        
        
        
    }
}

