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
    
    property real mainRadius: 8
    property real mainSpacing: 8
    
    Component.onCompleted: console.debug(JSON.stringify(facade.socket))
    
    Column{
        id:mainColumn
        padding: mainSpacing
        spacing: mainSpacing
        
        Row{
            id: networkFields
            padding: parent.padding*2
            spacing: parent.spacing
            Column{
                id: mainSettings
                spacing: parent.spacing
                Row{
                    spacing: parent.spacing
                    CtrlBtns {
                        radius: mainRadius
                        onConnectSignal: facade.connectToServer()
                        onDisconnectSignal: facade.disconnectFromServer()
                        reconnReq: facade.isReconRequire
                    }
                    AuthFields {
                        radius: mainRadius
                        addr: facade.server
                        pass: facade.password
                        port: facade.port
                        user: facade.username
                        onAddrChanged: facade.server = addr;
                        onPortChanged: facade.port = port;
                        onUserChanged: facade.username = user;
                        onPassChanged: facade.password = pass;
                        addrErr: facade.netSettState & Facade.NO_SERV
                        portErr: facade.netSettState & Facade.NO_PORT
                        userErr: facade.netSettState & Facade.NO_USER
                        passErr: facade.netSettState & Facade.NO_PASS
                    }
                }
                StateField {
                    radius: mainRadius
                    stateMsg: facade.netStateMsg
                    stateEnum: facade.netState
                }
            }
            NetSettings {
                id: netSettings
                genSpacing: mainSpacing
                genRadius: mainRadius
                
                width: 320
                height: 160
                
                Component.onCompleted: {
                    /* socket type */
                    settModel.append({settName: "Socket Type:", settValue: facade.socket, settValuesList: 
                                         mapToBoxModel(facade.getEnumFieldsVatiant("SocketType"))});
                    
                    /* client type */
                    settModel.append({settName: "Client Type:", settValuesList: 
                                         mapToBoxModel(facade.getEnumFieldsVatiant("ClientType")) })
                    
                    
                }
                
            }
            Item{
                z:-1    
                Rectangle{
                    height: networkFields.childrenRect.height + (mainSpacing*2)
                    width: networkFields.childrenRect.width + (mainSpacing*2)
                    radius: mainRadius
                    color: "#80bfff"
                }
            }
        }
        
        Row{
            id: scannerFields
            spacing: mainSpacing
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
            Item{
                z:-1
                Rectangle{
                    height: scannerFields.height
                    width: scannerFields.width
                    radius: mainRadius
                    color: "#ff9999"
                }
            }
        }
    }
}

