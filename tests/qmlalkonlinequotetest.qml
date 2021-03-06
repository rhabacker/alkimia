import QtQuick 2.6
import QtQuick.Controls 2.0
import org.kde.alkimia 1.0

ApplicationWindow {
    id: root
    width: 500
    height: 300
    visible: true

    AlkOnlineQuote {
        id: quote
        onError: {
            console.log(s)
            errorLabel.text = s
        }
        onStatus: {
            console.log(s)
            statusLabel.text = s
        }
        onQuote: {
            statusLabel.text = ""
            dateLabel.text = "Date: " + date
            priceLabel.text = "Price: " + price
        }
    }

    Label {
        id: symbolLabel
        y: 56
        x: 0
        text: "Enter symbol:"
    }

    TextField {
        y: 50
        x: 100
        id: symbol
        text: "CAD EUR"
        placeholderText: qsTr("Symbol")
    }

    Label {
        id: quoteLabel
        y: 86
        x: 0
        text: "Enter quote:"
    }

    TextField {
        y: 80
        x: 100
        id: source
        text: "Alkimia Currency"
        placeholderText: qsTr("Source")
    }

    Button {
        width: 200
        height: 30
        y: 122
        x: 100
        text: "Fetch"

        onClicked: {
            dateLabel.text = ""
            priceLabel.text = ""
            errorLabel.text = ""
            statusLabel.text = ""
            quote.launch(symbol.text, "", source.text)
        }
    }

    Label {
        id: statusLabel
        y: 150
        x: 100
    }

    Label {
        id: priceLabel
        y: 180
        x: 100
    }

    Label {
        id: dateLabel
        y: 210
        x: 100
    }

    Label {
        id: errorLabel
        y: 240
        x: 100
    }
}
