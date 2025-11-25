<p align="center">
 <a href="#"><img src="https://img.shields.io/badge/c++-%2300427e.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white"/></a>
 <a href="#"><img src="https://img.shields.io/badge/CMake-%23eb2633.svg?style=for-the-badge&logo=cmake&logoColor=white"/></a>
</p>

# Fortunas Echo

# Build instructions

## Windows

### Prerequisites

- CMake > v4
- Internet
- Compiler that support c++23
- MSVC installed
	
### Building

- Open the repo in a windows developer powershell

#### Manual

```powershell
cmake --preset="x64-ninja-debug"
```
```powershell
cd .\build\x64-ninja-debug\
```
```powershell
cmake --build ./
```
