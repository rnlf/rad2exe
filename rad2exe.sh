#!/bin/bash -e

# Sanity check
if [ -z "$WATCOM" ]; then
  echo '$WATCOM not set.'
  exit 1
fi

if [ -z "$(which wcl)" ]; then
  echo "Watcom's wcl not found in \$PATH"
  exit 1
fi


if [[ $# < 2 ]]; then
  echo "usage: $0 <rad-file> <out-file>" >&2
  exit 1
fi

SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
TMPDIR="$(mktemp -d)"

"$SCRIPTDIR/tools/prepare" "$1" "$TMPDIR/songdata.h"
cd "$TMPDIR"
wcl -bcl=dos -lr -ox -i="$SCRIPTDIR/ext" -i="$TMPDIR" -fe="$TMPDIR/play.exe" -mh "$SCRIPTDIR/player/main.cpp"
cd -
"$SCRIPTDIR/ext/upx" --ultra-brute --8086 "$TMPDIR/play.exe"
cp "$TMPDIR/play.exe" "$2"
