'use strict'

// C library API
const ffi = require('ffi');
const ref = require('ref');
var StructType = require('ref-struct');

// Express App (Routes)
const express = require("express");
const app = express();
const path = require("path");
const fileUpload = require('express-fileupload');

var GEDCOMerror = StructType({
  type: ref.types.int,
  line: ref.types.int
});

let GEDCOMobj = ref.refType("void");
let GEDCOMobjpointer = ref.refType(GEDCOMobj);

let mallocVal = ref.alloc(GEDCOMobjpointer);

let GEDCOMparser = ffi.Library('./GEDCOMparser', {
  'createGEDCOM': [GEDCOMerror, ['string', GEDCOMobjpointer]],
  'GEDCOMjson': ['string', [GEDCOMobjpointer]],
  'indListToJSON': ['string', [GEDCOMobjpointer]],
  'JSONtoGEDCOM': [GEDCOMobjpointer, ['string']],
  'writeGEDCOM': [GEDCOMerror, ['string', GEDCOMobjpointer]],
  'addingInd': [GEDCOMerror, [GEDCOMobjpointer, 'string', 'string']]

});


app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/', function (req, res) {
  res.sendFile(path.join(__dirname + '/public/index.html'));
});

// Send Style, do not change
app.get('/style.css', function (req, res) {
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname + '/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js', function (req, res) {
  fs.readFile(path.join(__dirname + '/public/index.js'), 'utf8', function (err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, { compact: true, controlFlowFlattening: true });
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function (req, res) {
  console.log('request upload');
  if (!req.files) {
    console.log('error');
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  if (fs.existsSync('./uploads/' + uploadFile.name)) {
    console.log('This is already exist bad file');
    return res.status(400).send('-No files were uploaded. already exist file name ' + uploadFile.name);
  }

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function (err) {
    if (err) {
      return res.status(500).send(err);
    }

    console.log('file uplaoded');

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function (req, res) {
  fs.stat('uploads/' + req.params.name, function (err, stat) {
    console.log(err);
    if (err == null) {
      res.sendFile(path.join(__dirname + '/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//Sample endpoint
app.get('/someendpoint', function (req, res) {
  res.send({
    foo: "bar"
  });
});

app.post('/checkValid', function (req, res) {
  console.log("Checking if its Valid File");

  var filename = req.files.uploadFile.name;

  let doublePtr = ref.refType(ref.refType("void"));
  let gedcomObject = ref.alloc(doublePtr);

  let error = GEDCOMparser.createGEDCOM('./uploads/' + filename, gedcomObject);

  if (error.type == '0') {
    console.log("it good");
    res.send({ data: "Successfully uploaded " + filename });
  }
  else {
    fs.unlink('./uploads/' + filename, function (err) {
      if (err) {
        console.log("not good");
      }
      return res.status(400).send('- Error Occured when uploading ' + filename + ' file');
    });
  }
});

app.get('/getFiles', function (req, res) {
  console.log("reading file");

  var val = 'Files';
  var filearr = {};
  filearr[val] = [];

  fs.readdir(__dirname + '/uploads', function (err, items) {
    var sizeoffile = items.length;
    for (var i = 0; i < sizeoffile; i++) {

      if (items[i].indexOf('.ged') != -1) {
        filearr[val].push({
          'File Name': items[i]
        });
      }
    }
    res.send(filearr);
  });

});


app.get('/getfileinfo', function (req, res) {
  console.log("creating gedcom for all files");
  var val = 'GEDCOM';
  var fileview = {};
  fileview[val] = [];

  fs.readdir(__dirname + '/uploads', function (err, items) {

    for (var i = 0; i < items.length; i++) {

      var gedstr;

      let doublePtr = ref.refType(ref.refType("void"));
      let gedcomObject = ref.alloc(doublePtr);

      //  console.log('Parsing: ' + items[i]);
      let error = GEDCOMparser.createGEDCOM('./uploads/' + items[i], gedcomObject);
      if (error.type == '0') {

        gedstr = GEDCOMparser.GEDCOMjson(ref.deref(gedcomObject));

        fileview[val].push({
          'File Name': items[i],
          json: JSON.parse(gedstr)
        });

      }
    }
    res.send(fileview);
  });

});

app.post('/getGedview', function (req, res) {
  console.log("creating GEDCOM View");
  var gedstr;

  let doublePtr = ref.refType(ref.refType("void"));
  let gedcomObject = ref.alloc(doublePtr);

  let error = GEDCOMparser.createGEDCOM('./uploads/' + req.body['File Name'], gedcomObject);
  
  if (error.type != '0') {
    return res.status(400).send('- Not able to Create GedCom');
  }

  gedstr = GEDCOMparser.indListToJSON(ref.deref(gedcomObject));

  var string;

  if (gedstr == '[]') {
    string = JSON.parse('[{"givenName":"No Individual present","surname":"","SEX":"","Number Family":""}]');
  }
  else {
    string = JSON.parse(gedstr);
  }
  res.send({ data: string });

});

app.post('/createGEDCOM', function (req, res) {
  console.log("creating GEDCOM");

  var nameoffile = JSON.parse(req.body['File Name']);

  let gedcomObject = ref.refType("void");
  gedcomObject = GEDCOMparser.JSONtoGEDCOM(req.body['Create GEDCOM']);
  let error = GEDCOMparser.writeGEDCOM('./uploads/' + nameoffile, gedcomObject);

  if (error.type != '0') {
    return res.status(400).send('-Did not Create the GEDCOM');
  }
  else {
    res.send({ data: "works fine" });
  }

});
app.post('/addindividual', function (req, res) {
  console.log("adding individual");

  var filename = JSON.parse(req.body['File Name']);
  console.log(filename);

  let doublePtr = ref.refType(ref.refType("void"));
  let gedcomObject = ref.alloc(doublePtr);

  let error = GEDCOMparser.createGEDCOM('./uploads/' + filename, gedcomObject);
  if (error.type != '0') {
    return res.status(400).send('-Did not Add the Individual');
  }
  let errorone = GEDCOMparser.addingInd(ref.deref(gedcomObject), req.body['Name'], './uploads/' + filename);
  if (errorone.type != '0') {
    return res.status(400).send('-Did not Add the Individual');
  }

  res.send({ data: "Successfully added Individual to " + filename });

});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);