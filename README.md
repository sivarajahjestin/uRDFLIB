# uRDFLib

RDF library designed to be an alternative to [RDFLib](https://rdflib.readthedocs.io/) for constrained devices.
It can be used both as a C native library, an Arduino/Platformio library or as a [MicroPython](https://docs.micropython.org/) module.

## Getting Started

uRDFLib depends on [`libcbor`](https://github.com/PJK/libcbor). 

### Linux

On Ubuntu, run:

```sh
sudo apt install libcbor-dev
```

See [download instructions](https://libcbor.readthedocs.io/en/latest/getting_started.html) for other platforms.

Then, run:

```sh
mkdir build
cd build
cmake .. # generate Makefile in /build
make # read Makefile and compile urdflib + test binaries
```

### Known Issue on OS X

After installing with `brew install libcbor`, add `libcbor` files to CMakeLists.txt as follows:

```
set(LIBCBOR_INCLUDE_DIRS "/usr/local/include")
set(LIBCBOR_LIBRARIES "/usr/local/lib/libcbor.a")
```

## Platformio

On an embedded device you can use [`platformio`](https://platformio.org), 
 we use a fork of [`libcbor`](https://gitlab.com/coswot/constrained-servient/libcbor) to prevent compilation errors.

```
lib_deps =
	git@gitlab.com:coswot/constrained-servient/urdflib.git
```

## Units tests

Tests are using [Unity](https://github.com/ThrowTheSwitch/Unity) they are located in  [test/](test/), they are run it Gitlab CI and when you use `make`.

## Example

### On desktop

An example program can be found in `example/coswot.c` which in turn will get compiled in the build directory after following the above steps. 

You may run it with:

```
./coswot
```

The example encodes the following graph in a uRDFLib buffer and stores buffer content to a file (CBOR format).
The output file, can then be decoded e.g. on [cbor.me](https://cbor.me/) for inspection.

```ttl
cosdataset:Com_ID
    a coswot:Communication ;
  	coswot:hasMedium cosdataset:COSSB_Servient_4ET_Office429_WhiteBoard;
  	coswot:hasCommunicator cosdataset:COSIO_Servient_4ET_Office429_WhiteBoard;
  	coswot:conveys cosdataset:Eval_TIX_4ET_Office429_CO2 ;
	coswot:isAbout "4ET_429_sensor1_CO2" ;
  	coswot:hasTimestamp "2022-10-26-T12:01:00"^^xsd:dateTime .

cosdataset:Eval_TIX_4ET_Office429_CO2
    a saref:Observation ;
	saref:madeBy cosdataset:4ET_Office429_WhiteBoard_CO2Sensor ;
    saref:hasResult [ saref:hasValue "1250"^^xsd:integer ] ;
    saref:resultTime "2022-10-26-T12:01:00"^^xsd:dateTime .
```

### On platformio

An example using the Arduino framework is provided in [`examples/Arduino/`](examples/Arduino/) to run it you can use [`pio ci`](https://docs.platformio.org/en/latest/core/userguide/cmd_ci.html):

```
pip install -U platformio
pio pkg install -g -l locroce/libcbor
pio ci --lib="." --board=esp32dev examples/Arduino
```

## Documentation

See the documentation of the [uRDFLib C API](https://coswot.gitlab.io/doc/urdflib-doxygen/html/index.html).

See also the [CoSWoT specification](https://docs.google.com/document/d/1m62Rr1ul3REZc1Fkp7_cHWluX3wxgS0y1hJXHy33L2E/edit?usp=sharing) to encode RDF data in the CBOR format. CBOR is used as the underlying format in uRDFLib.