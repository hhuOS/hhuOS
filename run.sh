#!/bin/bash

# Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
# Institute of Computer Science, Department Operating Systems
# Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
#
#
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

readonly CONST_QEMU_BIN_I386="qemu-system-i386"
readonly CONST_QEMU_BIN_X86_64="qemu-system-x86_64"
readonly CONST_QEMU_MACHINE_PC="pc"
readonly CONST_QEMU_MACHINE_PC_KVM="pc,accel=kvm,kernel-irqchip=split"
readonly CONST_QEMU_CPU_I386="base,+fpu,+tsc,+cmov,+fxsr,+mmx,+sse,+apic"
readonly CONST_QEMU_CPU_X86_64="qemu64"
readonly CONST_QEMU_DEFAULT_RAM="256M"
readonly CONST_QEMU_BIOS_PC=""
readonly CONST_QEMU_BIOS_IA32_EFI="bios/ovmf/ia32/OVMF.fd"
readonly CONST_QEMU_BIOS_X64_EFI="bios/ovmf/x64/OVMF.fd"
readonly CONST_QEMU_STORAGE_ARGS="-drive driver=raw,index=0,if=floppy,file=floppy0.img -drive driver=raw,node-name=hdd0,file.driver=file,file.filename=hdd0.img"
readonly CONST_QEMU_NETWORK_ARGS="-nic model=rtl8139,id=eth0,hostfwd=udp::1797-:1797 -object filter-dump,id=filter0,netdev=eth0,file=eth0.dump"
readonly CONST_QEMU_ARGS="-boot d -vga std -rtc base=localtime -device isa-debug-exit -smp 2"
readonly CONST_QEMU_OLD_AUDIO_ARGS="-soundhw pcspk -device sb16,irq=10,dma=1"
readonly CONST_QEMU_NEW_AUDIO_ARGS="-audiodev id=pa,driver=pa -machine pcspk-audiodev=pa -device sb16,irq=10,dma=1,audiodev=pa"

QEMU_BIN="${CONST_QEMU_BIN_I386}"
QEMU_MACHINE="${CONST_QEMU_MACHINE_PC}"
QEMU_BIOS="${CONST_QEMU_BIOS_IA32_EFI}"
QEMU_RAM="${CONST_QEMU_DEFAULT_RAM}"
QEMU_CPU="${CONST_QEMU_CPU_I386}"
QEMU_CPU_OVERWRITE="false"
QEMU_STORAGE_ARGS="${CONST_QEMU_STORAGE_ARGS}"
QEMU_AUDIO_ARGS="${CONST_QEMU_NEW_AUDIO_ARGS}"
QEMU_NETWORK_ARGS="${CONST_QEMU_NETWORK_ARGS}"
QEMU_ARGS="${CONST_QEMU_ARGS}"
QEMU_USB_ARGS="-usb -device usb-mouse,bus=usb-bus.0,port=1 -device usb-kbd,bus=usb-bus.0,port=2"
QEMU_USB_ARGS_ROOT=""

QEMU_GDB_PORT=""

if [ -f "hhuOS-towboot.img" ]; then
  QEMU_BOOT_DEVICE="-drive driver=raw,node-name=boot,file.driver=file,file.filename=hhuOS-towboot.img"
elif [ -f "hhuOS-limine.iso" ]; then
  QEMU_BOOT_DEVICE="-boot d -cdrom hhuOS-limine.iso"
elif [ -f "hhuOS-grub.iso" ]; then
  QEMU_BOOT_DEVICE="-boot d -cdrom hhuOS-grub.iso"
fi

version_lt() {
  test "$(printf "%s\n" "$@" | sort -V | tr ' ' '\n' | head -n 1)" != "${2}"
}

set_audio_parameters() {
  qemu_version=$(${QEMU_BIN} --version | head -n 1 | cut -c 23-)

  if version_lt "$qemu_version" "5.0.0"; then
    QEMU_AUDIO_ARGS="${CONST_QEMU_OLD_AUDIO_ARGS}"
  fi
}

get_ovmf() {
  cd "bios/ovmf" || exit 1
  ./build.sh || exit 1
  cd "../.." || exit 1
}

check_file() {
  local file=$1

  if [ ! -f "$file" ]; then
    printf "File '%s' does not exist!\\n" "${file}"
    exit 1
  fi
}

parse_file() {
  local path=$1
  
  if [[ $path == *.iso ]]; then
    QEMU_BOOT_DEVICE="-boot d -cdrom ${path}"
  elif [[ $path == *.img ]]; then
    QEMU_BOOT_DEVICE="-drive driver=raw,node-name=boot,file.driver=file,file.filename=${path}"
  else
    printf "Invalid file '%s'!\\n" "${path}"
    exit 1
  fi
  
  check_file $path
}

parse_architecture() {
  local architecture=$1

  if [ "${architecture}" == "i386" ] || [ "${architecture}" == "x86" ] || [ "${architecture}" == "ia32" ]; then
    QEMU_BIN="${CONST_QEMU_BIN_I386}"

    if [ "${QEMU_CPU_OVERWRITE}" != "true" ]; then
      QEMU_CPU="${CONST_QEMU_CPU_I386}"
    fi

    if [ "${QEMU_BIOS}" != "${CONST_QEMU_BIOS_PC}" ]; then
      QEMU_BIOS="${CONST_QEMU_BIOS_IA32_EFI}"
    fi
  elif [ "${architecture}" == "x86_64" ] || [ "${architecture}" == "x64" ]; then
    QEMU_BIN="${CONST_QEMU_BIN_X86_64}"

    if [ "${QEMU_CPU_OVERWRITE}" != "true" ]; then
      QEMU_CPU="${CONST_QEMU_CPU_X86_64}"
    fi

    if [ "${QEMU_BIOS}" != "${CONST_QEMU_BIOS_PC}" ]; then
      QEMU_BIOS="${CONST_QEMU_BIOS_X64_EFI}"
    fi
  else
    printf "Invalid architecture '%s'!\\n" "${architecture}"
    exit 1
  fi
}

parse_machine() {
  local machine=$1

  if [ "${machine}" == "pc" ]; then
    QEMU_MACHINE="${CONST_QEMU_MACHINE_PC}"
  elif [ "${machine}" == "pc-kvm" ]; then
    QEMU_MACHINE="${CONST_QEMU_MACHINE_PC_KVM}"
  else
    printf "Invalid machine '%s'!\\n" "${machine}"
    exit 1
  fi
}

parse_bios() {
  local bios=$1

  if [ "${bios}" == "true" ]; then
    QEMU_BIOS="${CONST_QEMU_BIOS_PC}"
  else
    printf "Invalid value for parameter 'bios' ('%s')!\\n" "${bios}"
    exit 1
  fi
}

parse_ram() {
  local memory=$1

  QEMU_RAM="${memory}"
}

parse_cpu() {
  local cpu=$1

  QEMU_CPU="${cpu}"
  QEMU_CPU_OVERWRITE="true"
}

parse_debug() {
  local port=$1

  echo "set architecture i386
      set disassembly-flavor intel
      target remote 127.0.0.1:${port}" >/tmp/gdbcommands."$(id -u)"

  QEMU_GDB_PORT="${port}"
}

start_gdb() {
  gdb -x "/tmp/gdbcommands.$(id -u)" "loader/boot/hhuOS.bin"
  exit $?
}

print_usage() {
  printf "Usage: ./run.sh [OPTION...]
    Available options:
    -f, --file
        Set the .iso or .img file, which qemu should boot (Default: hhuOS.img)
    -a, --architecture
        Set the architecture, which qemu should emulate ([i386,x86,ia32] | [x86_64,x64]) (Default: i386)
    -m, --machine
        Set the machine profile, which qemu should emulate ([pc] | [pc-kvm]) (Defualt: pc)
    -b, --bios
        Set to true, to use the classic BIOS instead of UEFI
    -r, --ram
        Set the amount of ram, which qemu should use (e.g. 256, 1G, ...) (Default: 128M)
    -c, --cpu
        Set the CPU model, which qemu should emulate (e.g. 486, pentium, pentium2, ...) (Default: base)
    -d, --debug
        Set the port, on which qemu should listen for GDB clients (default: disabled)
    -h, --help
        Show this help message\\n"
}

parse_usb() {
  local _type_=""
  local _vendor_=""
  local _product_=""
  local _host_port_=""
  local _host_addr_=""
  local _host_bus_=""
  local _port_=""

  local UHCI_TYPE="uhci"
  local OHCI_TYPE="ohci"
  local XHCI_TYPE="xhci"
  local EHCI_TYPE="ehci"
  local TYPE_KEY="type"
  local VENDOR_KEY="vendorid"
  local PRODUCT_KEY="productid"
  local HOST_BUS_KEY="hostbus"
  local HOST_ADDRESS_KEY="hostaddr"
  local HOST_PORT_KEY="hostport"
  local PORT_KEY="port"

  local device_msg="-device usb-host"
  local bus_msg="bus="
  local uhci_bus="usb-bus.0"
  local ehci_bus="ehci.0"

  #reset default
  QEMU_USB_ARGS=""

  OLD_IFS=${IFS}
  IFS=":"
  for y in $1; do
    IFS=";"
    read -ra controller_entry <<< ${y}
    for z in "${controller_entry[@]}"; do
      IFS=","
      read -ra device_entry <<< ${z}
      for x in "${device_entry[@]}"; do
        type=$(expr ${x} : "${TYPE_KEY}=\([a-z]\{4\}\)")
        if [[ -n "${type}" && -z "${_type_}" ]]; then _type_=${type};
          case ${type} in
            ${UHCI_TYPE})
              _type_="-usb -device usb-hub,bus=usb-bus.0,port=1 -device usb-hub,bus=usb-bus.0,port=2 ";;
            ${EHCI_TYPE})
              _type_="-device usb-ehci,id=ehci ";;
            ${XHCI_TYPE})
            _type_="-device qemu-xhci ";;
            *)
            printf "Unknown controller type '%s'\n" ${type};exit 1;;
          esac
          QEMU_USB_ARGS="${QEMU_USB_ARGS}${_type_}"
        fi

        vendor=$(expr ${x} : "${VENDOR_KEY}=\(0x[0-9a-fA-F]\{1,4\}\)")

        if [[ -n "${vendor}" && -z "${_vendor_}" ]]; then _vendor_=${vendor}; fi

        product=$(expr ${x} : "${PRODUCT_KEY}=\(0x[0-9a-fA-F]\{1,4\}\)")

        if [[ -n "${product}" && -z "${_product_}" ]]; then _product_=${product}; fi

        host_bus=$(expr ${x} : "${HOST_BUS_KEY}=\([0-9]\{1,4\}\)")

        if [[ -n "${host_bus}" && -z "${_host_bus_}" ]]; then _host_bus_=${host_bus}; fi

        host_address=$(expr ${x} : "${HOST_ADDRESS_KEY}=\([0-9]\{1,4\}\)")

        if [[ -n "${host_address}" && -z "${_host_addr_}" ]]; then _host_addr_=${host_address}; fi

        host_port=$(expr ${x} : "${HOST_PORT_KEY}=\([a-zA-Z0-9]\{1,4\}\)")

        if [[ -n ${host_port} && -z "${_host_port_}" ]]; then _host_port_=${host_port}; fi

        port=$(expr ${x} : "${PORT_KEY}=\([0-9]\{1\}[.]\{0,1\}[0-9]\{0,1\}\)")

        if [[ -n "${port}" && -z "${_port_}" ]]; then _port_=${port}; fi
      done
      if [ -z "${_type_}" ]; then printf "no controller type inserted ... exiting.\n";exit 1; fi
      QEMU_USB_ARGS="${QEMU_USB_ARGS}${device_msg}"
      t=$(grep -e "usb" <<< "${_type_}"); tu=$(grep -e "usb-ehci" <<< "${_type_}")

      if [ -n "${t}" ]; then QEMU_USB_ARGS="${QEMU_USB_ARGS},${bus_msg}${uhci_bus}"; fi
      if [ -n "${tu}" ]; then QEMU_USB_ARGS="${QEMU_USB_ARGS},${bus_msg}${ehci_bus}"; fi
      if [ -n "${_port_}" ]; then QEMU_USB_ARGS="${QEMU_USB_ARGS},${PORT_KEY}=${_port_}"; fi
      if [ -n "${_vendor_}" ]; then QEMU_USB_ARGS="${QEMU_USB_ARGS},${VENDOR_KEY}=${_vendor_}"; fi
      if [ -n "${_product_}" ]; then QEMU_USB_ARGS="${QEMU_USB_ARGS},${PRODUCT_KEY}=${_product_}"; fi
      if [ -n "${_host_bus_}" ]; then QEMU_USB_ARGS="${QEMU_USB_ARGS},${HOST_BUS_KEY}=${_host_bus_}"; fi
      if [ -n "${_host_addr_}" ]; then QEMU_USB_ARGS="${QEMU_USB_ARGS},${HOST_ADDRESS_KEY}=${_host_addr_}"; fi
      if [ -n "${_host_port_}" ]; then QEMU_USB_ARGS="${QEMU_USB_ARGS},${HOST_PORT_KEY}=${_host_port_}"; fi

      QEMU_USB_ARGS="${QEMU_USB_ARGS} "
      _port_=""
      _vendor_=""
      _product_=""
      _host_bus_=""
      _host_addr_=""
      _host_port_=""
    done
    _type_=""
  done
  IFS=${OLD_IFS}
  QEMU_USB_ARGS_ROOT=sudo
}

parse_args() {
  while [ "${1}" != "" ]; do
    local arg=$1
    local val=$2

    case $arg in
    -f | --file)
      parse_file "$val"
      ;;
    -a | --architecture)
      parse_architecture "$val"
      ;;
    -m | --machine)
      parse_machine "$val"
      ;;
    -b | --bios)
      parse_bios "$val"
      ;;
    -r | --ram)
      parse_ram "$val"
      ;;
    -c | --cpu)
      parse_cpu "$val"
      ;;
    -d | --debug)
      parse_debug "$val"
      ;;
    -g | --gdb)
      start_gdb
      ;;
    -h | --help)
      print_usage
      exit 0
      ;;
    -u | --usb)
      parse_usb "$val"
      ;;  
    *)
      printf "Unknown option '%s'\\n" "${arg}"
      print_usage
      exit 1
      ;;
    esac
    shift 2
  done
}

run_qemu() {
  local command="${QEMU_BIN}"

  if [ -n "${QEMU_MACHINE}" ]; then
    command="${command} -machine ${QEMU_MACHINE}"
  fi

  if [ -n "${QEMU_BIOS}" ]; then
    command="${command} -bios ${QEMU_BIOS}"
  fi

  command="${command} -m ${QEMU_RAM} -cpu ${QEMU_CPU} ${QEMU_ARGS} ${QEMU_BOOT_DEVICE} ${QEMU_STORAGE_ARGS} ${QEMU_NETWORK_ARGS} ${QEMU_AUDIO_ARGS} ${QEMU_USB_ARGS}"
  
  printf "Running: %s\\n" "${command}"

  if [ -n "${QEMU_GDB_PORT}" ]; then
    if [ "${QEMU_GDB_PORT}" == "1234" ]; then
      ${QEMU_USB_ARGS_ROOT} $command -gdb tcp::"${QEMU_GDB_PORT}" -S &
    else
      ${QEMU_USB_ARGS_ROOT} $command -gdb tcp::"${QEMU_GDB_PORT}" -S
    fi
  else
    $command
  fi
}

parse_args "$@"

if [ "${QEMU_BOOT_DEVICE}" == "" ]; then
  printf "No bootable image found!\\n"
  exit 1
fi

get_ovmf

QEMU_ARGS="${QEMU_ARGS}"
set_audio_parameters

run_qemu
