var temperatures = [];
var times = [];

const data = {
    labels: times,
    datasets: [{
        label: 'My First dataset',
        backgroundColor: 'rgb(255, 99, 132)',
        borderColor: 'rgb(255, 99, 132)',
        data: temperatures,
    }]
};


const config = {
    type: 'line',
    data: data,
};

var temperaturesChart = new Chart(
    document.getElementById('chart'),
    config
);
