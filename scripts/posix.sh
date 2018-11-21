set -e

CC=cc
CFLAGS="-O3 -Isrc -I/usr/X11R6/include -I/usr/local/include"
LFLAGS="-L/usr/local/lib -L/usr/X11R6/lib -lGLEW -lGLU -lGL -lglut -lm"

HORDE_SRC="`find src/horde -name '*.c'` \
  `find src/plank -name '*.c'`"

SRC="`find src/platform -name '*.c'` \
  `find src/lre -name '*.c'` \
  `find src/gluten -name '*.c'` \
  `find src/plank -name '*.c'`"

OUTDIR=build_posix
rm -r -f $OUTDIR
mkdir $OUTDIR

$CC -o $OUTDIR/horde $CFLAGS $HORDE_SRC

$OUTDIR/horde assets

$CC -o $OUTDIR/lre $CFLAGS $SRC $LFLAGS

$OUTDIR/horde
