Work in progress.

Adding basic LV2 plugins to use to create music with

1st plugin : vsynth, a midi controlled sound generator. The current program works, though the program does not stop writing audio when a midi press is "released" causing infintely long notes. Also, there is no volume control. Planning to make a fun FFT-using synth.
```
gcc -fvisibility=hidden -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -pthread `pkg-config --cflags lv2` -lm `pkg-config --libs lv2` vsynth.c -o vsynth.so
```
use above to compile and check for errors. Move .ttl and .so files into your lv2 dircetory and use a compatible host, like jalv or ardour. The plugin works, though it is bad. Thanks to https://github.com/sjaehn, for good and beginner friendly lv2 tutorials.
