#include "can_receiver.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <chrono>
#include <cerrno>

CanReceiver::CanReceiver()
    : can_socket_(-1), running_(false) {}

CanReceiver::~CanReceiver() { 
    stop(); 
}

bool CanReceiver::initialize(const std::string& can_interface) {
    can_interface_ = can_interface;

    // 创建CAN socket
    can_socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (can_socket_ < 0) {
        std::cerr << "Error: Cannot create CAN socket: " << strerror(errno) << std::endl;
        return false;
    }

    // 获取接口索引
    struct ifreq ifr;
    std::strcpy(ifr.ifr_name, can_interface_.c_str());
    if (ioctl(can_socket_, SIOCGIFINDEX, &ifr) < 0) {
        std::cerr << "Error: Cannot get interface index for " << can_interface_
                  << ": " << strerror(errno) << std::endl;
        close(can_socket_);
        can_socket_ = -1;
        return false;
    }

    // 绑定socket到CAN接口
    struct sockaddr_can addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(can_socket_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error: Cannot bind socket to CAN interface: " << strerror(errno) << std::endl;
        close(can_socket_);
        can_socket_ = -1;
        return false;
    }

    std::cout << "CAN interface " << can_interface_ << " initialized successfully" << std::endl;
    return true;
}

bool CanReceiver::start(CanReceiveCallback callback) {
    if (can_socket_ < 0) {
        std::cerr << "Error: CAN socket not initialized" << std::endl;
        return false;
    }

    if (running_.load()) {
        std::cout << "CAN receiver is already running" << std::endl;
        return true;
    }

    callback_ = callback;
    running_.store(true);

    // 启动工作线程
    worker_thread_ = std::thread(&CanReceiver::receiveThread, this);
    std::cout << "CAN receiver thread started" << std::endl;
    return true;
}

void CanReceiver::stop() {
    if (!running_.load()) return;

    running_.store(false);
    
    // 关闭socket来中断阻塞的read
    if (can_socket_ >= 0) {
        shutdown(can_socket_, SHUT_RD);
    }
    
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    
    if (can_socket_ >= 0) {
        close(can_socket_);
        can_socket_ = -1;
    }
    
    std::cout << "CAN receiver stopped" << std::endl;
}

bool CanReceiver::isRunning() const { 
    return running_.load(); 
}

void CanReceiver::receiveThread() {
    struct can_frame frame;
    CanFrame user_frame;

    std::cout << "CAN receiver thread started on " << can_interface_ 
              << ", waiting for data..." << std::endl;

    while (running_.load()) {
        // 读取CAN帧
        ssize_t nbytes = read(can_socket_, &frame, sizeof(struct can_frame));
        
        if (nbytes < 0) {
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
                // 被中断或暂时无数据，继续循环
                continue;
            } else {
                // 真正的错误
                std::cerr << "Error reading CAN frame: " << strerror(errno) << std::endl;
                break;
            }
        }

        if (nbytes == 0) {
            // socket关闭
            break;
        }

        if (static_cast<size_t>(nbytes) < sizeof(struct can_frame)) {
            std::cerr << "Error: Incomplete CAN frame" << std::endl;
            continue;
        }

        // 获取当前时间戳
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

        // 填充用户帧结构
        user_frame.can_id = frame.can_id;
        user_frame.can_dlc = frame.can_dlc;
        user_frame.timestamp = microseconds;
        std::memcpy(user_frame.data, frame.data, 8);

        // 打印接收到的CAN数据
        std::cout << "=== CAN Frame Received ===" << std::endl;
        std::cout << "  ID: 0x" << std::hex << user_frame.can_id << std::dec << std::endl;
        std::cout << "  DLC: " << static_cast<int>(user_frame.can_dlc) << std::endl;
        std::cout << "  Data: ";
        for (int i = 0; i < user_frame.can_dlc; i++) {
            std::cout << std::hex << static_cast<int>(user_frame.data[i]) << " ";
        }
        std::cout << std::dec << std::endl;
        std::cout << "  Timestamp: " << user_frame.timestamp << " us" << std::endl;
        std::cout << "==========================" << std::endl;

        // 调用回调函数
        if (callback_) {
            callback_(user_frame);
        }
    }
    
    std::cout << "CAN receiver thread stopped" << std::endl;
}
