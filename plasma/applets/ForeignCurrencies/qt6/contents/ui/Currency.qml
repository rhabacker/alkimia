import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import org.kde.plasma.plasmoid
import org.kde.plasma.components as PlasmaComponents

Item {
    id: control

    property string pairName
    property string from
    property string to

    MouseArea {
        anchors.fill: control
        onClicked: {
            var suffix = control.from + "+in+" + control.to + "&t=canonical&ia=currency"
            Qt.openUrlExternally("https://duckduckgo.com/?q=" + suffix)
        }
    }

    RowLayout {
        anchors.fill: parent

        PlasmaComponents.Label {
            id: currency;
            text: control.pairName
            font.pixelSize: 18
            Layout.fillWidth: true
        }

        PlasmaComponents.Label {
            id: price;
            text: "unknown";
            font.pixelSize: 18;
        }
    }

    Timer {
        id: timer
        interval: 50
        running: true
        repeat: true
        onTriggered: {
            console.log("=== Request attempt ===")
            if ((control.from === "") || (control.to === "")) {
                console.log("Empty parameter(s)")
                timer.interval = 10000
                return
            }
            var rq = new XMLHttpRequest()

            // https://api.frankfurter.dev/v1/latest?base=${from}&symbols=${to}`)
            var url = `https://api.frankfurter.dev/v1/latest?base=${control.from}&symbols=${control.to}`
            console.log("Requesting exchange rate from URL:", url)

            rq.onreadystatechange = function() {
                // readyState goes through states before request is complete.
                if (rq.readyState != XMLHttpRequest.DONE)
                    return

                if (rq.status !== 200) {
                    console.log("Http request error:", rq.status)
                    return
                }

                // Parse and display
                const data = JSON.parse(rq.responseText)
                price.text = data.rates[control.to].toFixed(2) + " " + control.to
            }

            rq.open("GET", url, true);
            rq.send();
            timer.interval = 900000
        }
    }
}

