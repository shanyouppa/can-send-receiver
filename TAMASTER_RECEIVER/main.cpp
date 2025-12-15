#include "can_receiver.h"
#include <iostream>
#include <unistd.h>

// 自定义CAN数据回调函数
void myCanCallback(const CanFrame& frame) {
    // 这里可以添加自定义处理逻辑
    
    // 示例：只处理特定ID的帧
    if (frame.can_id == 0x430) {
        std::cout << ">>> Special frame with ID 0x1F334455 received!" << std::endl;
    }
}

int main() {
    std::cout << "Starting CAN Receiver Demo..." << std::endl;

    // 创建CAN接收器实例
    CanReceiver receiver;

    // 初始化CAN接口（使用默认的can0）
    if (!receiver.initialize()) {
        std::cerr << "Failed to initialize CAN interface" << std::endl;
        return -1;
    }

    // 启动CAN接收（传入自定义回调函数）
    if (!receiver.start(myCanCallback)) {
        std::cerr << "Failed to start CAN receiver" << std::endl;
        return -1;
    }

    std::cout << "CAN receiver is running in background thread." << std::endl;
    std::cout << "Main program continues to execute..." << std::endl;

    // 主程序可以继续执行其他任务
    int counter = 0;
    while (counter < 10) {  // 运行10次后自动停止
        // 模拟主程序的其他工作
        std::cout << "Main program working... (" << ++counter << ")" << std::endl;
        sleep(2); // 每2秒打印一次
    }

    // 停止CAN接收
    std::cout << "Stopping CAN receiver..." << std::endl;
    receiver.stop();

    std::cout << "CAN Receiver Demo stopped successfully." << std::endl;
    return 0;
}
