# Cmssw-UserCode

EEC analysis user code

git submodule update --recursive --init

need to apply the patch in patch
download a local copy of boost 1.82
need to update the $CMSSW_BASE/config/toolbox/slc7_amd64_gcc700/tools/selected/boost* to point the local copy of boost

do 

scram update
scram b clean
scram b 

# setup instructions

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

# Running with crab

First, ensure that RUNNING_CRAB = True in Analysis/python/common_cmsRun.py

Then, cd Analysis/production/crab/

edit production_tag.py to be some unique name 

edit submit.py to point to an allocation you have at some T2 or T3

create a .yaml file with the datasets, config files, global tag, and crab config you want to use

run
```bash
> python submit.py -y <your yaml>
```

# Configuration

A half-hearted attempt has been made to consolidate configuration options into Analysis/python/config/config.json. This has not been entirely successful, and some options are currently hard-coded in some python cfi files. I'm not happy with this, but it's the current state of affairs. 

# EEC output dataformat

There are three options for how the EECs are computed and stored. They are:

1. "Unbinned": each N-tuple of particles contributes its own entry with floating-point EEC coordinates and weight
2. "Vector": each N-tuple of particles contributes its own entry with binned integer values for the EEC coordintes, and floating point weights
3. "Array": tuples sharing the same EEC coordinate bins are combined into a single entry (ie as though filling a histogram). The result is integer values for the EEC coordinates and floating point summed weights

The binning for the pre-binned coordinates is controlled in the EECs/python/<X>calcultor_cfi.py files

In any case the output in the NanoAOD flat table is a (zero-suppressed) list of (EEC coordinates, weight) entries. For the Vector and Array options the EEC coordinates are the integer bin indices, while for the Unbinned option they are floating-point. 
