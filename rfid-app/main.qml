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
    
    Column{
        id:mainColumn
        spacing: 10
        padding: spacing
 
        Row{
            id: networkFields
            spacing: 5

            CtrlBtns {
                id: ctrlBtns    
                onConnectSignal: facade.connectToServer()
                onDisconnectSignal: facade.disconnectFromServer()
                reconnReq: facade.isReconRequire
            }
            AuthFields {
                id: authFields
                
                addr: facade.server
                pass: facade.password
                port: facade.port
                user: facade.username
                onAddrChanged: facade.server = addr;
                onPortChanged: facade.port = port;
                onUserChanged: facade.username = user;
                onPassChanged: facade.password = pass;
            }

                Text {
                    id: netStateEnum
                    x: 428
                    text: facade.netState
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

            Rectangle{
                color: "#f2f6d2"
                border.color: "green"
                border.width: 1
                radius: 8
                z: -1
                Text {
                    id: netStatusMsg
                    text: facade.netStateMsg
                    fontSizeMode: Text.HorizontalFit
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap
                    elide: Text.ElideNone
                    padding: 8
                    font.pixelSize: 10 
                }
                height: netStatusMsg.height
                width: netStatusMsg.width
            }
            

        }
        Row{
            id: scannerFields
            spacing: 10
            
            Rectangle{
                height: 100
                width: 180
                color: "purple"
            }
            Rectangle{
                height: 100
                width: 105
                color: "green"
            }
        }

    }
}

