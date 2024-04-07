#ifndef _HARDWARE_HPP_
#define _HARDWARE_HPP_
#include "cavalry_gen.h"
#include "cavalry_ioctl.h"
#include "cavalry_mem.h"
#include "nnctrl.h"
#include "vproc.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

typedef struct cavalry_mem_s {
  void *virt;
  unsigned long phys;
  unsigned long size;
} cavalry_mem_t;

class hardware {
public:
  hardware() : fd_cavalry(0) {
    memset(&lib_mem, 0, sizeof(lib_mem));
  }

  ~hardware(){};
  static hardware &get_instance() {
    static hardware instance;
    return instance;
  }
  bool InitHardWare() {

    uint32_t size = 0;
    fd_cavalry = open(CAVALRY_DEV_NODE, O_RDWR, 0);
    if (fd_cavalry < 0) {
      std::cout << "open " << CAVALRY_DEV_NODE << " failed" << std::endl;
      return false;
    }
    if (nnctrl_init(fd_cavalry, 0)) {
      std::cout << "cavalry_mem_init err " << std::endl;
      return false;
    }
    if (cavalry_mem_init(fd_cavalry, 0) < 0) {
      std::cout << "cavalry_mem_init err " << std::endl;
      return false;
    }

    if (vproc_init("/usr/local/vproc/vproc.bin", &size) < 0) {
      std::cout << "[hardware]: vproc_init err, can not init "
                   "/usr/local/vproc/vproc.bin"
                << std::endl;
      return false;
    }
    lib_mem.size = size;
    if (cavalry_mem_alloc(&lib_mem.size, &lib_mem.phys, &lib_mem.virt, 0) < 0) {
      std::cout << "[hardware]: cavalry_mem_alloc err " << std::endl;
      return false;
    }
    if (vproc_load(fd_cavalry, (uint8_t *)lib_mem.virt, lib_mem.phys, size) <
        0) {
      std::cout << "[hardware]: cavalry vproc load err " << std::endl;
      return false;
    }
  }
  bool DeInitHardWare() {
    vproc_exit();
    if (lib_mem.virt && lib_mem.size) {
      if (cavalry_mem_free(lib_mem.size, lib_mem.phys, lib_mem.virt) < 0) {
        std::cout << "[hardware]: cavalry_mem_free lib_mem err " << std::endl;
        return -1;
      }
      lib_mem.virt = NULL;
      lib_mem.phys = 0;
      lib_mem.size = 0;
    }
    cavalry_mem_exit();
    nnctrl_exit();
    if (fd_cavalry > 0) {
      close(fd_cavalry);
      fd_cavalry = 0;
    }
  }

private:
  int fd_cavalry;
  cavalry_mem_t lib_mem;
}; // class hardware end

#endif
