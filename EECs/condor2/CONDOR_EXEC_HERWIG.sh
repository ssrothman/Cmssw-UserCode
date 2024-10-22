#!/bin/bash

set -euxo pipefail

echo "STARTED JOB"
export SCRAM_ARCH=slc7_amd64_gcc700
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
export CMS_PATH=/cvmfs/cms.cern.ch
export SITECONFIG_PATH=/cvmfs/cms.cern.ch/SITECONF/local
export CVSROOT=:gserver:cmssw.cvs.cern.ch:/local/reps/CMSSW
export MANPATH=/cvmfs/cms.cern.ch/share/man:/cvmfs/cms.cern.ch/share/man
#cmssw-cc7
source /cvmfs/cms.cern.ch/cmsset_default.sh
echo "SETUP ENV"
xrdcp root://cmseos.fnal.gov//store/user/srothman/CMSSW_10_6_26.tar.gz .
echo "GOT TARBALL"
tar -xf CMSSW_10_6_26.tar.gz
cd CMSSW_10_6_26/src
cmsenv
eval `scramv1 runtime -sh`
scram b ProjectRename
scram b clean
scram b -j4
cmsenv

mkdir config
xrdcp root://cmseos.fnal.gov//store/user/srothman/config.tar.gz config/
cd config
tar -xf config.tar.gz
cd ../

echo "READY TO RUN"

cmsRun config/test_EEC_genonly.py inputjson=config/filelists/herwig_1000.json index=$1
echo "DONE RUNNING"

xrdcp NANO_$1.root root://cmseos.fnal.gov//store/user/srothman/herwig/
xrdcp dropped_$1.root root://cmseos.fnal.gov//store/user/srothman/herwig/
