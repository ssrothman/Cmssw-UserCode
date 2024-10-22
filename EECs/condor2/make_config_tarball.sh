#!/bin/bash

tar -cvf config.tar.gz *
xrdcp config.tar.gz root://cmseos.fnal.gov//store/user/srothman/

