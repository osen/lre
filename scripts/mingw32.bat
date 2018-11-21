set OUTDIR=build_mingw32
set CC=gcc
set CFLAGS=-DGLEW_STATIC -DFREEGLUT_STATIC -O3 -Isrc -I%OUTDIR%/mingw32/include
set LFLAGS=%OUTDIR%/mingw32/lib/libglew32.a %OUTDIR%/mingw32/lib/libfreeglut_static.a -lopengl32 -lgdi32 -lwinmm

set HORDE_SRC=src/horde/*.c src/plank/*.c
set SRC=src/platform/*.c src/platform/contrib/*.c src/lre/*.c src/gluten/*.c src/plank/*.c

rmdir /s /q %OUTDIR%
mkdir %OUTDIR%
contrib\windows\unzip contrib/windows/mingw32.zip -d build_mingw32

echo GLUT_ICON ICON raw/icon.ico > %OUTDIR%\resources.rc
windres %OUTDIR%/resources.rc -O coff -o %OUTDIR%/resources.res

%CC% -o %OUTDIR%/horde %CFLAGS% %HORDE_SRC%

%OUTDIR%\horde assets

%CC% -o %OUTDIR%/lre %CFLAGS% %SRC% %OUTDIR%/resources.res %LFLAGS%

%OUTDIR%\horde
