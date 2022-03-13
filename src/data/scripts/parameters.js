const monthsShort = ["Jan", "Fev", "Mar", "Avr", "Mai", "Jun", "Jul", "Aou", "Sep", "Oct", "Nov", "Dec"];
var minsAtUpdate = [0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55];
var firstMeasureDate = "";
var hoursMin = 48;
const maxminDirectory = "../data/max-min.csv"

var request = new XMLHttpRequest();
request.open('GET', "../data/parameters.json");
request.responseType = 'json';
request.send();
request.onload = function () {
    var parameters = request.response;
    minsAtUpdate = parameters.measureTime;
    firstMeasureDate = parameters.firstMeasure;
    hoursMin = parameters.hoursNbMax;
}
request.onerror = function () {
    console.error("Request failed, parameters resetted to default")
}