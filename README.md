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
> git clone git@github.com:ssrothman/Cmssw-UserCode.git SRothman --branch EECs_106x

#setup usercode
> cd SRothman
> git submodule update --recursive --init

#setup boost library
> wget https://archives.boost.io/release/1.82.0/source/boost_1_82_0.tar.gz
> tar -xf boost_1_82_0.tar.gz #this takes a while. there is probably a better way to do this
> rm boost_1_82_0.tar.gz

# edit $CMSSW_BASE/config/toolbox/slc7_amd64_gcc700/tools/selected/boost.xml
# such that 
# BOOST_BASE = $CMSSW_BASE/src/SRothman/boost_1_82_0
# INCLUDE = $BOOST_BASE
# LIB = $BOOST_BASE
#
# and similarly edit $CMSSW_BASE/config/toolbox/slc7_amd64_gcc700/tools/selected/boost_header.xml
# such that
# BOSTHEADER_BASE = $CMSSW_BASE/src/SRothman/boost_1_82_0
# INCLUDE = $BOOSTHEADER_BASE

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


