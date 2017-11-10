import QtQuick 2.9

Item{
    z:-2  
    property alias color: rct.color
    property alias radius: rct.radius
    Rectangle{
        id: rct
        height: parent.parent.height
        width: parent.parent.width
        radius: 5
        color: "#4d4dff"
    }
}
