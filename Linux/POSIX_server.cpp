#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <thread>

using namespace std;

#define SHARED_MEMORY_NAME "my_shared_memory_001"
#define SHARED_MEMORY_SIZE 1024

int main() {
    // 创建或打开共享内存对象
    // SHARED_MEMORY_NAME 是共享内存对象的名称，它是一个字符串，用来唯一标识共享内存对象。在整个系统中，这个名称应该是唯一的。
    // O_CREAT | O_RDWR 是标志位参数，O_CREAT 表示如果指定的共享内存对象不存在则创建它，O_RDWR 表示打开共享内存对象后可读可写。如果只想打开已存在的共享内存对象而不创建新的，可以省略 O_CREAT 标志。
    // S_IRUSR | S_IWUSR 是权限参数，指定了共享内存对象的权限。S_IRUSR 表示允许拥有者（user）读取，S_IWUSR 表示允许拥有者写入。
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        cerr << "Failed to create shared memory object." << endl;
        exit(EXIT_FAILURE);
    }

    // 调整共享内存对象大小
    // ftruncate() 函数用于调整共享内存对象的大小
    // shm_fd 是共享内存对象的文件描述符，它是由之前调用 shm_open() 函数返回的。
    // SHARED_MEMORY_SIZE 是共享内存对象的新大小，以字节为单位。
    if (ftruncate(shm_fd, SHARED_MEMORY_SIZE) == -1) {
        cerr << "Failed to resize shared memory object." << endl;
        exit(EXIT_FAILURE);
    }

    // 将共享内存对象映射到进程地址空间        
    // mmap() 函数用于将共享内存对象映射到当前进程的地址空间中
    // nullptr 表示指定地址，即让系统自动选择一个适当的地址将共享内存映射到当前进程的地址空间中。你也可以传递一个指针来指定具体的映射地址。
    // SHARED_MEMORY_SIZE 是共享内存对象的大小，以字节为单位。
    // PROT_READ | PROT_WRITE 是权限参数，表示将共享内存映射为可读可写的。
    // MAP_SHARED 是映射标志，表示共享内存将被映射为多个进程共享的。
    // shm_fd 是共享内存对象的文件描述符，它是由之前调用 shm_open() 函数返回的。
    // 0 是偏移量，表示从共享内存对象的起始位置开始映射
    void *shm_addr = mmap(nullptr, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_addr == MAP_FAILED) {
        cerr << "Failed to map shared memory object into process address space." << endl;
        exit(EXIT_FAILURE);
    }

    // 写入数据到共享内存
    cout << "Enter a message to write into shared memory: ";
    string message;
    getline(cin, message);
    strncpy(static_cast<char*>(shm_addr), message.c_str(), SHARED_MEMORY_SIZE);

    // 在这段代码中，父进程在写入数据到共享内存后并不需要显式地解除对共享内存的映射
    // 这是因为共享内存的映射是在父进程的地址空间中完成的，并且父进程在正常退出时会自动释放它的地址空间，从而自动解除对共享内存的映射
    // 因此，父进程不需要显式地调用 munmap() 函数解除映射。
    // 但是，在某些情况下，如果父进程不是正常退出，而是使用 exit()、_exit() 或者收到一个信号而终止
    // 这时候可能需要显式地调用 munmap() 函数来确保正确释放共享内存。

    // 模拟另一个进程读取共享内存的数据
    pid_t pid = fork();
    if (pid == 0) {
        // 子进程
        // 读取共享内存数据
        cout << "Data read from shared memory by child process: " << static_cast<char*>(shm_addr) << endl;

        // 解除映射共享内存对象
        // munmap() 函数用于解除对共享内存对象的映射
        // shm_addr 是指向共享内存对象映射区域的指针，它是由之前调用 mmap() 函数返回的。
        // SHARED_MEMORY_SIZE 是共享内存对象的大小，以字节为单位。
        if (munmap(shm_addr, SHARED_MEMORY_SIZE) == -1) {
            cerr << "Child process failed to unmap shared memory object." << endl;
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    } else if (pid > 0) {
        // 父进程
        // 等待子进程结束
        wait(nullptr);

        // 打开文件
        std::ofstream file("POSIX_fd.txt");
        if (!file.is_open()) {
            std::cerr << "Failed to open file." << std::endl;
            return 1;
        }

        // 写入内容
        file << pid << std::endl;

        // 等待一段时间
        std::this_thread::sleep_for(std::chrono::seconds(30));

        // 解除映射共享内存对象
        if (munmap(shm_addr, SHARED_MEMORY_SIZE) == -1) {
            cerr << "Parent process failed to unmap shared memory object." << endl;
            exit(EXIT_FAILURE);
        }

        // 关闭共享内存文件描述符
        if (close(shm_fd) == -1) {
            cerr << "Parent process failed to close shared memory file descriptor." << endl;
            exit(EXIT_FAILURE);
        }

        // 删除共享内存对象
        if (shm_unlink(SHARED_MEMORY_NAME) == -1) {
            cerr << "Parent process failed to unlink shared memory object." << endl;
            exit(EXIT_FAILURE);
        }
    } else {
        // fork() 出错
        cerr << "Failed to fork a child process." << endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
