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
Window {
    id: window
    visible: true
    width: 800
    height: 480
    title: qsTr("EPC Gen2 RFID Node")
    color: "#33334d"
    
    property real mainRadius: 10
    property real mainSpacing: 12
    property real subRadius: 5
    property real subSpacing: 6
    
    Column{
        id:mainColumn
        spacing: subSpacing
        padding: mainSpacing
        
        Rectangle{
            radius: mainRadius
            color: "#80bfff"
            Column{
                id: networkFields
                padding: mainSpacing
                spacing: subSpacing
                Row{
                    spacing: subSpacing
                    CtrlBtns {
                        radius: subRadius
                        onConnectSignal: facade.connectToServer()
                        onDisconnectSignal: facade.disconnectFromServer()
                        reconnReq: facade.isReconRequire
                    }
                    AuthFields {
                        radius: subRadius
                        addr: facade.server
                        pass: facade.password
                        port: facade.port
                        user: facade.username
                        onAddrChanged: facade.server = addr;
                        onPortChanged: facade.port = port;
                        onUserChanged: facade.username = user;
                        onPassChanged: facade.password = pass;
                    }
                }
                StateField {
                    radius: subRadius
                    stateMsg: facade.netStateMsg
                    stateEnum: facade.netState
                }
            }
            height: networkFields.height
            width: networkFields.width
        }
        Rectangle{
            radius: mainRadius
            color: "#ff9999"
            Row{
                id: scannerFields
                spacing: subSpacing
                padding: mainSpacing
                
                Rectangle{
                    height: 100
                    width: 180
                    color: "purple"
                    radius: mainRadius
                }
                Rectangle{
                    height: 100
                    width: 105
                    color: "green"
                    radius: mainRadius
                }
            }
            height: scannerFields.height
            width: scannerFields.width
        }
    }
}

