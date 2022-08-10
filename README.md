Work in progress.

Adding basic LV2 plugins to use to create music

1st plugin : vsynth, a midi controlled sound generator with synthesizer-like features. (Not finished and broken atm)
```
gcc -fvisibility=hidden -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -pthread `pkg-config --cflags lv2` -lm `pkg-config --libs lv2` vsynth.c -o vsynth.so
```
use to compile and check for errors. Move ttl and so files into lv2 dircetory and use a compatible host. The plugin is detected and ports are correct. c-code causes seg fault. going to rewrite messy code
