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

