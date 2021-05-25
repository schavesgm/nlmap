# CCP4 map denoiser -- GPU accelerated application

This repository contains a C++ implementation of a denoiser specifically implemented for
CCP4 format maps. 

# Building the denoiser `denoise_map`

The denoiser is written in `C++/CUDA`. Its entry point is `main.cu`. The header files
used by the denoiser are contained inside `include`. The actual implementation is included
inside `src`. The denoiser uses `gemmi` as a dependency, which can be found inside
`external`.  Besides the standard library, no other dependencies are used.

The denoiser uses a `Makefile` to compile. The code is compiled using `nvcc` with `c++17`
as the minimum standard. The compilation has been tested with a version of `nvcc` 11.3, 
V11.3.58. One can change the flags used by `nvcc` changing `CXXFLAGS`. Note that the flag
`--gpu-architecture` might need to be varied depending on the GPU used. Building the
denoiser is just a matter of using,

```bash
make
```

Information about the parameters used in the denoiser can be found by calling

```bash
./denoise_map --help
```

# Testing the results

In addition to a denoiser, the repository contains a shell script to test and monitor the
performance of the denoiser. The shell script that implements this pipeline is `pipeline`.
`pipeline` uses several [CCP4](https://www.ccp4.ac.uk/) applications, therefore, the CCP4
software must be installed in the system. The basic installation of CCP4 is sufficient.
One should be sure that the system in which `pipeline` is run contains the following
applications: `cfft`, `cad`, `ccp4-python`, `cinvfft` and `sigmaa`.

After processing the maps, `pipeline` calls several `python` scripts contained inside
`scripts/python`. These scripts post-process the log files output by `ccp4-python` to
monitor the performance of the denoiser. These scripts create several files and images.
The dependencies of these scripts are: `numpy`, `matplotlib`, `regex`, `json` and `gemmi`.

The results of the pipeline for a given protein are stored inside a folder named `out`.
Inside `out`, one can find the resulting useful maps under the folder
`out/data/{protein}/s*_h*_r*_p*`, where the last part specifies the parameters of the
simulation. Inside `out/log/{protein}/s*_h*_r*_p*`, some important plots and log files are
stored to monitor the performance of the denoiser.

One can run the pipeline in any directory. However, inside the directory in which
`pipeline` is run, there must be a folder called `data`. An example of the structure of 
`data` is found in the repository.

In order to know how to run `pipeline`, we can invoke its help by doing

```bash
./pipeline help
```

The help message contains all the needed information of the needed parameters.
