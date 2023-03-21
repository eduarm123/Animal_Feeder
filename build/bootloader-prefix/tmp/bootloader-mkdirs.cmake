# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/eduar/esp/esp-idf/components/bootloader/subproject"
  "C:/Repos/Animal_Feeder/build/bootloader"
  "C:/Repos/Animal_Feeder/build/bootloader-prefix"
  "C:/Repos/Animal_Feeder/build/bootloader-prefix/tmp"
  "C:/Repos/Animal_Feeder/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Repos/Animal_Feeder/build/bootloader-prefix/src"
  "C:/Repos/Animal_Feeder/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Repos/Animal_Feeder/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Repos/Animal_Feeder/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
