//Variables

var tSDate = new Date();

var lastMeasureDate = getActDate();
/////////TEMPORARIEasqEEZ
//var firstMeasureDate = "2021-07-30";
//var lastMeasureDate = "2021-08-30";

startDateInput.value = firstMeasureDate;
endDateInput.value = lastMeasureDate;

startDateInput.min = firstMeasureDate;
startDateInput.max = lastMeasureDate;
endDateInput.min = firstMeasureDate;
endDateInput.max = lastMeasureDate;

startTimeInput.value = "00:00";
endTimeInput.value = getLastMeasureTime();

endTimeInput.max = getLastMeasureTime();

var dispModeDisp = document.getElementById('dispMode');
var dispModeMinMax = false;


//Functions

function getActDate() {
    tSDate = new Date();
    var day = ("0" + tSDate.getDate()).slice(-2);
    var month = ("0" + (tSDate.getMonth() + 1)).slice(-2);
    var year = tSDate.getFullYear().toString();
    return year + "-" + month + "-" + day;
}

function getLastMeasureTime() {
    tSDate = new Date();
    var hour = ("0" + tSDate.getHours()).slice(-2);
    var minInt = tSDate.getMinutes();
    while (!minsAtUpdate.find(element => element == minInt)) {
        minInt--;
    }
    var min = ("0" + minInt).slice(-2);
    return hour + ":" + min;
}


function setMode(setToMinMax) {
    dispModeMinMax = setToMinMax;
    dispModeDisp.className = setToMinMax ? "mode-minmax" : "mode-exact";
    dispModeDisp.innerText = setToMinMax ? "minimums / maximums" : "toutes les mesures";
}

function updateStartTimeLimits() {

    if (startDateInput.value == endDateInput.value) {
        startTimeInput.max = endTimeInput.value;
    }
    else {
        startTimeInput.max = "23:55";
    }
}
function updateEndTimeLimits() {
    if (endDateInput.value == getActDate()) {
        endTimeInput.max = getLastMeasureTime();
    }
    else {
        endTimeInput.max = "23:55";
    }

    if (endDateInput.value == startDateInput.value) {
        endTimeInput.min = startTimeInput.value;
    }
    else {
        endTimeInput.min = "00:00";
    }
}

startDateInput.addEventListener('change', () => {
    endDateInput.min = startDateInput.value;
    updateStartTimeLimits();
    updateEndTimeLimits();
})
endDateInput.addEventListener('change', () => {
    startDateInput.max = endDateInput.value;
    updateStartTimeLimits();
    updateEndTimeLimits();
})

startTimeInput.addEventListener('change', () => {
    updateEndTimeLimits();
})
endTimeInput.addEventListener('change', () => {
    updateStartTimeLimits();
})

function calcHoursDiff() {
    var diffInS = Math.floor((endTime - startTime) / 1000);
    if (diffInS < 0) {
        console.error("Error in the input date");
        return;
    }

    return Math.ceil(diffInS / 3600);
}


function submitForm() {
    console.log("ok");
}

document.getElementById('form').addEventListener('submit', async function (e) {
    console.log("oui !");
    e.preventDefault();
    var fileList = getFileList();
    if (!dispModeMinMax) {
        await setAllTemperatures(fileList);
    }
})