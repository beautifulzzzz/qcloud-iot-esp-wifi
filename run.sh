#!/bin/bash

#I don't like to set environment variables in the system, 
#so I put the environment variables in run.sh.
#Every time I use run.sh, the enviroment variables will be set, after use that will be unsetted.
PROJECT_ROOT=../..
TOOLS_PATH=$PROJECT_ROOT/tool
SDK_PATH=$PROJECT_ROOT/sdk
APP_PATH=$PROJECT_ROOT/app

XTENSA_LX106_ELF_PATH=$TOOLS_PATH/xtensa-lx106-elf
ESP_IDF_PATH=$SDK_PATH/ESP8266_RTOS_SDK

the_sdk_path=`cd $ESP_IDF_PATH; pwd`
the_tool_chain_path=`cd $XTENSA_LX106_ELF_PATH/bin; pwd`

export PATH="$PATH:$the_tool_chain_path"
export IDF_PATH="$the_sdk_path"

#--------------------------------------------------------------------------
function install_project_from_github(){
    echo "> install project form github ..."
    
    cd $ESP_IDF_PATH
    git checkout release/v3.1
}

function app_op(){
    echo ">> APP_OP   "$1

    cd ./qcloud-iot-esp8266-demo/
    if [ "$1" == "config" ]; then
        make menuconfig
    elif [ "$1" == "make" ]; then
        make
    elif [ "$1" == "erase" ]; then
        make erase
    elif [ "$1" == "flash" ]; then
        make flash
    elif [ "$1" == "monitor" ]; then
        make monitor
    elif [ "$1" == "clean" ]; then
        make clean
    else
        echo "error, try bash run.sh help"
    fi
    cd -
}

if [ "$1" == "create" ]; then
    install_project_from_github
elif [ "$1" == "app" ];then
    app_op $2
elif [ "$1" == "help" ];then
    echo "|----------------------------------------------------"
    echo "| ./run.sh op param"
    echo "| op:"
    echo "|   create : checkout the ESP8266_RTOS_SDK to V3.1"
    echo "|   app : param = config/make/erase/flash/monitor/clean"
    echo "| examples:"
    echo "|   second create app : config -> make -> flash -> monitor"
    echo "|----------------------------------------------------"
else
    echo "error, try bash run.sh help"   
fi

