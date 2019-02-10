import QtQuick 1.0
import org.kde.alkimia 1.0

Item {
    width: 500
    height: 300

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
            dateLabel.text = i18n("Date: %1", date)
            priceLabel.text = i18n("Price: %1", price)
        }
    }

    Text {
        id: symbolLabel
        y: 50
        x: 8
        text: i18n("Enter symbol:")
    }
    TextInput {
        y: 50
        x: 150
        id: symbol
        text: "CAD EUR"
        //placeholderText: i18n("Symbol")
    }

    Text {
        id: quoteLabel
        y: 80
        x: 8
        text: i18n("Enter quote:")
    }
    TextInput {
        y: 80
        x: 150
        id: source
        text: "Alkimia Currency"
        //placeholderText: i18n("Source")
    }

    Rectangle {
        width: 200
        height: 30
        y: 122
        x: 150
        color: "lightgray"
        MouseArea {
            id: mouseArea;
            anchors.fill: parent; 

            onClicked: {
                dateLabel.text = ""
                priceLabel.text = ""
                errorLabel.text = ""
                statusLabel.text = ""
                quote.launch(symbol.text, "", source.text)
            }
        }
    }

    Text {
        id: fetchLabel
        y: 128
        x: 170
        text: i18n("Fetch")
    }

    Text {
        id: statusLabel
        y: 150
        x: 150
    }

    Text {
        id: priceLabel
        y: 180
        x: 150
    }

    Text {
        id: dateLabel
        y: 210
        x: 150
    }

    Text {
        id: errorLabel
        y: 240
        x: 150
    }
}
