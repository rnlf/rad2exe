var express = require("express");
const fs = require("fs");
var body_parser = require("body-parser");
const uuidv4 = require('uuid/v4');
const { spawnSync } = require('child_process');

const exe_file_dir = "/var/www/vhosts/rad2exe.base13.de/files/";
const rad2exe = "/home/rad2exe/rad2exe/rad2exe";
const log_dir = "/home/rad2exe/log";

var app = express();
app.use(body_parser.json({
  limit: "512kb",
}));


app.post("/", async function (req, res, next) {
  var uuid = uuidv4()
  var src_file = "/tmp/" + uuid + ".rad";
  var res_name = uuidv4() + ".exe";
  var out_name = exe_file_dir + res_name;


  if(!req.body.data) {
    res.status(400);
    res.send({error: "No data field included"}).end();
    return;
  }

  var dataBuf = Buffer.from(req.body.data, 'base64');
  fs.writeFileSync(src_file, dataBuf);

  var args = [src_file, out_name];
  if(req.body.ignoreValidationErrors) {
    args.unshift("-i");
  }

  if(req.body.loop) {
    args.unshift("-l");
  }

  if(req.body.compress) {
    args.unshift("-c");
  }

  var conversion = spawnSync(rad2exe, args);
  if(conversion.status != 0) {
    var validation_error = conversion.stderr.toString().split(/\r?\n/).filter(l => l.toString().startsWith("File validation error: "));
    if(validation_error.length > 0) {
      var trimmed = validation_error[0].trim()
      res.status(400);
      res.send({error: trimmed});
      res.end();
    } else {
      console.log(conversion.stdout.toString())
      console.log(conversion.stderr.toString())
      res.status(500).end();
    }
    return;
  }

  res.send({result: "/files/" + res_name});
  res.status(200).end();

});


app.listen(4000, "0.0.0.0", () => { });
