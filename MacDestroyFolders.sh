#!/bin/sh -x

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
rm -rf $SCRIPT_DIR/Binaries
rm -rf $SCRIPT_DIR/Intermediate

for dir in $SCRIPT_DIR/Plugins/*/; do
  # Construct the full path to the sub-folder
  SUB_FOLDER_PATH=$dir/Binaries

  if [ -d "$SUB_FOLDER_PATH" ]; then
    rm -rf "$SUB_FOLDER_PATH"
  fi
  
   SUB_FOLDER_PATH=$dir/Intermediate
  
  if [ -d "$SUB_FOLDER_PATH" ]; then
    rm -rf "$SUB_FOLDER_PATH"
  fi
done