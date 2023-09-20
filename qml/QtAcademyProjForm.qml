// Copyright 2023 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

import QtQuick
import QtQuick.Controls
import Esri.QtAcademyProj

Item {
    property bool isOffline: false

    // Create MapQuickView here, and create its Map etc. in C++ code
    MapView {
        id: view
        anchors.fill: parent
        // set focus to enable keyboard navigation
        focus: true
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    QtAcademyProj {
        id: model
        mapView: view
    }

    Column {
        id: buttonCol
        anchors {
            top: parent.top
            left: parent.left
            margins: 15
        }
        spacing: 10

        Button {
            id: locationToggleButton

            text: "Toggle Location Display"
            onClicked: {
                model.toggleLocationDisplay();
            }
        }

        Button {
            id: toggleOfflineButton
            text: "Go offline"
            visible: !isOffline
            onClicked: {
                isOffline = true
                model.toggleOffline(isOffline);
            }
        }
        Button {
            id: toggleOnlineButton
            text: "Go online"
            visible: isOffline
            onClicked: {
                isOffline = false;
                model.toggleOffline(isOffline);
            }
        }

        Button {
            id: takeAreaOfflineButton

            text: "Download area"
            visible: !isOffline
            onClicked: {
                model.createOfflineAreaFromExtent();
            }
        }
    }
}
