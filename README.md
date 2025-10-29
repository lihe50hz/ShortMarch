# ShortMarch

add cmake tool chain to vcpkd.cmake

## How to build

We recommend using [Visual Studio](https://visualstudio.microsoft.com/) as the IDE for building this project.

### Step 0: Prerequisites

- [vcpkg](https://github.com/microsoft/vcpkg): The C++ package manager. Clone the vcpkg repo to anywhere you like, we will refer tha vcpkg path as
  `<VCPKG_ROOT>` in the following instructions (the path ends in `vcpkg`, not its parent directory).
- [MSVC with Windows SDK (version 10+)](https://visualstudio.microsoft.com/downloads/): We usually install this via Visual Studio installer. You should select the following workloads during installation:
  - Desktop development with C++

  Then everything should be installed automatically.
- [[optional] Python3](https://python.org): We provide python package with pybind11. Such functionality requires Python3 installation. You may install anywhere you like (System-wide, User-only, Conda, Homebrew, etc.). We will refer the python executable path as `<PYTHON_EXECUTABLE_PATH>` in the following instructions.
- [[optional] Vulkan SDK](https://vulkan.lunarg.com/sdk/home): Vulkan is the latest cross-platform graphics API. Since D3D12 is available on Windows, this is optional. Install the SDK [Caution: not the Runtime (RT)] via the official **SDK installer**. You should be able to run `vulkaninfo` command in a new terminal after installation. **No optional components are needed for this project**.
- [[optional] CUDA Toolkit](https://developer.nvidia.com/cuda-downloads): CUDA is optional, however, some functions such as most of the GPU-accelerated physics simulation features will require CUDA. Install the toolkit with the official **exe (local)** installer. You should be able to run `nvcc --version` command in a new terminal after installation.

- ### Step 1: Clone the repo

- Clone this repo with submodules:
  ```bash
  git clone --recurse-submodules
  ```
  or
- Clone without submodules:
  ```bash
  git clone <this-repo-url>
  ```
  Then initialize and update the submodules (in the root directory of this repo):
  ```bash
  git submodule update --init --recursive
  ```

### Step 2: CMake Configuration

In Visual Studio, open the `Project` -> `CMake Settings for Project` menu, and modify the `CMake toolchain file` to: `<VCPKG_ROOT>/scripts/buildsystems/vcpkg.cmake`.

In this process, the CMake script will check whether you have installed Vulkan SDK and CUDA Toolkit, and configure the build options accordingly.

### Step 3: Build and Run

Now you can build and run the project in Visual Studio as usual, selecting the desired target (`ShortMarchDemo.exe` for the demo we provided).

## Bug Shooting

### CMake Configuration Issues

Make sure that you have set the `CMake toolchain file` correctly to `<VCPKG_ROOT>/scripts/buildsystems/vcpkg.cmake`. After any change to the configuration, remember to clean the CMake cache (via `Project` -> `CMake Cache` -> `Delete Cache and Reconfigure` menu in Visual Studio) and reconfigure the project.

### Vulkan Validation Layer Error

If you encounter the following error when running the application:
```
validation layer (ERROR): loader_get_json: Failed to open JSON file </path/to/a/json>
```
where `/path/to/a/json` is a non-existent file, it indicates that the Vulkan validation layers are trying to load a configuration file that does not exist on your system. Hopefully, the </path/to/a/json> is related to your Steam or Epic Games installation. To resolve this issue, you can try the following steps:
1. Press `Win + R` and type `regedit` to open the Registry Editor.
2. Try to find the `</path/to/a/json>` under:
	- `HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\Vulkan\ImplicitLayers`
	- `HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\Vulkan\ExplicitLayers`
	- `HKEY_CURRENT_USER\SOFTWARE\Khronos\Vulkan\ImplicitLayers`
	- `HKEY_CURRENT_USER\SOFTWARE\Khronos\Vulkan\ExplicitLayers`.
3. Delete the entry that points to the non-existent JSON file and restart your program.