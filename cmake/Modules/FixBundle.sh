#!/bin/bash

if [ $# -lt 2 ] ; then
    echo "Usage: $0 <vlmc_binary_path> <libvlc_dir>"
    exit 1
fi

ZMVIEWER_BIN_DIR=$1
ZMVIEWER_BIN="$ZMVIEWER_BIN_DIR/zmviewer"
LIBVLC_DIR=$2

install_name_tool -change @loader_path/lib/libvlc.5.dylib @executable_path/libvlc.5.dylib $ZMVIEWER_BIN
install_name_tool -change @loader_path/lib/libvlccore.7.dylib @executable_path/libvlccore.7.dylib $ZMVIEWER_BIN

cp -a $LIBVLC_DIR/lib/./ $ZMVIEWER_BIN_DIR
cp -r $LIBVLC_DIR/plugins $ZMVIEWER_BIN_DIR/../