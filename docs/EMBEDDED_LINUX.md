# Embedded Linux / ARM64 notes

This repository includes **configuration scaffolding**, not a claim of a verified production deployment.

## ARM64 cross-compilation

A target build needs two external pieces that are intentionally not committed here:

1. an ARM64 sysroot matching the target image;
2. a Qt 6 build for ARM64, plus the Qt host tools required by Qt's CMake workflow.

Template:

```bash
export AARCH64_SYSROOT=/opt/sysroots/aarch64
export AARCH64_QT_PREFIX=/opt/qt6-aarch64

cmake -S . -B build-arm64 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/aarch64-linux-gnu.cmake \
  -DQT_HOST_PATH=/opt/qt6-host \
  -DBUILD_TESTING=OFF
cmake --build build-arm64 --parallel
```

The paths depend on the board/BSP and Qt installation. The toolchain file is kept generic so it can be adapted to a Raspberry Pi, custom ARM SoC SDK, or Yocto-generated SDK.

## Yocto / OpenEmbedded

`yocto/meta-automotive-console` is a small **recipe scaffold** for a project using the meta-qt6 layer. Before using it:

- publish/fork the repository and replace `YOUR_USERNAME`;
- pin `SRCREV` to an actual commit;
- calculate and set the MIT license MD5;
- verify the layer series against the selected Yocto release;
- add the layer and package to an image in a real BSP build.

This separation is deliberate: the portfolio repository demonstrates application engineering and packaging knowledge without pretending that an untested BSP-specific configuration has been deployed to an automotive ECU.
