#!/bin/sh -x

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
rm -rf $SCRIPT_DIR/Saved/SampleProject
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

mkdir -p $SCRIPT_DIR/Saved/SampleProject

rsync -a $SCRIPT_DIR/SampleProjectBase/ $SCRIPT_DIR/Saved/SampleProject
mkdir -p $SCRIPT_DIR/Saved/SampleProject/Plugins
rsync -a $SCRIPT_DIR/Plugins/EmergenceSamples/ $SCRIPT_DIR/Saved/SampleProject/Plugins/EmergenceSamples
rsync -a $SCRIPT_DIR/Plugins/VRM4U/ $SCRIPT_DIR/Saved/SampleProject/Plugins/VRM4U
rsync -a $SCRIPT_DIR/Plugins/Emergence/ $SCRIPT_DIR/Saved/SampleProject/Plugins/Emergence
rsync -a $SCRIPT_DIR/Plugins/EmergenceVRMSupport/ $SCRIPT_DIR/Saved/SampleProject/Plugins/EmergenceVRMSupport

rm -f $SCRIPT_DIR/Saved/UnrealSampleProject.zip

tar -C $SCRIPT_DIR/Saved/SampleProject -cf UnrealSampleProject.zip "Config" "Plugins" "Source" "EmergenceSample.uproject"

read -n1 -r -p "Press any key to continue..."