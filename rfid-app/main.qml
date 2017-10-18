import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3

import ScannerMainElements 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("EPC Gen2 RFID Client")
    
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
    
    TextField {
        id: addrField
        x: 307
        y: 135
        text: "localhost"
        placeholderText: qsTr("Host or IP")
    }
    
    TextField {
        id: userField
        x: 274
        y: 191
        placeholderText: qsTr("Username")
    }
    
    TextField {
        id: passField
        x: 307
        y: 241
        placeholderText: qsTr("Password")
    }
    
    SpinBox {
        id: portField
        x: 207
        y: 311
        width: 79
        height: 29
        antialiasing: true
        value: 5600.7
        decimals: 0
        minimumValue: 1
        maximumValue: 65565
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
        
        Connections{
            target: facade
            onNetStateChanged:{
                switch(facade.netState){
                case NetStateEnum.DISCONNECTED : NetStateEnum.text = "disc"
                    break
                case NetStateEnum.CONNECTED : NetStateEnum.text = "ok"
                    break
                }
                
            }
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
    
}
