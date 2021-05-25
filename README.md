# CCP4 map denoiser

This repository contains a C++ implementation of a denoiser specifically implemented
for CCP4 format maps. A testing pipeline to monitor the performance of the denoiser can
also be found.

# Non-local means denoiser
The repository contains a non-local means denoiser implementation written in
`C++`.  The entry point of the code is `main.cpp`. 

The denoiser uses a `Makefile` as the build system. The code can be compiled using
`g++` with `c++14` as the minimum C++ standard tested. The older version of `g++`
tested is 6.3.0.

The denoiser can be built by invoking
```bash
make
```

Information about how to use the denoiser can be found by invoking
```bash
./denoise_map --help
```

## Dependencies
`denoise_map` does not use any external dependencies. The only external dependency is
`gemmi`. However, the library is include in the repository. 

# Testing/monitoring the denoiser
In addition to the denoiser, this repository contains `pipeline`; a shell script to
test and monitor the performance of the denoiser. The pipeline processes a given
protein contained in folder called `data` and produces several builds of the same
protein. The `data` folder must be located in the same directory where `pipeline` is
invoked. The repository contains and example of the structure that data `data` should
have. Each processing generates three different builds of the same protein: one for
the denoised version of map, obtained after applying `denoise_map` to the original
map; another one for the `noisy` version of the map, obtained after adding some noise
to the original map; finally, two more maps are analysed. One for the original .mtz
file and another one for the original .map extracted from the .mtz file.

After processing the maps, `pipeline` calls several python scripts contained inside
`scripts/python`. These scripts post-process the maps to obtain some metrics to
monitor the performance of the denoiser.

The results of the pipeline are stored inside a folder named `out`, created in the
directory where `pipeline` was invoked. The results are stored in a directory tree,
grouping all runs for a given protein under the same folder.

Information about the input parameters used in `pipeline` and an example can be found
by using,
```bash
./pipeline help
```

The help message contains all the needed information of the needed parameters.

## Dependencies
`pipeline` uses the following dependencies:

 - Python modules:
    - numpy
    - matplotlib
    - json
    - regex
    - gemmi
 - [CCP4 software](https://www.ccp4.ac.uk/)
    - cfft
    - cad
    - ccp4-python
    - cinvfft
    - sigma
 - Utilities:
    - convert (ImageMagick)
