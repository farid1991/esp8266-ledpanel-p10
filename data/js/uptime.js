async function getJsonData() {
  let res = await fetch('/uptime');
  let data = await res.json();
  return data;
}

function getUptimeData() {
  const footer__uptime = document.getElementById('uptime');
  getJsonData().then(data => {
    let upTime = 'System uptime: ' + data.d + ' Days, ' + data.h + ':' + data.m + ':' + data.s;
    footer__uptime.innerHTML = upTime;
  });
}

setInterval(getUptimeData, 1000);