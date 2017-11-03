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
                settModel: ListModel{
                    id:settMod 
                    //                        ListElement{
                    //                            settName:"Client Type:"
                    //                            //settValue: 0000
                    //                            settValuesList: sd//facade.getEnumFields("ClientType");
                    //                        }
                    /* settValue settName settValuesList */
                }
                Component.onCompleted: {
                    settMod.append({settName: "Socket Type:", settValuesList: [{ name: "TCP"},{name:"SSL"}]});
                    settMod.append({settName: "Client Type:", settValuesList: facade.getEnumFields("ClientType")});
                    //                              append({"icon": "2.png", value: -1});
                    //                              append({"icon": "3.png", value: leftGrid.secondValue});
                    //                              append({"icon": "4.png", value: leftGrid.thirdValue});
                    
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

