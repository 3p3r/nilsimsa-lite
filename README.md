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

## Example Usage
```C
// input text to be hashed
const char data[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam cursus lacinia condimentum. Integer feugiat tortor sed massa lobortis eleifend vel eu diam. Nullam libero ligula, accumsan eu sollicitudin et, rhoncus id lorem. Etiam lacinia erat id feugiat pulvinar. Nulla scelerisque, libero vel commodo malesuada, lorem mi ultricies arcu, eget rutrum lorem velit sed ipsum. Cras ac tellus in mauris eleifend elementum. Curabitur tempor sed massa sed dignissim.";

// -1 to compensate for trailing null character
int data_size = (sizeof(data) / data(data[0])) - 1;

// pre-allocate hash's string
char hash[65] = { 0 };
nilsimsa_compute(data, data_size, hash);

// hash is now:
5DF842A0C27C234DF170AB95440675C10E82294B181438D87D09CE4CB7C4F47E
```

## API Documentation
 - `nilsimsa_compute`
 > computes the Nilsimsa hash of the given data. Returns a 64 character long string representing the digest.

 - `nilsimsa_compare`
 > compares two given Nilsimsa hashes. Returns a score between -127 and 128 where -127 means completely uncorrelated data and 128 means same data.

## Notes
This is a minimal port of [rholder/nilsimsa](https://github.com/rholder/nilsimsa) to C.
The reference C implementation can be found [here](http://ixazon.dynip.com/~cmeclax/nilsimsa.html).
