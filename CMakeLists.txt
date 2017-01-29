CMAKE_MINIMUM_REQUIRED( VERSION 2.8 FATAL_ERROR )

PROJECT( nilsimsa-lite )

ADD_LIBRARY(
 nilsimsa-lite
 nilsimsa.h
 nilsimsa.c )

 ENABLE_TESTING( ON )
ADD_SUBDIRECTORY( tests )
