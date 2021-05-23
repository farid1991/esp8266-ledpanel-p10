setInterval(function () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
        var jsonDht = JSON.parse(this.responseText);
        document.getElementById("humidity").innerHTML = jsonDht.humidity;
        document.getElementById("temperature").innerHTML =
          jsonDht.temperature;
      }
    };
    xhttp.open("GET", "/dht", true);
    xhttp.send();
  }, 2000);