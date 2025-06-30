import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.plasmoid
import org.kde.plasma.components as PlasmaComponents

PlasmoidItem {
    id: root

    property real lineHeight: Kirigami.Units.gridUnit * 2
    property var config: JSON.parse(plasmoid.configuration.currencies)

    Layout.minimumWidth: Kirigami.Units.gridUnit * 10
    Layout.minimumHeight: lineHeight * lines.count

    Column {
        Repeater {
            id: lines
            model: config
            delegate: Currency {
                width: root.width
                height: root.lineHeight

                pairName: modelData.pairName
                from: modelData.from
                to: modelData.to
            }
        }
    }
}
