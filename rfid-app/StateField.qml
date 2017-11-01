import QtQuick 2.9
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import ScannerMainElements 1.0


Row{
    id: stateField
    property var stateEnum
    property string stateMsg
    
    property var radius: 5
    
    Text {
        id: netStateEnum
        text: stateField.stateEnum
        padding: 5
        font.bold: true
        fontSizeMode: Text.Fit
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 14
        onTextChanged:  {
            var state = stateField.stateEnum;
            if(state == NetState.DISCONNECTED)
                netStateEnum.color = "red";
            else if(state == NetState.CONNECTED)
                netStateEnum.color = "green";
            else
                netStateEnum.color = "#B1C220";
        }
        width: 160
        Rectangle{
            anchors.fill: parent
            color: "#ffff99"
            z:-1
        }
    }
    Text {
        id: netStatusMsg
        text: stateMsg
        fontSizeMode: Text.HorizontalFit
        wrapMode: Text.WordWrap
        padding: 5
        font.pixelSize: 9 
        visible: text.length
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        Rectangle{
            anchors.fill: parent
            color: "#f7bf88"
            z:-1
        }
        height: parent.height
    }
    layer.enabled: true
    layer.effect: OpacityMask {
        maskSource: Item {
            width: stateField.width
            height: stateField.height
            Rectangle{ 
                anchors.centerIn: parent
                width: parent.width
                height: parent.height
                radius: stateField.radius  
            }
        }
    }
}
