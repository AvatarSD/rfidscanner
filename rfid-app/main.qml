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
    
    //    Component.onCompleted: console.debug("socket:", facade.socket)
    
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
            Column{
                spacing: mainSpacing
                padding: mainSpacing
                
                SettingsComboBox{
                    settName: "Socket Type"
                    settModel: mapToBoxModel(facade.getEnumFieldsVatiant("SocketType"))
                    settValue: facade.socket
                    onSettValueChanged: facade.socket = settValue
                }
                SettingsComboBox{
                    settName: "Client Type"
                    settModel: mapToBoxModel(facade.getEnumFieldsVatiant("ClientType"))
                    settValue: facade.clientType
                    onSettValueChanged: facade.clientType = settValue
                }
                SettingsComboBox{
                    settName: "Proto Bound"
                    settModel: mapToBoxModel(facade.getEnumFieldsVatiant("MsgBound"))
                    settValue: facade.msgBoundaries
                    onSettValueChanged: facade.msgBoundaries = settValue
                }
                SettingsComboBox{
                    settName: "Client Mode"
                    settModel: mapToBoxModel(facade.getEnumFieldsVatiant("NetModeEnum"))
                    settValue: facade.mode
                    onSettValueChanged: facade.mode = settValue
                }
                SettingsComboBox{
                    settName: "Auth Type"
                    settModel: mapToBoxModel(facade.getEnumFieldsVatiant("AuthType"))
                    settValue: facade.authType
                    onSettValueChanged: facade.authType = settValue
                }                
                Background {
                    radius: mainRadius
                }
            }
            
            Background{
                radius: mainRadius
                color: "#90cf70"
            }
            
        }
        
        Row{
            id: scannerFields
            spacing: mainSpacing
            padding: mainSpacing
            
            
            CtrlBtns {
                radius: mainRadius
                onConnectSignal: facade.connectToScanner()
                onDisconnectSignal: facade.disconnectFromScanner()
                reconnReq: facade.isScannerReconReq
                connBtnTxt: "Attach"
                disconnBtnTxt: "Dethach"
            }
            
            Rectangle{
                height: 100
                width: 105
                color: "green"
                radius: mainRadius
            }
            Background{
                radius: mainRadius
                color: "#ff9999"
            }
        }
    }
}

