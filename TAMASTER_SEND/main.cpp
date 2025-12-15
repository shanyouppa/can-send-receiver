#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    std::cout << "Starting CAN Sender Test..." << std::endl;

    // 创建CAN socket
    int can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (can_socket < 0) {
        std::cerr << "Error: Cannot create CAN socket" << std::endl;
        return -1;
    }

    // 获取接口索引
    struct ifreq ifr;
    std::strcpy(ifr.ifr_name, "can0");  // 使用can0接口发送
    if (ioctl(can_socket, SIOCGIFINDEX, &ifr) < 0) {
        std::cerr << "Error: Cannot get interface index for can0" << std::endl;
        close(can_socket);
        return -1;
    }

    // 绑定socket到CAN接口
    struct sockaddr_can addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(can_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error: Cannot bind socket to CAN interface" << std::endl;
        close(can_socket);
        return -1;
    }

    std::cout << "CAN interface can0 initialized successfully" << std::endl;

    // 准备CAN帧数据（与您示例中的格式一致）
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));
    
    // 设置CAN ID（与您示例中的ID一致）
    frame.can_id = 0x430;
    frame.can_dlc = 8;  // 数据长度8字节
    
    // 设置数据内容（与您示例中的数据一致）
    frame.data[0] = 0x11;
    frame.data[1] = 0x22;
    frame.data[2] = 0x33;
    frame.data[3] = 0x44;
    frame.data[4] = 0x55;
    frame.data[5] = 0x66;
    frame.data[6] = 0x77;
    frame.data[7] = 0x88;

    // 发送CAN帧
    std::cout << "Sending CAN frame..." << "ID: 0x" << std::hex << frame.can_id << std::dec << std::endl;
    std::cout << "Data: ";
    for (int i = 0; i < frame.can_dlc; i++) {
        std::cout << std::hex << static_cast<int>(frame.data[i]) << " ";
    }
    std::cout << std::dec << std::endl;

    ssize_t bytes_sent = write(can_socket, &frame, sizeof(frame));
    if (bytes_sent != sizeof(frame)) {
        std::cerr << "Error: Failed to send CAN frame" << std::endl;
        close(can_socket);
        return -1;
    }

    std::cout << "CAN frame sent successfully!" << std::endl;

    // 关闭socket
    close(can_socket);
    std::cout << "CAN Sender Test completed." << std::endl;

    return 0;
}
