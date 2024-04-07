#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHARED_MEMORY_NAME "/my_shared_memory"
#define SHARED_MEMORY_SIZE 1024

int main() {
    // 创建或打开共享内存对象
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        std::cerr << "Failed to create shared memory object." << std::endl;
        exit(EXIT_FAILURE);
    }

    // 调整共享内存对象大小
    if (ftruncate(shm_fd, SHARED_MEMORY_SIZE) == -1) {
        std::cerr << "Failed to resize shared memory object." << std::endl;
        exit(EXIT_FAILURE);
    }

    // 将共享内存对象映射到进程地址空间
    void *shm_addr = mmap(nullptr, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_addr == MAP_FAILED) {
        std::cerr << "Failed to map shared memory object into process address space." << std::endl;
        exit(EXIT_FAILURE);
    }

    // 写入数据到共享内存
    std::cout << "Enter a message to write into shared memory: ";
    std::string message;
    getline(std::cin, message);
    strncpy(static_cast<char*>(shm_addr), message.c_str(), SHARED_MEMORY_SIZE);

    // 等待一段时间，模拟另一个进程读取共享内存的数据
    sleep(5);

    // 从共享内存中读取数据
    std::cout << "Data read from shared memory: " << static_cast<char*>(shm_addr) << std::endl;

    // 解除映射共享内存对象
    if (munmap(shm_addr, SHARED_MEMORY_SIZE) == -1) {
        std::cerr << "Failed to unmap shared memory object." << std::endl;
        exit(EXIT_FAILURE);
    }

    // 关闭共享内存文件描述符
    if (close(shm_fd) == -1) {
        std::cerr << "Failed to close shared memory file descriptor." << std::endl;
        exit(EXIT_FAILURE);
    }

    // 删除共享内存对象
    if (shm_unlink(SHARED_MEMORY_NAME) == -1) {
        std::cerr << "Failed to unlink shared memory object." << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}


#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    int fd = ashmem_create_region("my_shared_memory", 1024);
    if (fd < 0) {
        // 创建共享内存失败
        return 1;
    }

    // 将共享内存映射到进程地址空间
    void *ptr = mmap(nullptr, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        // 映射共享内存失败
        close(fd);
        return 1;
    }

    // 在共享内存中进行操作

    // 解除映射共享内存并关闭 Ashmem 区域
    munmap(ptr, 1024);
    close(fd);

    return 0;
}
