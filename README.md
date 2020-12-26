## For iOS

```
cmake -G Xcode -B build \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0
```
## For MacOS

```shell
cmake -B build -G Xcode
## For Windows 64
```
## For Windows64

```shell
cmake -B build -G "Visual Studio 15 2017 Win64"
cp ../third_party/ffmpeg/lib/windows/*.dll .
cp ../third_party/sdl2/lib/windows/*.dll .
```

注意：windows下可能需要处理一些问题才能编译（因为我平时都是使用xcode开发），以下为可能的问题：

1. 文件保存格式以及CRLF等问题

2. MSVC不支持的语法

   ```c++
   // render.cpp
   std::shared_ptr<MediaFrame> nextFrame = ({
   std::unique_lock<std::mutex> lock(frame_buffer_.mutex_);
   frame_buffer_.UnsafeFront();
   });
   // 修改为
   std::shared_ptr<MediaFrame> nextFrame;
   {
   std::unique_lock<std::mutex> lock(frame_buffer_.mutex_);
   newxtFrame = frame_buffer_.UnsafeFront();
   }
   ```

   

   

