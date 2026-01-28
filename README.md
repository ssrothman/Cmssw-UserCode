# Cmssw-UserCode for EECs <!-- omit from toc -->

This package provides CMSSW software for running EEC analysis. This includes:

 - Event selection utilities for Z->mumu
 - Jet preselection utilities 
 - Support for calculation and writing to NanoAOD projected, resolved 3-point, and resolved 4-point EECs
 - Support for (highly-configurable) matching of tracks between GEN and RECO
 - Computation of EEC detector effects from the gen matching 
 - Useful NanoAOD tables 

Details on all of this are below

##  Table of contents <!-- omit from toc -->

- [1. setup instructions](#1-setup-instructions)
- [2. SimonJets](#2-simonjets)
- [3. Gen-matching](#3-gen-matching)
- [4. EEC calculations](#4-eec-calculations)
  - [4.1 Types of EEC observables:](#41-types-of-eec-observables)
    - [4.1.1 Projected EECs (referred to throughout the code as "proj")](#411-projected-eecs-referred-to-throughout-the-code-as-proj)
    - [4.1.2 Resolved three-point EECs (referred to throughout the code as "res3")](#412-resolved-three-point-eecs-referred-to-throughout-the-code-as-res3)
    - [4.1.3 Resolved four-point EECs (referred to throughout the code as "res4")](#413-resolved-four-point-eecs-referred-to-throughout-the-code-as-res4)
  - [4.2 EEC calculator data formats ("resultType")](#42-eec-calculator-data-formats-resulttype)
    - [4.2.1 Unbinned EECs (resultType="Unbinned")](#421-unbinned-eecs-resulttypeunbinned)
    - [4.2.2 Prebinned, unmerged EECs (resultType="Vector")](#422-prebinned-unmerged-eecs-resulttypevector)
    - [4.2.3 Prebinned, merged EECs (resultType="Array")](#423-prebinned-merged-eecs-resulttypearray)
  - [4.3 Running EEC calculations](#43-running-eec-calculations)
- [Running with crab](#running-with-crab)
- [Running with condor](#running-with-condor)
- [Configuration](#configuration)
- [EEC output dataformat](#eec-output-dataformat)


## 1. setup instructions

NB the version of the boost libraries that ships with CMSSW_10_26 does not have some of the functionality we need, so we have to trick the build system into using a local copy of boost 1.82.0. We only use the header-only libraries, so no need to actually build anything

```bash

# use central singularity environment
> cmssw-cc7

#setup CMSSW release area
> cmsrel CMSSW_10_6_26
> cd CMSSW_10_6_26/src/
> cmsenv
> git clone git@github.com:ssrothman/Cmssw-UserCode.git SRothman --branch EECs_106x

#setup usercode
> cd SRothman
> git submodule update --recursive --init

#setup boost library
> wget https://archives.boost.io/release/1.82.0/source/boost_1_82_0.tar.gz
> tar -xf boost_1_82_0.tar.gz #this takes a while. there is probably a better way to do this
> rm boost_1_82_0.tar.gz
```
Then, in a text editor:

edit $CMSSW_BASE/config/toolbox/slc7_amd64_gcc700/tools/selected/boost.xml such that:
 - BOOST_BASE = $CMSSW_BASE/src/SRothman/boost_1_82_0
 - INCLUDE = $BOOST_BASE
 - LIB = $BOOST_BASE
 - CMSSW_FWLITE_INCLUD_PATH = $BOOST_BASE

and similarly edit $CMSSW_BASE/config/toolbox/slc7_amd64_gcc700/tools/selected/boost_header.xml such that
 - BOSTHEADER_BASE = $CMSSW_BASE/src/SRothman/boost_1_82_0
 - INCLUDE = $BOOSTHEADER_BASE

Then, back in bash:

``` bash
#build
> cd $CMSSW_BASE/src
> scram setup #tell it to re-read the xml files and learn about the local copy of boost. When rebuilding later you don't need to do this
> scram b clean
> scram b -j8

#run
> cmsRun SRothman/Analysis/test/test_MC.py
```

## 2. SimonJets

## 3. Gen-matching

## 4. EEC calculations

The actual EEC calculations are implemented in the EEC backend submodule `EECs/src`. These are wrapped in CMSSW producers in `EECs/plugins`, and NANO table producers are also provided. 

### 4.1 Types of EEC observables:

There are three different kinds of EEC calculators:

#### 4.1.1 Projected EECs (referred to throughout the code as "proj")

 By default this calculates the projected EECs from 2-point through 6-point. 
 These are 1-dimensional histograms, binned in delta R ("R" in the code).
 This coordinate lives in the domain `0 < R < \infty`

#### 4.1.2 Resolved three-point EECs (referred to throughout the code as "res3")

 This calculates the resolved three-point EECs. 
 This is a three-dimensional histogram. The binning is according to equation (3) of https://arxiv.org/pdf/2201.07800
 The names of the coordinates in the code are:
  - "R" = `RL`
  - "r" = `\xi`
  - "c" = `\phi`
  
and they live in the domains:
 - `0 < R < \infty` 
 - `0 < r < 1`
 - `0 < c < pi/2`

#### 4.1.3 Resolved four-point EECs (referred to throughout the code as "res4")

This calculates the resolved four-point EECs. 
In principle the full four-point EECs live in a five-dimensional space. This is practically infeasible, so instead we have picked out three three-dimensional subspaces of this space. These are: the "dipole", the "tee", and the "triangle". Each of these lives in a 3-dimensional space with coordinates naned `R`, `r`, and `c`

### 4.2 EEC calculator data formats ("resultType")

The EEC calculator can run in three different modes, yielding output with different amounts of pre-applied binning.

#### 4.2.1 Unbinned EECs (resultType="Unbinned")

This mode keeps the most information, at the cost of the most computational expense (both in terms of memory/cpu in the calculation, but also disk space required for the resulting NANO files). In this mode the EECs are completely unbinned (any bin edges passed in the configuration are ignored), and a distinct row is created in the NANO tables for each distinct N-tuple of jet constituents. 

Take, for example, the three-point projected EEC in a four-particle jet. There are four three-tuples of particles, plus six ways to take pairs of particles, and four ways to take a single particle. There will therefore be 14 rows in the NANO table, with the format:
```csv
R       wt
<float> <float>
<float> <float>
<float> <float>
...
```
Note that there will be repeated values in the `R` column because we are binning in only the "largest" `R` coordinate, which can be the same in different triples of particles. The trivial example of this is that when we take all three particles to be the same, the `R` coordinate will always be zero, so there will be four entries with `R=0`. 

#### 4.2.2 Prebinned, unmerged EECs (resultType="Vector")

In this mode, the EECs are pre-binned in the angular coordinates, but a distinct row is still created for each distinct N-tuple of particles. Here the binning passed in the config is used, and the output values for the angular coordinates are integer bin indices (indexed such that underflow is bin 0, the first bin is bin 1, etc). 

For the same example scenario as above, there will again be 14 rows in the NANO table, this time with a format like
```csv
R       wt
<int>   <float>
<int>   <float>
<int>   <float>
...
```

#### 4.2.3 Prebinned, merged EECs (resultType="Array")

This mode has the most aggressive optimization, and can give dramatically smaller files on disk than the other modes. This is achieved by pre-binning in the angular coordinates (just as for the "vector" resulttype), and merging entries which fall in the same bin. The output to the NANO table is the zero-suppressed total histogram counts in each angular bin. 

For the same example scenario as above, there are at most 7 distinct angular separations (0, A-B, A-C, A-D, B-C, B-D, C-D), so there will be at most 7 rows in the NANO table. If any of these angular separations fall in the same R bin, or if any potential R bins are empty, then there will be fewer rows. The maximum number of rows in the NANO table for a given jet is the total number of R bins in the supplied binning. The output will be in the same format as the other prebinned mode:
```csv
R       wt
<int>   <float>
<int>   <float>
<int>   <float>
...
```

### 4.3 Running EEC calculations

A generic process modifier for setting up EEC calculations in data or MC is avilable in `SRothman/EECs/setupEEC.py`. This can be called as
``` python
from SRothman.EECs.setupEEC import setupEEC_MC
process = setupEEC_MC(process,
    name : str = "name to use in CMSSW (eg for naming NANO tables)",
    genMatch : str = "name (ie input tag) of genmatching producer",
    genjets : str = 'name (ie input tag) of simonjets producer for gen jets',
        config : dict = "configuration dictionary",
    recojets : str= 'name (ie input tag) of simonjets producer for gen jets',
    whichEEC : str = "one of 'proj', 'res3', 'res4'",
    verbose : int = "verbosity level (0 or 1 basically)",
)
```
or 
``` python
from SRothman.EECs.setupEEC import setupEEC_data
process = setupEEC_data(process,
    name : str = "name to use in CMSSW (eg for naming NANO tables)",
    recojets : str= 'name (ie input tag) of simonjets producer for gen jets',
    config : dict = "configuration dictionary",
    whichEEC : str = "one of 'proj', 'res3', 'res4'",
    verbose : int = "verbosity level (0 or 1 basically)",
)
```

This automatically sets up the correct CMSSW producers, and names everything consistently such that there are no name clashes. Note that it is possible to run multiple EEC calculations at once, just by calling the process modifier multiple times with different `whichEEC` values. An example of this can be found in `Analysis/test/test_MC.py` or `Analysis/test/test_data.py`.

## Running with crab

First, ensure that RUNNING_CRAB = True in Analysis/python/common_cmsRun.py

Then, cd Analysis/production/crab/

edit production_tag.py to be some unique name 

edit submit.py to point to an allocation you have at some T2 or T3

create a .yaml file with the datasets, config files, global tag, and crab config you want to use

run
```bash
> python submit.py -y <your yaml>
```
## Running with condor

There is a framework for running on condor in Analysis/production/condor. 

## Configuration

A half-hearted attempt has been made to consolidate configuration options into Analysis/python/config/config.json. This has not been entirely successful, and some options are currently hard-coded in some python cfi files. I'm not happy with this, but it's the current state of affairs. 

## EEC output dataformat

There are three options for how the EECs are computed and stored. They are:

1. "Unbinned": each N-tuple of particles contributes its own entry with floating-point EEC coordinates and weight
2. "Vector": each N-tuple of particles contributes its own entry with binned integer values for the EEC coordintes, and floating point weights
3. "Array": tuples sharing the same EEC coordinate bins are combined into a single entry (ie as though filling a histogram). The result is integer values for the EEC coordinates and floating point summed weights

The binning for the pre-binned coordinates is controlled in the EECs/python/<X>calcultor_cfi.py files

In any case the output in the NanoAOD flat table is a (zero-suppressed) list of (EEC coordinates, weight) entries. For the Vector and Array options the EEC coordinates are the integer bin indices, while for the Unbinned option they are floating-point. 
