# RAD2EXE

RAD2EXE is a command line tool and web app that allow you to convert your Reality Adlib Tracker 2 files into small 16 bit DOS executables, like you would need to enter the Revision Oldskool Music Compo.

If you're just looking for a way to make your `.rad` files into DOS executables and don't mind downloading executables from a questionable source (i.e. me), just head over to https://rad2exe.base13.de where this thing here is living.

## Installation

### Command Line Tool

The primary command line tool is `rad2exe`. Currently I only include the stuff needed to run it on Linux x86_64. After that it only needs a few things to be usable:

* a `g++` compiler that understands `-std=c++11`
* OpenWatcom DOS compiler for your host
  * You'll also need to set a bunch of environment variables to configure where to
    find the compiler and some of its settings:
    * `$WATCOM` needs to point to the root directory of your OpenWatcom installation
    * `$PATH` needs to contain the `binl64` directory of your Watcom installation
    * `$INCLUDE` needs to point to the `h` directory of your Watcom installation

Running the tool for the first time will build a small tool for validating the `.rad` file and creating a C header file from it.

### Web backend

The backend is written in `node.js` and needs a bunch of packages. In the `web/backend` directory, run

```npm install```

to install all the dependencies. You may also want to edit the paths in `rad2exe.js` to point to the correct places on your system:

* `exe_file_dir` to tell the backend where to put the compiled DOS executables (configure your static web server to serve from there!)
* `rad2exe` to point to the `rad2exe` executable in the root directory of this repo
* `url_prefix` to point to the relative path your webserver will serve the generated files from.

### Web frontend

Frontend configuration only means setting the correct URL in `convert_url`, where the frontend can find the backend server. In a typical setup you would proxy the backend behind your Apache or whatever server you are using. In my setup, I have the backend running on `localhost:4000` but use Apache's proxy module to make it available at `/convert`.

## Usage

### Command Line Tool

Assuming all environment variables are set correctly (see above), running the tool is straighforward:

```
rad2exe mysong.rad mysongplayer.exe
```

This will try to read the given song file and produce the player.

There are a few command line options:

* `-i` will make the converter ignore song file validation errors. For some reason, songs created with the DOS version of RAD tracker produce "file truncated" errors. The files typically work regardless, but using this option may make the player crash. Keep that in mind.
* `-l` will make the song loop instead of exit after one playthrough.
* `-c` will compress the created executable using the `upx` executable compressor

### Web Interface

Assuming the backend is configured correctly, just run it:

```
cd web/backend
node rad2exe.js
```

Also serve the files in `web/frontend` from your static file server. The interface is self-explanatory I think.

### Web API without frontend

You can also use this service without the web frontend. Just POST a JSON object to `/convert` (or directly to the port used in the backend, 4000 by default). The following fields are supported:

* `data`: (required) the base64 encoded contents of your .rad file
* `ignoreValidationErrors`: if `true`, the input file is not validated (may produce a non-working .exe!)
* `loop`: if `true`, the player will loop the song forever
* `compress`: if `true`, the created .exe will be compressed.


If everything goes well, you will get a `200` response containing the URL for the created .exe file in the `result` field of the JSON payload. (Note: this may change soon, such that the created file is included in the response). In case there is something wrong with your input file, you'll get a `400` response and a description of the error in the `error` field of the response's JSON payload.

Total POST request size limit is 512K.


## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## Acknowledgements
This tool uses lots of other things:

* [Reality Adlib Tracker 2](https://www.3eality.com/productions/reality-adlib-tracker) is the thing you'll need to make your songs in the first place.
* [OpenWatcom](https://github.com/open-watcom) is used to compile the executable for DOS
* The [upx](https://upx.github.io/) executable compressor is used to drastically reduce the size of the created files.
* The backend runs on [express](https://expressjs.com/).

## Author
Made by Florian Keßeler in 2019 - [link](https://base13.de)

## License
This project is licensed under the MIT License - see the LICENSE.md file for details
