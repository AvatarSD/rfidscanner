import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.5
//import QtQuick.Layouts 1.3

import ScannerMainElements 1.0

/********* require ******** 
  1. ScannerFacade access by setContextProperty
  
*/

/* todo: 
    Network: netSettState, isReconnReq, clientType, msgBound.
    Scanner: Type, Address, connect, disconnect, reconnReq, addrValid, scannerState, Field
    
    SocketType,workMode, Msg(Repeat|Attempt|Inspect), msgStartSqns, msgEngSqns
    logfile
*/
Column {
    id: netSettings
    
    property real textWidth
    property real boxWidth: 80
    property real itemHeigth: 20
    property real margins: subSpacing
    
    padding: margins
    spacing: margins
    
    
    
    Rectangle{
        height: childrenRect.height+margins*2
        width:textWidth+boxWidth+margins+3
        Text{
            text: "Client Type"
            width: textWidth
            onTextChanged: textWidth = Math.max(textWidth, this.width);
            anchors.margins: margins
            anchors.left: parent.left
            anchors.horizontalCenter: parent.horizontalCenter
        }
        ComboBox{
            width: boxWidth
            anchors.margins: margins
            anchors.right: parent.right
            anchors.horizontalCenter: parent.horizontalCenter
            
        }
    }
    Rectangle{
        id:proto
        Text{
            text: "Protocol"
            width: textWidth
            onTextChanged: textWidth = Math.max(textWidth, this.width);
            
        }
        ComboBox{
            width: boxWidth
            
        }
    }
    Rectangle{
        Text{
            text: "Socket type"
            width: textWidth
            onTextChanged: textWidth = Math.max(textWidth, this.width);
            
        }
        ComboBox{
            width: boxWidth
            
        }
    }
    
    Item{
        z:-1
        Rectangle{
            height: netSettings.height
            width: netSettings.width
            color: "#bfa1e6"
        }
    }
    
}
