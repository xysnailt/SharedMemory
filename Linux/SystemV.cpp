#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>


using namespace std;

#define SHARED_MEMORY_KEY 1234
#define SHARED_MEMORY_SIZE 1024

int main() {
    // 创建共享内存
    int shm_id = shmget(SHARED_MEMORY_KEY, SHARED_MEMORY_SIZE, IPC_CREAT | 0666);
    // SHARED_MEMORY_KEY 是用来唯一标识共享内存段的键值，这个键值在整个系统中必须是唯一的。
    // SHARED_MEMORY_SIZE 是共享内存段的大小，以字节为单位。
    // IPC_CREAT | 0666 是标志位参数，IPC_CREAT 表示如果指定的共享内存不存在则创建它，否则返回已存在的共享内存标识符
    // 0666 表示设置共享内存的权限，这里表示允许所有用户读写该共享内存。
    if (shm_id == -1) {
        cerr << "Failed to create shared memory segment." << endl;
        exit(EXIT_FAILURE);
    }

    // 将共享内存映射到进程地址空间
    void *shm_addr = shmat(shm_id, nullptr, 0);
    // shm_id 是共享内存段的标识符，它是由之前调用 shmget() 函数创建或获取的。
    // nullptr 表示指定地址，即让系统自动选择一个适当的地址将共享内存映射到当前进程的地址空间中。你也可以传递一个指针来指定具体的映射地址。
    // 0 表示标志位，通常设为0表示默认行为。
    // 这行代码的作用是将共享内存段映射到当前进程的地址空间中，并返回一个指向共享内存段的指针，以便后续对共享内存的读写操作
    if (shm_addr == (void *) -1) {
        cerr << "Failed to attach shared memory." << endl;
        exit(EXIT_FAILURE);
    }

    // 写入数据到共享内存
    char *shared_memory = static_cast<char*>(shm_addr);
    cout << "Enter a message to write into shared memory: ";
    string message;
    getline(cin, message);
    strncpy(shared_memory, message.c_str(), SHARED_MEMORY_SIZE);

    // 分离共享内存
    // shmdt() 函数用于将之前通过 shmat() 函数映射到当前进程地址空间的共享内存段分离，这样当前进程就无法再访问这段共享内存了
    if (shmdt(shm_addr) == -1) {
        cerr << "Failed to detach shared memory." << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Data written to shared memory successfully." << endl;

    // 模拟另一个进程读取共享内存的数据
    pid_t pid = fork();
    if (pid == 0) {
        // 子进程
        void *child_shm_addr = shmat(shm_id, nullptr, 0);
        if (child_shm_addr == (void *) -1) {
            cerr << "Child process failed to attach shared memory." << endl;
            exit(EXIT_FAILURE);
        }

        char *child_shared_memory = static_cast<char*>(child_shm_addr);
        cout << "Data read from shared memory by child process: " << child_shared_memory << endl;

        // 分离共享内存
        if (shmdt(child_shm_addr) == -1) {
            cerr << "Child process failed to detach shared memory." << endl;
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    } else if (pid > 0) {
        // 父进程
        // 等待子进程结束
        wait(nullptr);
    } else {
        // fork() 出错
        cerr << "Failed to fork a child process." << endl;
        exit(EXIT_FAILURE);
    }

    // 删除共享内存
    if (shmctl(shm_id, IPC_RMID, nullptr) == -1) {
        cerr << "Failed to delete shared memory segment." << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Shared memory segment deleted successfully." << endl;

    return 0;
}
