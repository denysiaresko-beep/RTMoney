import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true
    color: white
    title: qsTr("Hello World")

    GridLayout {
        id: grid
        columns: 2

        ComboBox {
            editable: true
            model: ListModel {
                id: model
                ListElement { text: "Bitcoin" }
                ListElement { text: "Ethereum" }
                ListElement { text: "Solana" }
                ListElement { text: "USD" }
                ListElement { text: "EUR" }
                ListElement { text: "UAH" }
            }
        }

        ComboBox {
            editable: true
            model: ListModel {
                id: model2
                ListElement { text: "Bitcoin" }
                ListElement { text: "Ethereum" }
                ListElement { text: "Solana" }
                ListElement { text: "USD" }
                ListElement { text: "EUR" }
                ListElement { text: "UAH" }
            }
        }

        TextField {
            id: currencyInput
            placeholderText: "0.00"
            inputMethodHints: Qt.ImhFormattedNumbersOnly

            validator: DoubleValidator {
                bottom: 0.00
                decimals: 10
                notation: DoubleValidator.StandardNotation
            }
        }
        TextField {
            id: currencyOutput
            placeholderText: "0.00"
            inputMethodHints: Qt.ImhFormattedNumbersOnly

            validator: DoubleValidator {
                bottom: 0.00
                decimals: 10
                notation: DoubleValidator.StandardNotation
            }
        }

    }
}
