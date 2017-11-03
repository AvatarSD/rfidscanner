import QtQuick 2.9
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3

import ScannerMainElements 1.0

Rectangle {
    id: netSettings
    
    property int genSpacing: 6
    property int genRadius: 6
    property int boxWidth: 280
    property int boxHeigth: 36
    property color blockColor: "#4d4dff"
    property color itemColor: "#d67a5c"
    property var settModel: []
    
    Component{
        id: settDelegate
        Rectangle{
            radius: netSettings.genRadius
            color: netSettings.itemColor
            width: parent.width
            height: boxVal.height+(netSettings.genSpacing)
            anchors.horizontalCenter: parent.horizontalCenter
            Row{
                anchors.verticalCenter: parent.verticalCenter
                Text{
                    id: txt
                    text: settName
                    anchors.verticalCenter: parent.verticalCenter
                    padding: Math.max(((parent.height-this.height)/2), netSettings.genSpacing)
                }
                ComboBox{
                    id: boxVal
                    model: settValuesList
                    anchors.verticalCenter: parent.verticalCenter
                    
                }
            }
        }
    }
    
    /***/
    ListView{
        id:settView
        model: settModel
        delegate: settDelegate
        spacing: netSettings.genSpacing
        width: parent.width-(netSettings.genSpacing*2)
        height: parent.height-(netSettings.genSpacing*2)
        anchors.centerIn: parent
    }
    
    color: netSettings.blockColor
    radius: netSettings.genRadius
    border.width: 1
    border.color: "black"
    
    width: 320//settView.implicitWidth+(netSettings.genSpacing*2)
    height: 160//settView.implicitWidth+(netSettings.genSpacing*2)
}
