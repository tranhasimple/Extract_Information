# Extract_Information

1. Install library
- In Visual Studio, open Project -> EKYC properties -> C/C++ -> General -> Additional Include Directories 
    add: opencv/build/include
         vcpkg\installed\x64-windows\include\tesseract
         vcpkg\installed\x64-windows\include
         vcpkg\installed\x64-windows\include\leptonica
  - In linker -> General -> Additional Library Directories
    add: vcpkg\installed\x64-windows\lib
         opencv\build\x64\vc15\lib
