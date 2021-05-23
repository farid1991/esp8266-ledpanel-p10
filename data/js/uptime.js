setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var jsonTime = JSON.parse(this.responseText);
      var upTime = "System uptime: " + jsonTime.d + " Days, " + jsonTime.h + ":" + jsonTime.m + ":" +jsonTime.s;
      document.getElementById("uptime").innerHTML = upTime;
    }
  };
  xhttp.open("GET", "/uptime", true);
  xhttp.send();
}, 1000);