const days = ["Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"];
const months = ["Janvier", "F&#233vrier", "Mars", "Avril", "Mai", "Juin", "Juillet", "Ao&#251t", "Septembre", "Octobre", "Novembre", "D&#233cembre"];

var dateText = document.getElementById('date');
var hourText = document.getElementById('hour');

var waitForUpdate = false;

var date = ("0" + dt.getDate()).slice(-2);
dateText.innerHTML = days[dt.getDay()] + "&nbsp;" + date + "&nbsp;" + months[dt.getMonth()];

function hourConvert(dateobj) {
    var hours = ("0" + dateobj.getHours()).slice(-2);
    var minutes = ("0" + dateobj.getMinutes()).slice(-2);
    return hours + "h" + minutes;
}

var min = 0;

function setHour() {
    dt = new Date();
    hourText.innerHTML = hourConvert(dt);

    min = dt.getMinutes();
    if (!waitForUpdate && minsAtUpdate.find(element => element == min)) {
        waitForUpdate = true;
        setTimeout(setLastTemp, 15000);
        setTimeout(function() { waitForUpdate = false }, 60000)
        console.log('updated');
    }
}
setHour();

setInterval(setHour, 5000);
