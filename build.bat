gcc -o raylib_snake_test.exe raylib_snake_test.cpp {PATH_TO_RAYLIB}/src/raylib.rc.data -s -static -Os -std=c99 -Wall -I"{PATH_TO_RAYLIB}/src" -L"{PATH_TO_RAYLIB}/src" -Iexternal -DPLATFORM_DESKTOP -lraylib -lopengl32 -lgdi32 -lwinmm