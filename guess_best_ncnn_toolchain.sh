#!/bin/sh

if [ "${USE_NCNN_TOOLCHAIN}" = "YES" ]; then
  case "${TARGET_ARCH}" in
    aarch64)
      echo "${TARGET_ARCH}-${TARGET_OS}-gnu.toolchain.cmake" ;;
    arm)
      echo "${TARGET_ARCH}-${TARGET_OS}-${TARGET_ABI}.toolchain.cmake" ;;
    *)
      ;;
  esac
fi
