const monthsShort = ["Jan", "Fev", "Mar", "Avr", "Mai", "Jun", "Jul", "Aou", "Sep", "Oct", "Nov", "Dec"];

var dt = new Date();

function getDirectory() {
    var folder = monthsShort[dt.getMonth()] + '_' + dt.getFullYear();
    var file = ("0" + dt.getDate()).slice(-2) + ".csv";
    return "../data/" + folder + "/" + file;
}

function setLastTemp() {
    fetch(getDirectory()).then(function (response) {
        response.text().then(function (text) {
            console.log(text);
            var i = text.length - 2;
            while (text[i] != '\n') {
                i--;
            }
            var line = text.slice(i + 1, text.length - 2).split(',');
            document.getElementById('temperature').innerText = line[1];
            document.getElementById('lastUpdateTime').innerText = line[0];
        });
    });
}

setLastTemp();