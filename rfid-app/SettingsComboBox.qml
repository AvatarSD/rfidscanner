import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3


Rectangle{
    id: settingsComboBox
    
    property string settName: "undef"
    property var settModel: []
    property real settValue: 0
    
    property int spacing: 5
    color: "#d67a5c"
    
    Row{
        id:container
        anchors.centerIn: parent
        padding: settingsComboBox.spacing
        spacing: settingsComboBox.spacing
        Text{
            id: txt
            text: settingsComboBox.settName + ":"
            anchors.verticalCenter: parent.verticalCenter
            padding: settingsComboBox.spacing
        }
        ComboBox{
            id: box
            model: settingsComboBox.settModel
            textRole: "name"
            onCurrentIndexChanged: settingsComboBox.settValue = model[currentIndex].value


//            Connections{
//                target: settingsComboBox
//                onSettValueChanged: {
//                    for(var i = box.model.count-1; i >= 0; i--)
//                        if(box.model[i].value === settingsComboBox.settValue)
//                        {
//                            console.log("true", settModel[i].value, settingsComboBox.settValue)
//                            box.currentIndex = i;//displayText = model.get(i)["name"]
//                            break;
//                        }
//                     console.log(box.currentIndex, JSON.stringify(box.model), JSON.stringify(settingsComboBox.settValue))
////                    box.displayText = settingsComboBox.value
//                }
//            }
            
            anchors.verticalCenter: parent.verticalCenter
            height: txt.height + settingsComboBox.spacing
        }
    }
    
    function mapToBoxModel(variantMapModel){
        var typeMap = []
        for(var val in variantMapModel)
            typeMap.push({name: val, value: variantMapModel[val]})
        console.debug("in map:", JSON.stringify(variantMapModel))
        console.debug("out model:", JSON.stringify(typeMap))
        return typeMap
    }
    
    width: container.width
    height: container.height
}
