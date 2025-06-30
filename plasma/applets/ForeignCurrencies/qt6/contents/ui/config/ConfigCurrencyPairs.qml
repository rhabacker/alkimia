import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.plasma.core as PlasmaCore
import org.kde.kcmutils as KCMUtils

Kirigami.Page {
    id: page

    property string cfg_currencies

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Background color
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            color: Kirigami.Theme.backgroundColor

            ListView {
                id: currenciesView

                anchors.fill: parent

                model: ListModel {
                    id: currenciesModel
                }

                delegate: QQC2.ItemDelegate {
                    id: delegate

                    required property int index
                    required property string from
                    required property string to
                    required property string pairName

                    width: ListView.view.width

                    text: String("%1 (%2 => %3)").arg(pairName).arg(from).arg(to)

                    highlighted: ListView.isCurrentItem

                    contentItem: Kirigami.TitleSubtitle {
                        title: delegate.pairName
                        subtitle: delegate.from + " > " + delegate.to
                        font: delegate.font
                        selected: delegate.highlighted
                    }
                    onClicked: {
                        currenciesView.currentIndex = delegate.index

                        console.debug("IDX:" + currenciesView.currentIndex)
                    }
                }
            }
        }

        RowLayout {
            QQC2.Button {
                text: i18n("Add")
                icon.name: "list-add"
                onClicked: addDialog.open()
            }
            QQC2.Button {
                text: i18n("Move up")
                icon.name: "go-up"
                enabled: currenciesView.currentIndex > 0
                onClicked: {
                    const idx = currenciesView.currentIndex
                    currenciesModel.move(idx, idx - 1, 1)
                    page.updateConfig()
                }
            }
            QQC2.Button {
                text: i18n("Move down")
                icon.name: "go-down"
                enabled: currenciesView.currentIndex < (currenciesView.count - 1)
                onClicked: {
                    const idx = currenciesView.currentIndex
                    currenciesModel.move(idx, idx + 1, 1)
                    page.updateConfig()
                }
            }
            QQC2.Button {
                text: i18n("Remove")
                icon.name: "list-remove"
                enabled: currenciesView.currentIndex >= 0
                onClicked: {
                    currenciesModel.remove(currenciesView.currentIndex)
                    page.updateConfig()
                }
            }
        }
    }

    Kirigami.Dialog {
        id: addDialog

        title: i18n("New currency pair")
        preferredWidth: Kirigami.Units.gridUnit * 16
        standardButtons: Kirigami.Dialog.Ok

        onAccepted: page.addNewPair(newName.text, newFrom.currentValue, newTo.currentValue)

        ListModel {
            id: availableCurrenciesModel
            ListElement { text: qsTr("EUR - Euro"); value: "EUR"}
            ListElement { text: qsTr("USD - US Dollar"); value: "USD"}
            ListElement { text: qsTr("JPY - Japanese yen"); value: "JPY"}
            ListElement { text: qsTr("BGN - Bulgarian lev"); value: "BGN"}
            ListElement { text: qsTr("CZK - Czech koruna"); value: "CZK"}
            ListElement { text: qsTr("DKK - Danish krone"); value: "DKK"}
            ListElement { text: qsTr("GBP - Pound sterling"); value: "GBP"}
            ListElement { text: qsTr("HUF - Hungarian forint"); value: "HUF"}
            ListElement { text: qsTr("PLN - Polish zloty"); value: "PLN"}
            ListElement { text: qsTr("RON - Romanian leu"); value: "RON"}
            ListElement { text: qsTr("SEK - Swedish krona"); value: "SEK"}
            ListElement { text: qsTr("CHF - Swiss franc"); value: "CHF"}
            ListElement { text: qsTr("ISK - Icelandic krona"); value: "ISK"}
            ListElement { text: qsTr("NOK - Norwegian krone"); value: "NOK"}
            ListElement { text: qsTr("TRY - Turkish lira"); value: "TRY"}
            ListElement { text: qsTr("AUD - Australlian dollar"); value: "AUD"}
            ListElement { text: qsTr("BRL - Brazilian real"); value: "BRL"}
            ListElement { text: qsTr("CAD - Canadian dollar"); value: "CAD"}
            ListElement { text: qsTr("CNY - Chinese yan renminbi"); value: "CNY"}
            ListElement { text: qsTr("HKD - Hong Kong dollar"); value: "HKD"}
            ListElement { text: qsTr("IDR - Indonesian rupiah"); value: "IDR"}
            ListElement { text: qsTr("ILS - Israeli shekel"); value: "ILS"}
            ListElement { text: qsTr("INR - Indian rupee"); value: "INR"}
            ListElement { text: qsTr("KRW - South Korean won"); value: "KRW"}
            ListElement { text: qsTr("MXN - Mexican peso"); value: "MXN"}
            ListElement { text: qsTr("MYR - Malaysian ringgit"); value: "MYR"}
            ListElement { text: qsTr("NZD - New Zealand dollar"); value: "NZD"}
            ListElement { text: qsTr("PHP - Phillipine peso"); value: "PHP"}
            ListElement { text: qsTr("SGD - Singapore dollar"); value: "SGD"}
            ListElement { text: qsTr("THB - Thai baht"); value: "THB"}
            ListElement { text: qsTr("ZAR - South African rand"); value: "ZAR"}
        }

        Kirigami.FormLayout {
            QQC2.TextField {
                id: newName
                Kirigami.FormData.label: i18n("Name")
                text: i18n("New currency pair")
            }
            QQC2.ComboBox {
                id: newFrom
                Kirigami.FormData.label: i18n("From")
                model: availableCurrenciesModel
                textRole: "text"
                valueRole: "value"
            }
            QQC2.ComboBox {
                id: newTo
                Kirigami.FormData.label: i18n("To")
                model: availableCurrenciesModel
                textRole: "text"
                valueRole: "value"
            }
        }

        onOpened: {
            newName.text = i18n("New currency pair")
            newFrom.currentIndex = 0
            newTo.currentIndex = 0
        }
    }

    Component.onCompleted: {
        const currencies = JSON.parse(cfg_currencies)

        currencies.forEach(line => {
            currenciesModel.append({
                pairName: line.pairName,
                from: line.from,
                to: line.to,
            })

            console.debug("Line:", line.pairName, line.from, line.to)
        })
    }

    function addNewPair(name, from, to) {
        currenciesModel.append({
            pairName: name,
            from: from,
            to: to
        })

        updateConfig()
    }

    function updateConfig() {
        let newCurrenciesArray = []

        for (let i = 0; i < currenciesModel.count; i++) {
            newCurrenciesArray.push({
                pairName: currenciesModel.get(i).pairName,
                from: currenciesModel.get(i).from,
                to: currenciesModel.get(i).to,
            })
        }

        cfg_currencies = JSON.stringify(newCurrenciesArray)
    }
}
