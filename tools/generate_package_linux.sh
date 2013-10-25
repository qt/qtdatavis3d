#!/bin/sh

DATAVIS_TEMP_DIR=temp_dir
DATAVIS_TEMP_DIR_FULL=../$DATAVIS_TEMP_DIR

if [ "$1" = "" ]
then
    echo Usage: $0 version [branch or SHA]
    echo Branch defaults to master.
    echo Creates the package in parent dir.
    echo A temporary dir $DATAVIS_TEMP_DIR_FULL is utilized for intermediate steps.
fi

if [ "$2" = "" ]
then
    DATAVIS_BRANCH=origin/master
else
    DATAVIS_BRANCH=$2
fi

DATAVIS_VERSION=$1
DATAVIS_CURRENT_DIR=$PWD
DATAVIS_BUILD_DIR=$DATAVIS_TEMP_DIR_FULL/tempbuild
DATAVIS_PACKAGE_UNTAR_NAME=qtdatavisualization_$DATAVIS_VERSION
DATAVIS_PACKAGE_UNTAR_DIR=$DATAVIS_TEMP_DIR_FULL/$DATAVIS_PACKAGE_UNTAR_NAME
DATAVIS_TEMP_TAR=qtdatavisualization_temp_$DATAVIS_VERSION.tar
DATAVIS_TEMP_TAR_FULL=$DATAVIS_TEMP_DIR_FULL/$DATAVIS_TEMP_TAR
DATAVIS_FINAL_TAR=$DATAVIS_CURRENT_DIR/../qtdatavisualization_$DATAVIS_VERSION.tar

echo Exporting $DATAVIS_BRANCH to $DATAVIS_TEMP_TAR_FULL...
rm -r -f $DATAVIS_TEMP_DIR_FULL 2> /dev/null
mkdir -p $DATAVIS_TEMP_DIR_FULL 2> /dev/null
git fetch
git archive --format tar --output $DATAVIS_TEMP_TAR_FULL $DATAVIS_BRANCH

echo Unpacking $DATAVIS_TEMP_TAR_FULL to $DATAVIS_PACKAGE_UNTAR_DIR and $DATAVIS_BUILD_DIR...
mkdir -p $DATAVIS_PACKAGE_UNTAR_DIR 2> /dev/null
mkdir -p $DATAVIS_BUILD_DIR 2> /dev/null
tar -xvf $DATAVIS_TEMP_TAR_FULL -C $DATAVIS_PACKAGE_UNTAR_DIR > /dev/null
tar -xvf $DATAVIS_TEMP_TAR_FULL -C $DATAVIS_BUILD_DIR > /dev/null
#Workaround for git archive bug
rm -r -f $DATAVIS_PACKAGE_UNTAR_DIR/tools
rm -r -f $DATAVIS_PACKAGE_UNTAR_DIR/tests
rm -r -f $DATAVIS_BUILD_DIR/tools
rm -r -f $DATAVIS_BUILD_DIR/tests

echo Generating includes, mkspecs, and docs in $DATAVIS_BUILD_DIR...
cd $DATAVIS_BUILD_DIR
mkdir -p .git 2> /dev/null
qmake > /dev/null 2> /dev/null
make docs > /dev/null 2> /dev/null
cd $DATAVIS_CURRENT_DIR

echo Copying generated files to $DATAVIS_PACKAGE_UNTAR_DIR
cp -r $DATAVIS_BUILD_DIR/doc $DATAVIS_PACKAGE_UNTAR_DIR/doc
cp -r $DATAVIS_BUILD_DIR/include $DATAVIS_PACKAGE_UNTAR_DIR/include

echo Repackaging $DATAVIS_PACKAGE_UNTAR_DIR to $DATAVIS_FINAL_TAR
rm $DATAVIS_FINAL_TAR 2> /dev/null
cd $DATAVIS_TEMP_DIR_FULL
tar -cvf $DATAVIS_FINAL_TAR $DATAVIS_PACKAGE_UNTAR_NAME >/dev/null
gzip $DATAVIS_FINAL_TAR >/dev/null
cd $DATAVIS_CURRENT_DIR

exit 0
