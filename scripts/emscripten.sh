set -e

export PATH="$PATH:$HOME/.opt/emscripten"
export PATH="$HOME/.opt/emscripten/fastcomp/bin:$PATH"

CC=cc
EMCC=emcc
CFLAGS="-O3 -Isrc"

HORDE_SRC="`find src/horde -name '*.c'` \
  `find src/plank -name '*.c'`"

SRC="`find src/platform -name '*.c'` \
  `find src/lre -name '*.c'` \
  `find src/gluten -name '*.c'` \
  `find src/plank -name '*.c'`"

OUTDIR=build_emscripten
rm -r -f $OUTDIR
mkdir $OUTDIR

$CC -o $OUTDIR/horde $CFLAGS $HORDE_SRC

$OUTDIR/horde assets

# 160 MB
TOTAL_MEM=`expr 16777216 \* 10`

$EMCC -s TOTAL_MEMORY=$TOTAL_MEM -o $OUTDIR/lre.html $CFLAGS $SRC
#$EMCC -s ALLOW_MEMORY_GROWTH=1 -o $OUTDIR/lre.html $CFLAGS $SRC

$OUTDIR/horde
