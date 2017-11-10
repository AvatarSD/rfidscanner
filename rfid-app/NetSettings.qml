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
        SettingsComboBox {
            name: settName
            value: settValue
            model:settValuesList
            
            color: netSettings.itemColor
            radius: netSettings.genRadius

            anchors.horizontalCenter: parent.horizontalCenter
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
