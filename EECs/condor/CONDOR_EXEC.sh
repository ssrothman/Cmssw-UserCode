#!/bin/bash

set -euxo pipefail

echo "Starting job on " `date` #Print the time and date
echo "Running on: `uname -a`" #Print the machine architecture
echo "Hostname: `hostname`" #Print the hostname
echo "User: `whoami`" #Print the user name
echo "Working directory: `pwd`" #Print the working directory
