import QtQuick 2.2
import org.kde.plasma.configuration 2.0

ConfigModel {
    ConfigCategory {
         name: i18n('Currency pairs')
         icon: Qt.resolvedUrl('currency.svg')
         source: 'config/ConfigCurrencyPairs.qml'
    }
}

