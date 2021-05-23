const urlParams = new URLSearchParams(window.location.search);
var directory = "";
var parentDirectory = "";
var requestUrl = "";

function setWorkingDirectory() {
    if (urlParams.has("path")) {
        if(urlParams.get('path')=="/"){
            directory = "";
        }
        else {
            directory = urlParams.get('path');
        }
    } else {
        directory = "";
    }
    parentDirectory = directory.split("/").slice(0, -1).join("/");
    requestUrl = "/fs?path=" + directory;
}

function buildTable(data) {
    var tBody = document.getElementById('tbody');
    var tRows = "";
    data.forEach((elem) => {
        tRows += "<tr>";
        if (elem.isdir) {
            tRows += "<td><a class='icon dir' href='/fm?path=" + directory + "/" + elem.name + "'>" + elem.name + "</a></td>";
            tRows += "<td></td>";
            tRows += "<td>" + elem.modified + "</td>";
            tRows += "<td></td>";
        } else {
            tRows += "<td><a class='icon file' href='" + directory + "/" + elem.name + "'>" + elem.name + "</a></td>";
            tRows += "<td>" + elem.size + "B</td>";
            tRows += "<td>" + elem.modified + "</td>";
            tRows += "<td><a href='" + directory + "/" + elem.name + "' download>Download</a></td>";
        }
        tRows += "</tr>";
    });
    tBody.innerHTML += tRows;
}

function createTitle() {
    var htext = document.getElementById("htext");
    var title = document.getElementById("title");
    var titleText = (directory=="") ? "/" : directory;
    htext.innerHTML = "Index of " + titleText;
    title.innerText = title.innerText.replace("LOCATION", titleText);
}

function onHasParentDirectory() {
    var box = document.getElementById("parentDirLinkBox");
    box.style.display = "block";
    var link = document.getElementById("parentDirLink");
    link.href = "/fm?path=" + parentDirectory;
}

function createTable() {
    createTitle();
    if (directory!="") onHasParentDirectory();
    fetch(requestUrl).then(res => { res.json().then(data => { buildTable(data); }) });
}

window.addEventListener('DOMContentLoaded', setWorkingDirectory());
window.addEventListener('DOMContentLoaded', createTable());