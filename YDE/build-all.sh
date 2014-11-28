#!/usr/bin/bash
# (C) 2014 FrankHB.
# Script for build all YDE packages using SHBuild-BuildApp.

set -e
# NOTE: SHBuild-BuildApp.sh should be in $PATH.
source SHBuild-BuildApp.sh

: ${SHBuild_ThisDir:="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"}

SHBuild_PkgPathList="`ls -d ${SHBuild_ThisDir}/*/`"

for SHBuild_PkgPath in $SHBuild_PkgPathList
do
	SHBuild_Pkg=`basename "$SHBuild_PkgPath"`
	echo Building package \"${SHBuild_Pkg}\" in directory \"\
"${SHBuild_PkgPath}"\" ...
	SHBuild_BuildApp ${SHBuild_PkgPath} "$@"
	echo Built \"${SHBuild_Pkg}\".
done

echo Done.

