import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtCharts

Window {
    width: 640
    height: 480
    visible: true
    color: "black"
    title: qsTr("Hello World")

    property real currentyRate: 0.0

    Component.onCompleted: {
        updateCurrency()
    }

    function updateCurrency() {
        let coin = coinCombo.currentText.toLowerCase()
        let curr = currCombo.currentText.toLowerCase()
        if (timeCombo.currentText === "Combo") {
            return
        }

        let days = timeCombo.getDays();

        console.log("Send for: ", coin, curr, "Days: ", days)
        myManager.updateChart(coin, curr, days)
    }

    function calculateTotal() {
        if (currencyInput.text === "") {
            currencyOutput.text = ""
            return
        }

        let amount = parseFloat(currencyInput.text.replace(',', '.'))

        if (!isNaN(amount)) {
            let total = amount * currentyRate
            currencyOutput.text = total.toFixed(6)
        }

    }

    Connections {
        target: myManager

        function onRateChanged(rate) {
            currentyRate = rate
            calculateTotal()
        }

        function onChartDataReady(points, minPrice, maxPrice) {
            priceSeries.clear()

            for (let i = 0; i < points.length; i++) {
                priceSeries.append(points[i].x, points[i].y)
            }

            axisY.min = minPrice * 0.95
            axisY.max = maxPrice * 1.05




            if(timeCombo.currentText === "Day") {
                axisX.format = "hh:mm"
                axisX.tickCount = 6
            } else if (timeCombo.currentText === "Year") {
                axisX.format = "MMM yyyy"
                axisX.tickCount = 12
            } else if (timeCombo.currentText === "Week" || timeCombo.currentText === "Month") {
                axisX.format = "dd MMM"
                axisX.tickCount = 7
            }

            if (points.length > 0) {
                axisX.min = new Date(points[0].x)
                axisX.max = new Date(points[points.length - 1].x)
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        GridLayout {
            id: grid
            columns: 2

            ComboBox {
                id: coinCombo
                editable: true
                Component.onCompleted: {
                        for (var i = 0; i < model.count; ++i) {
                            if (model.get(i).text === "Bitcoin") {
                                currentIndex = i;
                                break;
                            }
                        }
                    }
                model: ListModel {
                    id: model
                    ListElement { text: "Bitcoin" }
                    ListElement { text: "Ethereum" }
                    ListElement { text: "Solana" }
                    ListElement { text: "USD" }
                    ListElement { text: "EUR" }
                    ListElement { text: "UAH" }
                }
                onActivated: updateCurrency();
            }

            ComboBox {
                id: currCombo
                editable: true
                Component.onCompleted: {
                        for (var i = 0; i < model.count; ++i) {
                            if (model.get(i).text === "USD") {
                                currentIndex = i;
                                updateCurrency();
                                break;
                            }
                        }
                    }
                model: ListModel {
                    id: model2
                    ListElement { text: "Bitcoin" }
                    ListElement { text: "Ethereum" }
                    ListElement { text: "Solana" }
                    ListElement { text: "USD" }
                    ListElement { text: "EUR" }
                    ListElement { text: "UAH" }
                }
                 onActivated: updateCurrency();
            }

            TextField {
                id: currencyInput
                //placeholderText: "0.00"
                text: "1.00"
                inputMethodHints: Qt.ImhFormattedNumbersOnly

                validator: DoubleValidator {
                    bottom: 0.00
                    decimals: 10
                    notation: DoubleValidator.StandardNotation
                }
                onTextChanged: calculateTotal()
            }
            TextField {
                id: currencyOutput
                placeholderText: "0.00"
                readOnly: true
                inputMethodHints: Qt.ImhFormattedNumbersOnly

                validator: DoubleValidator {
                    bottom: 0.00
                    decimals: 10
                    notation: DoubleValidator.StandardNotation
                }
            }
        }

        ComboBox {
            id: timeCombo
            editable: false
            model: ["Day", "Week", "Month", "Year", "Custom"]

            function getDays() {
                switch(currentText) {
                    case "Day": return 1;
                    case "Week": return 7;
                    case "Month": return 30;
                    case "Year": return 365;
                    default: return 7;
                }
            }


            background: Rectangle {
                implicitWidth: 120
                        implicitHeight: 40
                        border.color: "gray"
                        border.width: 1
                        radius: 5
                        color: "#2c2c2c"
            }

            onActivated: (index) => {
                console.log("Choosed: " + textAt(index))
                            updateCurrency();
            }
        }

        RowLayout {
            visible: timeCombo.currentText === "Custom"
            spacing: 10

            TextField {
                id: fromDate
                placeholderText: "YYYY-MM-DD"
                color: "white"
            }

            Text {
                text: "-"
                color: "white"
            }

            TextField{
                id: toDate
                placeholderText: "YYYY-MM-DD"
                color: "white"
            }

            Button {
                text: "Apply"
                onClicked:  {
                    let fromMs = new Date(fromDate.text).getTime() / 1000
                    let toMs = new Date(toDate.text).getTime() / 1000

                    if (!isNaN(fromMs) && !isNaN(toMs)) {
                        let coin = coinCombo.currentText.toLowerCase();
                        let curr = currCombo.currentText.toLowerCase();
                        console.log("Custom Range:", fromDate.text, "to", toDate.text);
                        myManager.updateChartRange(coin, curr, Math.floor(fromMs), Math.floor(toMs));
                    } else {
                        console.log("Invalid date format. Use YYYY-MM-DD.");
                    }
                }
            }
        }

        ChartView {
            id: chart
            title: "chart"
            Layout.fillWidth: true
            Layout.fillHeight: true
            antialiasing: true
            theme: ChartView.ChartThemeDark

            LineSeries {
                id: priceSeries
                name: "Price"

                axisX: DateTimeAxis {
                        id: axisX
                        format: "dd MMM"
                        tickCount: 7
                }

                axisY: ValueAxis {
                    id: axisY
                    labelFormat: "%.2f"
                }
            }
        }
    }
}
