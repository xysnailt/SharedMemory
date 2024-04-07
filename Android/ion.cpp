#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/ion.h>

#define SHARED_MEMORY_SIZE 1024

int main() {
    int ion_fd = open("/dev/ion", O_RDWR);
    if (ion_fd < 0) {
        perror("Failed to open /dev/ion");
        return EXIT_FAILURE;
    }

    // 设置 ion 分配参数
    struct ion_allocation_data alloc_data = {
        .len = SHARED_MEMORY_SIZE,
        .align = 0,
        .heap_id_mask = ION_HEAP(ION_SYSTEM_HEAP),
        .flags = 0,
    };

    // 使用 ion 分配内存
    if (ioctl(ion_fd, ION_IOC_ALLOC, &alloc_data) < 0) {
        perror("ION_IOC_ALLOC failed");
        close(ion_fd);
        return EXIT_FAILURE;
    }

    // 获取 ion 分配的共享内存的文件描述符
    int shared_mem_fd = alloc_data.fd;

    // 使用 mmap 将共享内存映射到进程地址空间
    void *shared_mem_addr = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_fd, 0);
    if (shared_mem_addr == MAP_FAILED) {
        perror("mmap failed");
        close(ion_fd);
        close(shared_mem_fd);
        return EXIT_FAILURE;
    }

    // 写入数据到共享内存
    snprintf((char*)shared_mem_addr, SHARED_MEMORY_SIZE, "Hello, shared memory!");

    // 打印写入的数据
    printf("Data written to shared memory: %s\n", (char*)shared_mem_addr);

    // 解除共享内存映射
    munmap(shared_mem_addr, SHARED_MEMORY_SIZE);

    // 释放 ion 分配的内存
    struct ion_fd_data free_data = {
        .fd = shared_mem_fd,
    };
    if (ioctl(ion_fd, ION_IOC_FREE, &free_data) < 0) {
        perror("ION_IOC_FREE failed");
        close(ion_fd);
        return EXIT_FAILURE;
    }

    // 关闭文件描述符
    close(ion_fd);

    return EXIT_SUCCESS;
}
