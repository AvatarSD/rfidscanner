import QtQuick 2.9
import QtQuick.Controls 2.2
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
    property var settModel: ListModel{}
    
    function mapToBoxModel(variantMapModel){
        var typeMap = []
        for(var val in variantMapModel)
            typeMap.push({name: val, value: variantMapModel[val]})
        console.debug("in map:", JSON.stringify(variantMapModel))
        console.debug("out model:", JSON.stringify(typeMap))
        return typeMap
    }
    
    Component{
        id: settDelegate
        Rectangle{
            id: delegatRect
            radius: netSettings.genRadius
            color: netSettings.itemColor
            width: parent.width
            height: netSettings.boxHeigth
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
                    textRole: 'name'
//                    onContentItemChanged: itemChanged();
                    currentIndex: settValue
                    onCurrentIndexChanged: console.debug(txt.text,  boxVal.currentText+"("+boxVal.currentIndex+")")
                    
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height - netSettings.genSpacing/2
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
}
