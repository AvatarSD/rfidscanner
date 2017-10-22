import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3

import ScannerMainElements 1.0

/********* require ******** 
  1. ScannerFacade access by setContextProperty
  
*/

//todo: isReady, isReconnReq, clientType, SocketType, 
//      Server, port, msgBound, user, pass, workMode, 
//      timings, logfile

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("EPC Gen2 RFID Node")
    
    Button {
        id: connectBtn
        x: 90
        y: 117
        width: 110
        height: 32
        text: qsTr("Connect")
        checkable: false
        
        onClicked: facade.connectToServer();
    }  
    Button {
        id: disconnectBtn
        x: 90
        y: 166
        width: 110
        height: 32
        text: qsTr("Disconnect")
        checkable: false
        
        onClicked: facade.disconnectFromServer();
    }
    
    Text {
        id: netStateEnum
        x: 391
        y: 50
        width: 173
        height: 40
        text: qsTr("")
        font.bold: true
        fontSizeMode: Text.Fit
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 14
        
        function netStateChangedHandler(stat)
        {
            netStateEnum.text = stat; //facade.netState;
            
            if(stat == NetState.DISCONNECTED){
                netStateEnum.color = "red";
            }
            else if(stat == NetState.CONNECTED){
                netStateEnum.color = "green";
            }
            else{
                netStateEnum.color = "#B1C220";
            }
            
        }
        Connections{
            target: facade
            onNetStateChanged: netStateEnum.netStateChangedHandler(facade.netState)
        }
    }
    Text {
        id: netStatusMsg
        x: 43
        y: 404
        width: 540
        height: 43
        text: facade.netStateMsg
        wrapMode: Text.WordWrap
        renderType: Text.QtRendering
        font.pixelSize: 10
    }
   
    TextField {
        id: addrField
        x: 307
        y: 135
        placeholderText: qsTr("Host or IP")
        text: facade.server
        onLengthChanged: facade.server = this.text
    }
    SpinBox {
        id: portField
        x: 207
        y: 311
        width: 79
        height: 29
        antialiasing: true
        value: facade.port
        decimals: 0
        minimumValue: 1
        maximumValue: 65535
        onValueChanged: facade.port = this.value
    }
    
    TextField {
        id: userField
        x: 274
        y: 191
        placeholderText: qsTr("Username")
        text: facade.username
        onLengthChanged: facade.username = this.text 
    }
    TextField {
        id: passField
        x: 307
        y: 241
        placeholderText: qsTr("Password")
        text: facade.password
        onLengthChanged: facade.password = this.text
    }
    
}

