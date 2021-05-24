async function getJsonData(path) {
    let reqUrl = '/fs?path=' + path;

    let res = await fetch(reqUrl);
    let data = await res.json();

    return data;
}

function createTable(data, dir) {
    const tBody = document.getElementById('tbody');
    tBody.innerHTML = '';

    let tRows = '';
    let path;

    if (dir == '/') {
        path = dir;
    } else {
        path = dir + '/';
    }

    data.forEach((elem) => {
        tRows += "<tr>";
        if (elem.isdir) {
            tRows += "<td><a class='icon dir' href='/fm?path=" + path + elem.name + "'>" + elem.name + "</a></td>";
            tRows += "<td></td>";
            tRows += "<td>" + elem.modified + "</td>";
            tRows += "<td></td>";
        } else {
            tRows += "<td><a class='icon file' href='" + path + elem.name + "'>" + elem.name + "</a></td>";
            tRows += "<td>" + elem.size + "B</td>";
            tRows += "<td>" + elem.modified + "</td>";
            tRows += "<td><a href='" + path + elem.name + "' download>Download</a></td>";
        }
        tRows += "</tr>";
    });
    tBody.innerHTML += tRows;
}

function setTitleText(directory) {
    const htext = document.getElementById("header__text");
    const title = document.getElementById("title__text");
    let titleText = (directory == '') ? '/' : directory;
    htext.innerHTML = "Index of " + titleText;
    title.innerText = title.innerText.replace("LOCATION", titleText);
}

function onHasParentDirectory(parentDir) {
    const box = document.getElementById("parentDirLinkBox");
    box.style.display = "block";
    const link = document.getElementById("parentDirLink");
    link.href = "/fm?path=" + parentDir;
}

function initFileManager() {
    const urlParams = new URLSearchParams(window.location.search);

    let directory;

    if (urlParams.has("path")) {
        if (urlParams.get('path') == '') {
            directory = '/';
        }
        else {
            directory = urlParams.get('path');
        }
    } else {
        directory = '/';
    }

    if (directory != '/') {
        let parentDirectory = directory.split('/').slice(0, -1).join('/');

        if (!parentDirectory.length) {
            parentDirectory = '/';
        }

        onHasParentDirectory(parentDirectory);
    }

    getJsonData(directory).then(data => {
        createTable(data, directory);
    });

    setTitleText(directory);
}

window.addEventListener('DOMContentLoaded', initFileManager());