---
author: Sergio Chaves García-Mascaraque
title:  Output JSON log format for protein maps denoising builds.
geometry: margin=30mm
---
The script `./json_log.py` takes a protein folder as input and produces
a JSON log file. To run the script, simply use:

```bash
python ./json_log.py {path_to_protein}
```

The script will output a file named `{protein_name}_log.json` inside
the `log` directory. The JSON file contains the basic information to
categorise and monitorise the builds. Basically, it consists on 4
entries in a python dictionary. From now on, bold text inside brackets
([__text__]) corresponds to an key in the dictionary. 

The number of base keys in the dictionary is prone to change.

## 1. Protein (string).
Name of the analysed protein. This is the basename of the
`{path_to_protein}` path.

## 2. Space group (string).
Space group for the given protein.

## 3. Resolution (string).
Resolution of the given protein

## 4. Builds (dictionary).
The entry named [__builds__] is a dictionary containing, at least, two
entries. Each entry in [__builds__] contains some monitoring parameters from
a Buccaneer building process. The first entry is called [__refmtz__] and
contains the building results for the reference build generated through
the original `.mtz` file. The second entry is called [__refmap__] and
contains the results for the reference build generated through the map
`.map` file obtained from the original `.mtz` file. The other possible
entries are different postprocessed maps, for example, a version of the
original `.map` file with added noise or a denoised version of the
original `.map` file.

Each entry in [__builds__] contains some dictionaries. The first one is
called [__params__] and some parameters extracted from the Buccanneer 
pipeline are displayed inside. These parameters are [__Rw__], [__Rf__], 
the rms length [__rms_L__], the rms angle [__rms_A__] and the rms volume 
[__rms_V__]. These parameters are extracted from a Buccaneer log file. 
Each entry is parsed from the following block of text,

>                       Initial    Final
>            R factor    0.3345   0.2531
>              R free    0.3353   0.2889
>      Rms BondLength    0.0207   0.0069
>       Rms BondAngle    2.3838   1.8318
>      Rms ChirVolume    0.1174   0.1449

For each parameter shown in the block of text above, we parse the
initial and final value. Therefore, for the key [__Rw__] which
corresponds to `R factor`, two entries can be found under the name of
[__o__] and [__f__]. [__o__] corresponds to the initial value and
[__f__] to the final value. Each of these initial and final value
entries points to a list containing several values for each iteration of
the Buccanner processing pipeline. For example, if Buccaneer has done 5
iterations of its own pipeline, then we could explore the values of `R
free` for each iteration by going to the key [__Rf__]. From there, if we
wanted to know the final values, we just need to access the [__f__]
key in the dictionary. Accessing [__f__] would provide a list of `Nc`
examples.

The second entry inside a given build is called [__residues__]. It
contains information about the building process of buccaneer. Inside
[__residues__], there are several entries: [__resb__], [__prunning__],
[__chains__], [__%_build__] and [__%_chain__]. Each of them corresponds, in
order, to the numbers extracted from the following sentences in a
Buccaneer log file,

> 166 residues were built in 9 fragments, the longest having 44 residues. \
> 135 residues were sequenced, after pruning. \
> 142 residues were uniquely allocated to 2 chains. \
> Completeness by residues built:    85.5% \
> Completeness of chains (number):   74.0%    (2) \

For example, [__resb__] contains a list for each Buccaneer cycle. The list
is composed by three elements `[r, f, l]`, where `r` is the number of
residues (166 in the example above), `f` is the number of fragems (9) in
the example above and `l` is the longest example (44 in the example
above). The same logic is used for the other entries in __residues__.

In the builds coming from `.map` files, there is another entry called
[__sigma__]. This entry contains a dictionary with values related to 
the level of noise in the map. Each key in the dictionary corresponds to
a given `D limit`, which is a measure of the distance. For each of these
`D limit` keys, represented with a string with pattern `(r_max, r_min)`,
there are several entries. Each of these keys contains a dictionary. 
We parse the dictionary from a block of text similar to,

>                     OLD                NEW       MEAN
>       D Limits     SIGMAA     Shift    SIGMAA    W
>     49.97--10.92   0.98820   -0.00985  0.97835   0.85496
>     10.92-- 7.82   0.97993   -0.02773  0.95220   0.87391
>      7.82-- 6.41   0.95209   -0.03389  0.91820   0.73430

The key is the first column in the table. The entries are labelled as
`SIGMAA`, `SHIFT`, `NEW_SIGMAA` and `MEAN_W`; they correspond to the
columns first to fifth in the table above.
