const express       = require("express");
const fs            = require("fs");
const body_parser   = require("body-parser");
const uuidv4        = require('uuid/v4');
const { spawnSync } = require('child_process');

// Configure server locations
const exe_file_dir = "/var/www/vhosts/rad2exe.base13.de/files/";
const rad2exe      = "/home/rad2exe/rad2exe/rad2exe";
const url_prefix   = "/files/";

const app = express();
app.use(body_parser.json({
  limit: "512kb",
}));


function getCommandLineArgs(body) {
  var args = [];

  if(body.ignoreValidationErrors) {
    args.unshift("-i");
  }

  if(body.loop) {
    args.unshift("-l");
  }

  if(body.compress) {
    args.unshift("-c");
  }

  return args;
}


app.post("/", async function (req, res, next) {
  const uuid = uuidv4()
  const src_file = "/tmp/rad2exe-" + uuid + ".rad";
  const res_name = uuidv4() + ".exe";
  const out_name = exe_file_dir + res_name;

  if(!req.body.data) {
    res.status(400);
    res.send({error: "No data field included"}).end();
    return;
  }

  const dataBuf = Buffer.from(req.body.data, 'base64');
  fs.writeFileSync(src_file, dataBuf);

  var args = getCommandLineArgs(req.body);
  Array.prototype.push.apply(args, [src_file, out_name]);

  const conversion = spawnSync(rad2exe, args);
  fs.unlinkSync(src_file);
  if(conversion.status != 0) {
    // Does stderr contain a message from the `prepare` tool?
    const validation_error = conversion.stderr
      .toString()
      .split(/\r?\n/)
      .filter(l => l.toString().startsWith("File validation error: ")
    );

    if(validation_error.length > 0) {
      const trimmed = validation_error[0].trim()
      res.status(400);
      res.send({error: trimmed});
      res.end();
    } else {
      // Something unexpected, just log it for review
      console.log(conversion.stdout.toString())
      console.log(conversion.stderr.toString())
      res.status(500).end();
    }
    return;
  }

  res.send({result: url_prefix + res_name});
  res.status(200).end();

});


app.listen(4000, "0.0.0.0");
