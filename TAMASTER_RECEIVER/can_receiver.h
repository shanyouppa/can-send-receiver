#ifndef CAN_RECEIVER_H
#define CAN_RECEIVER_H

#include <functional>
#include <string>
#include <thread>
#include <atomic>

// CAN帧结构体
struct CanFrame {
    uint32_t can_id;    // CAN标识符
    uint8_t can_dlc;    // 数据长度
    uint8_t data[8];    // 数据
    uint64_t timestamp; // 时间戳(微秒)
};

// CAN接收回调函数类型
using CanReceiveCallback = std::function<void(const CanFrame& frame)>;

class CanReceiver {
public:
    CanReceiver();
    ~CanReceiver();

    // 初始化CAN接口
    bool initialize(const std::string& can_interface = "can0");
    
    // 启动CAN接收线程
    bool start(CanReceiveCallback callback = nullptr);
    
    // 停止CAN接收
    void stop();
    
    // 检查是否正在运行
    bool isRunning() const;

private:
    // CAN接收线程函数
    void receiveThread();

private:
    std::string can_interface_;
    int can_socket_;
    std::atomic<bool> running_;
    CanReceiveCallback callback_;
    std::thread worker_thread_;
};

#endif // CAN_RECEIVER_H
