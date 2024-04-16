# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

option(ENERGY_MODE "Enable energy mode" 0)
# cmake args

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -mext-dsp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mext-dsp")

project(onnc_project)

set(ANDES_PATH $ENV{ANDES_PATH})
set(andes_includes 
  ${ANDES_PATH}/include
)
set(TINKERV_LINK_OPTS "-L${ANDES_PATH}/lib -lnn -Wl,-T,${CMAKE_SOURCE_DIR}/tinkerV_ldscript")
if("$ENV{STATIC_LINK}" STREQUAL "True")
  set(TINKERV_LINK_OPTS "${TINKERV_LINK_OPTS} -static")
endif()


# define a library for the model sources.
add_library(onnc_model)
file(GLOB_RECURSE onnc_model_srcs ad01_code/*.cpp)
target_sources(onnc_model PRIVATE ${onnc_model_srcs})
target_include_directories(onnc_model PRIVATE
  ${CMAKE_SOURCE_DIR} 
  ad01_code
  ${andes_includes}
)
target_compile_options(onnc_model PRIVATE -Wno-unused-variable)  # onnc-generated code tends to include lots of these.

add_library(tinymlperf_internal)
file(GLOB_RECURSE tiny_internal_srcs src/api/*.cpp)
target_sources(tinymlperf_internal PRIVATE ${tiny_internal_srcs})
target_compile_options(tinymlperf_internal PRIVATE -Wno-unused-variable)  # onnc-generated code tends to include lots of these.

add_executable(app)
target_link_libraries(app PRIVATE onnc_model tinymlperf_internal ${TINKERV_LINK_OPTS})
file(GLOB_RECURSE app_srcs src/**.cpp)
target_sources(app PRIVATE
  ${app_srcs}
)
target_include_directories(app PRIVATE
  ${CMAKE_SOURCE_DIR} 
  ad01_code
  ${andes_includes}
)
target_compile_definitions(tinymlperf_internal PUBLIC -DEE_CFG_ENERGY_MODE=${ENERGY_MODE})