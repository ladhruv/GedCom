// Put all onload AJAX calls here, and event listeners
$(document).ready(function () {
    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/someendpoint',   //The server endpoint we are connecting to
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */

            //We write the object to the console to show that the request was successful
            console.log(data);
        },
        fail: function (error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });

    // displayFileLog();

    // Event listener form replacement example, building a Single-Page-App, no redirects if possible
    $('#someform').submit(function (e) {
        e.preventDefault();
        $.ajax({});
    });
});

function getInput() {

    var filenm = document.getElementById("fil").value;
    var source = document.getElementById("user").value;
    var version = document.getElementById("usr").value;
    var encode = document.getElementById("encode").value;
    var submitname = document.getElementById("subnames").value;
    var submitadd = document.getElementById("subaddress").value;

    var final = { "source": source, "gedcversion": version, "encoding": encode, "submitterName": submitname, "submitterAddress": submitadd };
    var jsonstring = JSON.stringify(final);
    var filestr = JSON.stringify(filenm);

    var formData = new FormData();
    formData.append('File Name', filestr);
    formData.append('Create GEDCOM', jsonstring);

    $.ajax({
        type: 'post',
        url: '/createGEDCOM',
        data: formData,
        processData: false,
        contentType: false,

        success: function (data) {

            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode("-Successfully created a gedcom");
            statusinput.appendChild(content);
            status.appendChild(statusinput);

            displayFileLog();
        },

        error: function (error) {
            let err = error['responseText'];
            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode(err);
            statusinput.appendChild(content);
            status.appendChild(statusinput);
        }
    });
}
function checkforvalidfile() {
    var file = document.getElementById("upload-file").files;
    var formData = new FormData();
    formData.append('uploadFile', file[0], file[0].name);

    $.ajax({
        type: 'post',
        url: '/checkValid',
        data: formData,
        processData: false,
        contentType: false,
        success: function (data) {
            var stat = data['data'];

            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode(stat);
            statusinput.appendChild(content);
            status.appendChild(statusinput);

            displayFileLog();

        },
        error: function (error) {

            var stat = error['responseText'];
            console.log(stat);

            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode(stat);
            statusinput.appendChild(content);
            status.appendChild(statusinput);
        }
    });
}
function uploadfile() {
    var files = document.getElementById("upload-file").files;
    console.log(files);

    var formData = new FormData();
    formData.append('uploadFile', files[0], files[0].name);

    $.ajax({
        type: 'post',
        url: '/upload',
        data: formData,
        processData: false,
        contentType: false,

        success: function (data) {

            console.log('Success');

            checkforvalidfile();

        },

        error: function (error) {
            var stat = error['responseText'];
            console.log(stat);
            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode(stat);
            statusinput.appendChild(content);
            status.appendChild(statusinput);
        }
    })
}
function showaFile() {
    var div1 = document.getElementById("gedView");

    while (div1.firstChild != null) {
        div1.removeChild(div1.lastChild);
    }

    $.ajax({
        type: 'get',
        url: '/getFiles',
        dataType: 'json',

        success(data) {

            var files = document.getElementById("selectedFile");

            while (files.firstChild != null) {
                files.removeChild(files.lastChild);
            }

            var filename = data['Files'];

            console.log("success");

            for (var i = 0; i < filename.length; i++) {

                var option = document.createElement("option");

                var file = filename[i];
                var jsonstr = file['File Name'];

                option.appendChild(document.createTextNode(jsonstr));
                files.appendChild(option);
            }

        },
        error: function (error) {
            console.log(error);
            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode("-There was a error getting all the files");
            statusinput.appendChild(content);
            status.appendChild(statusinput);
        }
    });
}

function getSelectedFile() {

    var filename = document.getElementById("selectedFile");
    var formData = new FormData();

    formData.append('File Name', filename.value);

    $.ajax({
        type: 'post',
        url: '/getGedview',
        data: formData,
        processData: false,
        contentType: false,

        success(data) {

            var div1 = document.getElementById("gedView");

            while (div1.firstChild != null) {
                div1.removeChild(div1.lastChild);
            }

            var indinfo = data['data'];
            console.log(data['data']);

            for (var i = 0; i < indinfo.length; i++) {

                var eachinfo = indinfo[i];
                var row = document.createElement("tr");

                for (var j in eachinfo) {
                    console.log(eachinfo[j]);
                    var innercell = document.createElement("th");
                    innercell.appendChild(document.createTextNode(eachinfo[j]));
                    row.appendChild(innercell);
                }
                div1.appendChild(row);
            }
            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode("-Created GEDCOM View for the Selected file Successfully");
            statusinput.appendChild(content);
            status.appendChild(statusinput);


        },
        error: function (error) {
            let err = error['responseText'];
            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode(err);
            statusinput.appendChild(content);
            status.appendChild(statusinput);
        }
    });
}
function displayFileLog() {

    $.ajax({
        type: 'get',
        url: '/getfileinfo',
        dataType: 'json',
        success: function (data) {
            console.log("successfull");

            var div1 = document.getElementById("filelogview");

            while (div1.firstChild != null) {
                div1.removeChild(div1.lastChild);
            }

            var files = data['GEDCOM'];

            for (var i = 0; i < files.length; i++) {

                var fileData = files[i];

                var fileName = fileData['File Name'];

                var jsonData = fileData['json'];

                var row = document.createElement("tr");
                var cell = document.createElement("th");

                var linkfile = fileData['File Name'].link('uploads/' + fileData['File Name']);
                cell.innerHTML = linkfile;

                row.appendChild(cell);

                for (var r in jsonData) {
                    var innerCell = document.createElement("th");
                    innerCell.appendChild(document.createTextNode(jsonData[r]));
                    row.appendChild(innerCell);
                }

                div1.appendChild(row);
            }
            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode("-Created File log View for the all the files Successfully");
            statusinput.appendChild(content);
            status.appendChild(statusinput);
        },
        error: function (error) {
            console.log('fail');
            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode("-There was a error creating File Log view");
            statusinput.appendChild(content);
            status.appendChild(statusinput);
        }
    });
}

function showaFiles() {

    document.getElementById("add-indiv").reset();

    $.ajax({
        type: 'get',
        url: '/getFiles',
        dataType: 'json',

        success: function (data) {

            var files = document.getElementById("fileoption");
            while (files.firstChild != null) {
                files.removeChild(files.lastChild);
            }
            var filename = data['Files'];

            console.log("success");

            for (var i = 0; i < filename.length; i++) {

                var option = document.createElement("option");

                var file = filename[i];
                var jsonstr = file['File Name'];

                option.appendChild(document.createTextNode(jsonstr));
                files.appendChild(option);
            }

        },
        error: function (error) {
            console.log(error);
            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode("-There was a error getting all the files");
            statusinput.appendChild(content);
            status.appendChild(statusinput);
        }
    });
}
function addindividual() {

    var filename = document.getElementById("fileoption").value;

    var givenName = document.getElementById("givnname").value;
    var lastName = document.getElementById("surname").value;

    var final = { "givenName": givenName, "surname": lastName };

    var jsonstring = JSON.stringify(final);
    var filestr = JSON.stringify(filename);

    console.log(filestr);
    var formData = new FormData();
    formData.append('File Name', filestr);
    formData.append('Name', jsonstring);

    $.ajax({
        type: 'post',
        url: '/addindividual',
        data: formData,
        processData: false,
        contentType: false,
        success: function (data) {
            var stat = data['data'];

            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode(stat);
            statusinput.appendChild(content);
            status.appendChild(statusinput);

            displayFileLog();
        },
        error: function (error) {
            let err = error['responseText'];
            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode(err);
            statusinput.appendChild(content);
            status.appendChild(statusinput);
        }
    });
}
function clearstatus() {
    var status = document.getElementById("status");

    while (status.firstChild != null) {
        status.removeChild(status.lastChild);
    }

}
function myFunction() {
    document.getElementById("create-gedcom").reset();
}

function myFunction2() {
    document.getElementById("get-des").reset();
    $.ajax({
        type: 'get',
        url: '/getFiles',
        dataType: 'json',

        success: function (data) {

            var files = document.getElementById("listoffile");
            while (files.firstChild != null) {
                files.removeChild(files.lastChild);
            }
            var filename = data['Files'];

            var status = document.getElementById("desinfo");
            while (status.firstChild != null) {
                status.removeChild(status.lastChild);
            }
            console.log("success");

            for (var i = 0; i < filename.length; i++) {

                var option = document.createElement("option");

                var file = filename[i];
                var jsonstr = file['File Name'];

                option.appendChild(document.createTextNode(jsonstr));
                files.appendChild(option);
            }

        },
        error: function (error) {
            console.log(error);
            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode("-There was a error getting all the files");
            statusinput.appendChild(content);
            status.appendChild(statusinput);
        }
    });
}
function getDes() {
    var name = document.getElementById("name").value;
    var filename = document.getElementById("listoffile").value;

    var status = document.getElementById("desinfo");

    var statusinput = document.createElement("p");

    var content = document.createTextNode("GENERATION #1:");
    statusinput.appendChild(content);
    var content1 = document.createTextNode("Individual1,Individual2,Individual3 ");
    statusinput.appendChild(content1);

    var content2 = document.createTextNode("GENERATION #2:");
    statusinput.appendChild(content2);
    var content3 = document.createTextNode("Individual1,Individual2,Individual3 ");
    statusinput.appendChild(content3);
    status.appendChild(statusinput);
}

function myFunction3() {
    document.getElementById("get-ans").reset();
    $.ajax({
        type: 'get',
        url: '/getFiles',
        dataType: 'json',

        success: function (data) {

            var files = document.getElementById("filelistss");

            while (files.firstChild != null) {
                files.removeChild(files.lastChild);
            }
            var status = document.getElementById("ancestor");
            while (status.firstChild != null) {
                status.removeChild(status.lastChild);
            }
            var filename = data['Files'];

            console.log("success");

            for (var i = 0; i < filename.length; i++) {

                var option = document.createElement("option");

                var file = filename[i];
                var jsonstr = file['File Name'];

                option.appendChild(document.createTextNode(jsonstr));
                files.appendChild(option);
            }

        },
        error: function (error) {
            console.log(error);
            var status = document.getElementById("status");
            var statusinput = document.createElement("p");

            var content = document.createTextNode("-There was a error getting all the files");
            statusinput.appendChild(content);
            status.appendChild(statusinput);
        }
    });
}
function getAns() {
    var name = document.getElementById("name").value;
    var filename = document.getElementById("filelistss").value;

    var status = document.getElementById("ancestor");

    var statusinput = document.createElement("p");

    var content = document.createTextNode("GENERATION #1:");
    statusinput.appendChild(content);
    var content1 = document.createTextNode("Individual1,Individual2,Individual3 ");
    statusinput.appendChild(content1);

    var content2 = document.createTextNode("GENERATION #2:");
    statusinput.appendChild(content2);
    var content3 = document.createTextNode("Individual1,Individual2,Individual3 ");
    statusinput.appendChild(content3);
    status.appendChild(statusinput);
}