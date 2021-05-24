async function getJsonData() {
  let res = await fetch("/dht");
  let data = await res.json();
  return data;
}

function loadSensorData(path) {
  const humidity = document.getElementById("humidity");
  const temperature = document.getElementById("temperature");
  const heatindex = document.getElementById("heatindex");

  getJsonData().then(data => {
    humidity.innerHTML = data.humidity.toFixed(2);
    temperature.innerHTML = data.temperature.toFixed(2);
    heatindex.innerHTML = data.heatindex.toFixed(2);
  });
}

setInterval(loadSensorData, 5000);