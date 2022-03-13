//Variables

var startDateInput = document.getElementById('dateStart');
var endDateInput = document.getElementById('dateEnd');

var startTimeInput = document.getElementById('timeStart');
var endTimeInput = document.getElementById('timeEnd');

var startTime;
var endTime;

var temperatures = [4, 3, 2, 1];
var times = ['a', 'b', 'c', 'd', 'e'];

var filesList = [];



//Functions

function getFileNb(hours) {
    return Math.floor(hours / 24) + 1;
} 

function getFileList() {
    startTime = new Date(Date.parse(startDateInput.value + 'T' + startTimeInput.value));
    endTime = new Date(Date.parse(endDateInput.value + 'T' + endTimeInput.value));

    var diffInH = calcHoursDiff();
    setMode(diffInH > hoursMin);

    //Si il y a trop de valeurs, on affiche uniquement les max/min
    if (dispModeMinMax) {
        return [maxminDirectory];
    }
    //Sinon, on doit ouvrir tous les fichiers concernés
    else {
        var filesToOpen = getFileNb(diffInH);

        var currTime = startTime;
        var fileList = [];
        for (var i = 0; i < filesToOpen; i++) {
            var folder = monthsShort[currTime.getMonth()] + '_' + currTime.getFullYear();
            var file = currTime.getDate() + ".csv";

            fileList.push("../data/" + folder + "/" + file);

            currTime.setDate(currTime.getDate() + 1);
        }
        return fileList;
    }
}

async function setAllTemperatures(fileList) {
    temperatures = [];

    var allTemperatures = [];
    var allTimes = [];

    var promesses = [];

    for (var i = 0; i < fileList.length; i++) {
        promesses.push(await getTimesAndTemperature(fileList[i]));

        //allTemperatures.push(tempInFile);
        //allTimes.push(timeInFile);
        //console.log(tempInFile);
        //console.log(timeInFile);
    }
    Promise.all(promesses).then(function (values) {
        for (var i = 0; i < values.length; i++) {
            allTimes.push(values[i][0]);
            allTemperatures.push(values[i][1]);
        }

        console.log(allTimes);
        console.log(allTemperatures);

        var startDayIdx = 0;
        var startTimeIdx = 0;

        try {
            startTimeIdx = searchTimeInTimesIncr(allTimes[0], [startTime.getHours(), startTime.getMinutes()]);
            console.log(startTimeIdx);
        }
        catch {
            startDayIdx++;
        }

        var endDayIdx = allTimes.length - 1;
        var endTimeIdx = 0;

        try {
            endTimeIdx = searchTimeInTimesIncr(allTimes[0], [endTime.getHours(), endTime.getMinutes()]);
            console.log(endTimeIdx);
        }
        catch {
            endDayIdx--;
        }

        console.log(allTemperatures[startDayIdx][startTimeIdx]);
        console.log(allTemperatures[endDayIdx][endTimeIdx]);
    });
}

async function getTimesAndTemperature(fileName) {
    let response = await fetch(fileName);
    let text = await response.text();

    console.log(text);

    var timeInFile = [];
    var tempInFile = [];

    var str = '';
    var time = [];
    for (var i = 0; i < text.length; i++) {
        if (text[i] == ':') {
            time.push(parseInt(str));
            str = '';
        }
        else if (text[i] == ',') {
            time.push(str);
            str = '';
            timeInFile.push(time);
            time = [];
        }
        else if (text[i] == '\r') { }
        else if (text[i] == '\n') {
            tempInFile.push(parseInt(str));
            str = '';
        }
        else {
            str += text[i];
        }
    }
    return [timeInFile, tempInFile];
}

//Cherche à quelle ligne se trouve l'heure correspondante, time est de la forme [h, m]
function searchTimeInTimesIncr(timeInFile, time) {
    var timeIdx = 0;

    while (timeIdx < timeInFile.length) {
        if (timeInFile[timeIdx][0] < time[0]) {
            timeIdx++;
        }
        else if (timeInFile[timeIdx][0] == time[0]) {
            if (timeInFile[timeIdx][1] < time[1]) {
                timeIdx++;
            }
            else {
                return timeIdx;
            }
        }
        else {
            return timeIdx;
        }
    }
    throw 'Any values to show';
}
function searchTimeInTimesDecr(timeInFile, time) { ////////C4EST ICI QUE CA MARCHE PA !!
    var timeIdx = timeInFile.length - 1;

    while (timeIdx >= 0) {
        if (timeInFile[timeIdx][0] > time[0]) {
            timeIdx--;
        }
        else if (timeInFile[timeIdx][0] == time[0]) {
            if (timeInFile[timeIdx][1] < time[1]) {
                timeIdx--;
            }
            else {
                return timeIdx;
            }
        }
        else {
            return timeIdx;
        }
    }
    throw 'Any values to show';
}

//const maxminData = {
//    labels: days,
//    datasets: [{
//        label: 'Maximums',
//        data: maximum,
//        fill: false,
//        borderColor: 'darkred',
//        tension: 0.1
//    },
//    {
//        label: 'Minimums',
//        data: minimum,
//        fill: false,
//        borderColor: 'blue',
//        tension: 0.1
//    }]
//};

//const allData = {
//    labels: hours,
//    datasets: [{
//        label: 'Température',
//        data: temperatures,
//        fill: true,
//        borderColor: 'rgb(8, 186, 150)',
//        tension: 0.1
//    }]
//}


//const maxminConfig = {
//    type: 'line',
//    data: maxminData,
//};

//const allConfig = {
//    type: 'line',
//    data: allData,
//}

//var temperaturesChart = new Chart(
//    document.getElementById('chart'),
//    config
//);
