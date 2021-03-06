#!/bin/sh

# NOTE: This script assumes that:
# - the sonarqube server is running on the default port
# - the C/C++ module has been installed in SonciQube
# - the sonar-scanner is installed and in $PATH

# Check for configuration file
if [ ! -f sonar-project.properties ]
then
  echo "ERROR: Could not find sonarqube properties file!"
  exit
fi

# Get and unzip build wrapper
wget http://localhost:9000/static/cpp/build-wrapper-linux-x86.zip 
unzip build-wrapper-linux-x86.zip

# Build with build wrapper
(cd ..; build/build-wrapper-linux-x86/build-wrapper-linux-x86-64 --out-dir build/sonar_build_wrapper_output make clean all)

# Run analysis
# NOTE: This assumes that:
# - the sonarqube server is running on the default port
# - the sonar-scanner is installed and in $PATH
(cd ..; sonar-scanner -Dproject.settings=build/sonar-project.properties -Dsonar.cfamily.build-wrapper-output=build/sonar_build_wrapper_output)

# Remove build wrapper and scanner files
rm -rf sonar_build_wrapper_output
rm -rf ../.scannerwork

# Remove build wrapper
rm -rf build-wrapper-linux-x86
rm -f build-wrapper-linux-x86.zip
