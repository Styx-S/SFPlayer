## For Windows 64

```shell
cmake -B build -G "Visual Studio 15 2017 Win64"
cp ../third_party/ffmpeg/lib/windows/*.dll .
cp ../third_party/sdl2/lib/windows/*.dll .
```

## For MacOS

```shell
cmake -B build -G Xcode
```

## For iOS

```
cmake -G Xcode -B build \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0
```

