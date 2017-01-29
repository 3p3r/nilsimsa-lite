# nilsimsa-lite
A minimal C implementation of the Nilsimsa algorithm

## What is Nilsimsa?
In its most simplistic explanation, Nilsimsa produces the same hash if the data it represents is "almost" identical. Compare this to cryptographic hash functions (such as MD5) where hashes are identical iff the data it represents are identical.

## Build
There is no dependency in the source. Unit tests require `<assert.h>` and `<string.h>`. To build:

 * Linux
```Bash
$> cd <root of the repo>
$> mkdir build && cd build
$> cmake ..
$> make
```

* Windows
```Batch
$> cd <root of the repo>
$> mkdir build && cd build
$> cmake .. -G "Visual Studio 14 Win64"
```
Open up the solution and build.

## API Documentation
 - `nilsimsa_compute`
 > computes the Nilsimsa hash of the given data. Returns a 64 character long string representing the digest.

 - `nilsimsa_compare`
 > compares two given Nilsimsa hashes. Returns a score between -127 and 128 where -127 means completely uncorrelated data and 128 means same data.

## Notes
This is a minimal port of [rholder/nilsimsa](*%20https://github.com/rholder/nilsimsa) to C.
The reference C implementation can be found [here](http://ixazon.dynip.com/~cmeclax/nilsimsa.html).
