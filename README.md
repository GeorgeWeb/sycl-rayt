# sycl-rayt-test

## Building the project (Linux/Ubuntu)

```
mkdir build && cd build
```
```
cmake .. -GNinja -DComputeCpp_DIR=/path/to/computecpp && ninja
```

### Possible warning on compilation about the memcpy/memse intrinsics added by the llvm optimizer:

```
Building ComputeCpp integration header file /.../intel_sycl-rayt/build/sycl-rayt_sycl-rayt.cpp.sycl
remark: [Computecpp:CC0027]: Some memcpy/memset intrinsics added by the llvm optimizer were replaced by serial functions. This is a workaround for OpenCL drivers that do not support those intrinsics. This may impact performance, consider using -no-serial-memop. [-Rsycl-serial-memop]
```

### ComputeCpp **flag** to avoid the above warning: `-no-serial-memop`.

Use with the cmake script as follows:

```
cmake .. -GNinja -DComputeCpp_DIR=/path/to/computecpp -DCOMPUTECPP_USER_FLAGS=-no-serial-memop
```

The cmake script will automatically target spir64, considering the platform supports SPIR.

If you wish to specify the **SYCL Target** manually, the `sycl-target` **flag** can be used.

Use with the cmake script as follows:

```
cmake .. -GNinja -DComputeCpp_DIR=/path/to/computecpp -DCOMPUTECPP_BITCODE=spir64
```

More information about the compiler options can be found [here](https://developer.codeplay.com/products/computecpp/ce/guides/compute-compiler).

---

## Changing the selected SYCL/OpenCL device

Open `src/sycl-rayt.cpp`.

Look for this line of code:

```cpp
auto queue = sycl::queue(sycl::gpu_selector{});
```

And change `gpu_selector` to `cpu_selector` (i.e).

---

To write the raytraced image data to an image file (currently `.ppm`), use:

```
./sycl-rayt | tee output.ppm
```

The output image (using the working `gpu_selector`) should look like this.

![Output Image](https://raw.githubusercontent.com/GeorgeWeb/sycl-rayt-test/master/misc/output.jpg)
