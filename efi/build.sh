#!/bin/bash

readonly OVMF_URL="https://archlinux.org/packages/extra/any/edk2-ovmf/download"

cleanup_and_exit() {
  local exit_code=$1

  rm -f edk2-ovmf.pkg.tar.zst
  rm -f edk2-ovmf.pkg.tar
  rm -rf usr/

  exit $exit_code
}

if [[ -f "OVMF.fd" ]]; then
  exit 0
fi

wget -O edk2-ovmf.pkg.tar.zst "${OVMF_URL}" || cleanup_and_exit 1

zstd -d edk2-ovmf.pkg.tar.zst || cleanup_and_exit 1
tar -xf edk2-ovmf.pkg.tar || cleanup_and_exit 1
cp "usr/share/edk2/ia32/OVMF.fd" "OVMF.fd" || cleanup_and_exit 1

cleanup_and_exit 0
