import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3


Rectangle{
    id: settingsComboBox
    
    property string name
    property var model
    property string value
    
    property real spacing: 5
    
    Row{
        id:container
        anchors.centerIn: parent
        padding: settingsComboBox.spacing
        spacing: settingsComboBox.spacing
        Text{
            id: txt
            text: settingsComboBox.name + ":"
            anchors.verticalCenter: parent.verticalCenter
            padding: settingsComboBox.spacing
        }
        ComboBox{
            model: settingsComboBox.model
            textRole: 'name'
            displayText: settingsComboBox.value
            onCurrentTextChanged: {
                settingsComboBox.value = currentText;
                console.debug(txt.text,  currentText+"("+currentIndex+")")
            }
            
            anchors.verticalCenter: parent.verticalCenter
            height: txt.height + settingsComboBox.spacing
        }
    }
    
    width: container.width
    height: container.height
}
