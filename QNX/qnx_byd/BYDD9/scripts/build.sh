#!/bin/bash

create_readme()
{
    local dms_path=$1
    
    date_str=$(date)
    echo "####################################################################
#
# Supplier   : Senseauto
# Build date : ${date_str}
# Sdk version: BYDD9_D9GDv1.0.0_DMS7f7175f5_231225.1534
# App version：BYDD9_INT_B01
#
####################################################################" > ${dms_path}/README.txt

echo -e "\n\nStart script: dms_service_start.sh" >> ${dms_path}/README.txt

echo -e "\n\nDirectory tree: " >> ${dms_path}/README.txt
echo "DMS
├── bin
├── lib64
├── README.txt
└── resource" >> ${dms_path}/README.txt

echo -e "\n使用约定: 
    由于配置中使用了相对路径，需要将上述bin、lib64、resource三个目录放置在同一级，否则不能正常工作！！！" >> ${dms_path}/README.txt

cd ${dms_path}
echo -e "\n\n------------------------- files md5 info ---------------------------" >> ${dms_path}/README.txt
md5sum bin/* >> ${dms_path}/README.txt
echo -e "\n" >> ${dms_path}/README.txt
md5sum lib64/* >> ${dms_path}/README.txt
echo -e "\n" >> ${dms_path}/README.txt
md5sum resource/models/* >> ${dms_path}/README.txt
echo "--------------------------------------------------------------------" >> ${dms_path}/README.txt
cd -
echo -e "\n\nUsage: " >> ${dms_path}/README.txt
echo -e "  请使用dms_service_start.sh脚本启动程序，该脚本需要传入放置DMS相关文件的绝对路径" >> ${dms_path}/README.txt
echo -e "  例如启动脚本放置/data/local/bin/目录下，调用方式为：/data/local/bin/dms_service_start.sh /data/local\n\n" >> ${dms_path}/README.txt
}


if [[ $# < 1 ]]; then
    echo "Usage: ./build.sh [pack | all]"
    exit 0
fi

BUILD_TYPE=$1
PROJECT_DIR=$(pwd)

if [[ ${BUILD_TYPE} == "all" ]]; then
    cd ${PROJECT_DIR}/../
    ${PROJECT_DIR}/../scripts/build.sh
    cd ${PROJECT_DIR}
fi

DMS_PATH=${PROJECT_DIR}/DMS
OUTPUT_PATH=${PROJECT_DIR}/OUTPUT
#echo "OUTPUT_PATH: ${OUTPUT_PATH}"

#check DMS
if [ -f OUTPUT/dms_service_*.txt ]; then
    COMMIT_ID=$(ls OUTPUT/dms_service_* | awk -F "_" '{printf $3}')
    COMPILE_DATE=$(ls OUTPUT/dms_service_* | awk -F "_" '{printf $4}' | awk -F ".txt" '{printf $1}')
else
    echo "Please build dms service firstly"
    exit 0
fi 

PACKAGE_PATH=${COMMIT_ID}_${COMPILE_DATE}
if [ ! -d ${OUTPUT_PATH}/${PACKAGE_PATH} ]; then
    mkdir ${OUTPUT_PATH}/${PACKAGE_PATH}
fi

cp -a ${DMS_PATH} ${OUTPUT_PATH}/${PACKAGE_PATH}
cp -a ${PROJECT_DIR}/../build/senseauto_dms_service ${OUTPUT_PATH}/${PACKAGE_PATH}/DMS/bin

if [ -d ${OUTPUT_PATH}/${PACKAGE_PATH}/DMS/lib64 ]; then
    mkdir -p ${OUTPUT_PATH}/${PACKAGE_PATH}/DMS/lib64
fi
cp -a ${PROJECT_DIR}/../dms_algo/lib/* ${OUTPUT_PATH}/${PACKAGE_PATH}/DMS/lib64

if [ -d ${OUTPUT_PATH}/${PACKAGE_PATH}/DMS/resource ]; then
    mkdir -p ${OUTPUT_PATH}/${PACKAGE_PATH}/DMS/resource
fi
cp -a ${PROJECT_DIR}/../dms_algo/config ${OUTPUT_PATH}/${PACKAGE_PATH}/DMS/resource
cp -a ${PROJECT_DIR}/../dms_algo/models ${OUTPUT_PATH}/${PACKAGE_PATH}/DMS/resource

sed -i "s|models/|../resource/models/|g" ${OUTPUT_PATH}/${PACKAGE_PATH}/DMS/resource/config/module_config.json
sed -i "s|config/|../resource/config/|g" ${OUTPUT_PATH}/${PACKAGE_PATH}/DMS/resource/config/module_config.json
sed -i "s|models/|../resource/models/|g" ${OUTPUT_PATH}/${PACKAGE_PATH}/DMS/resource/config/fusion_logic_config.json
sed -i "s|config/|../resource/config/|g" ${OUTPUT_PATH}/${PACKAGE_PATH}/DMS/resource/config/fusion_logic_config.json

create_readme ${OUTPUT_PATH}/${PACKAGE_PATH}/DMS

cd ${OUTPUT_PATH}/${PACKAGE_PATH}
tar -zcf DmsService_${COMMIT_ID}_${COMPILE_DATE}.tar.gz DMS

echo "Package path: "$(pwd)/DmsService_${COMMIT_ID}_${COMPILE_DATE}.tar.gz
