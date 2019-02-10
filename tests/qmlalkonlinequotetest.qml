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
            dateLabel.text = i18n("Date: %1", date)
            priceLabel.text = i18n("Price: %1", price)
        }
    }

    Label {
        id: symbolLabel
        y: 56
        x: 8
        text: i18n("Enter symbol:")
    }

    TextField {
        y: 47
        x: 150
        id: symbol
        text: "CAD EUR"
        placeholderText: i18n("Symbol")
    }

    Label {
        id: quoteLabel
        y: 86
        x: 8
        text: i18n("Enter quote:")
    }

    TextField {
        y: 74
        x: 150
        id: source
        text: "Alkimia Currency"
        placeholderText: i18n("Source")
    }

    Button {
        width: 200
        height: 30
        y: 122
        x: 150
        text: i18n("Fetch")

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
        x: 150
    }

    Label {
        id: priceLabel
        y: 180
        x: 150
    }

    Label {
        id: dateLabel
        y: 210
        x: 150
    }

    Label {
        id: errorLabel
        y: 240
        x: 150
    }
}
