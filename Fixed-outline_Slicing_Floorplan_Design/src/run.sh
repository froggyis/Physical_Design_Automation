#!/bin/bash


# echo -n "compile via make, run, and verify (y for yes, enter for skip): "
# read ans
# if [[ ${ans} == "y" ]]; then
  echo -n "filename (e.g. n100): "
  read filename
  echo -n "dead space ratio (e.g. 0.2): "
  read dsr
  set -x
  make clean
  make
  ../bin/hw3 "../testcases/${filename}.hardblocks" "../testcases/${filename}.nets" "../testcases/${filename}.pl" "../output/${filename}.floorplan" "${dsr}"
  ../verifier/verifier "../testcases/${filename}.hardblocks" "../testcases/${filename}.nets" "../testcases/${filename}.pl" "../output/${filename}.floorplan" "${dsr}"
  # set +x
# fi
