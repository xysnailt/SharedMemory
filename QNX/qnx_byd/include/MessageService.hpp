#pragma once
#include "VehicleConfigManage.hpp"

#include <functional>
#include <map>
#include <stdint.h>
#include <vector>

namespace BYDD9Msg {
/**
------------------------------------------------------
标识符        帧内容长度            帧内容            标识符
------------------------------------------------------
        |                |   功能码    数据区     |
BYTE          WORD             BYTE    BYTE[N]        BYTE
------------------------------------------------------
0xFE                                            0xFE
------------------------------------------------------

备注:
    BYTE：表示1个字节
    WORD: 表示2个字节
    标志位：采用0xFE表示，如果帧长度/内容中出现0xFE，则要进行转义，处理规则如下：
    0xFE  -> 0xFD 0x02
    0xFD  -> 0xFD 0x01
    帧内容长度为 N+1，N小于2048
**/

class MessageService final {
public:
    enum class FunctionCode : uint8_t {
        qnx_service_get = 0xD0,
        dms_switch_set = 0xD1,
        dms_service_get = 0xD2,
        drow_alarm_suspend_15m = 0xD3,
        dms_workmode_set = 0xD5,
        dms_workmode_get = 0xD6,
        drow_switch_set = 0xD7,
        drow_switch_get = 0xD8,
        dist_switch_set = 0xD9,
        dist_switch_get = 0xDA,
        dms_selfcheck_status = 0xE0,
        dms_drow_status = 0xE1,
        dms_dist_status = 0xE2,
        dms_occlusion_status = 0xE3,
        driver_no_response = 0xE4,
        drow_alarm_resume = 0xE5
    };

    enum class DmsSwitch : uint8_t { dms_invalid = 0x00, dms_open, dms_close };
    enum class DmsWorkMode : uint8_t { standard_mode = 0x00, sensitive_mode, factory_mode };
    enum class DrowSwitch : uint8_t { drow_invalid = 0x00, drow_open, drow_close };
    enum class DistSwitch : uint8_t { dist_invalid = 0x00, dist_open, dist_close };
    enum class DmsSelfCheckStatus : uint8_t {
        normal = 0x00,
        unauthorized,
        camera_fault,
        algorithm_fault,
        other
    };
    enum class DrowLevel : uint8_t {
        level_drowsy = 0x01,
        level_nap,
        level_sleep,
        level_no_reaction
    };
    enum class DistLevel : uint8_t { level_short = 0x01, level_long };
    enum class OccType : uint8_t { occ_eye = 0x01, occ_head, occ_camera };

    using proc_func_t =
        std::function< void(const int fd, const uint8_t* data, const int16_t data_len) >;

public:
    // ----------------------------------
    //! @brief Construct MessageService with buff length
    //! @param[in] recv_buff_len : will allocate memery for data recv.
    //! @param[in] send_buff_len : will allocate memery for data send.
    MessageService(int recv_buff_len, int send_buff_len) {
        m_recv_buff_len = recv_buff_len;
        m_send_buff_len = send_buff_len;
        m_recv_buff = new uint8_t[m_recv_buff_len];
        m_send_buff = new uint8_t[m_send_buff_len];
    }
    ~MessageService() {
        if (nullptr != m_recv_buff) {
            delete[] m_recv_buff;
        }
        if (nullptr != m_send_buff) {
            delete[] m_send_buff;
        }
    }
    MessageService(const MessageService& other) = delete;
    MessageService& operator=(const MessageService& other) = delete;
    MessageService(const MessageService&& other) = delete;
    MessageService& operator=(const MessageService&& other) = delete;

    // ----------------------------------
    //! @brief message recv and proc
    //! @param[in] fd : socket fd.
    int MessageRecv(int fd);
    // ----------------------------------
    //! @brief register process func with message code
    //! @param[in] func_code : message id.
    //! @param[in] func : process func.
    //! @return if func invalid, will return false
    bool MsgProcFuncRegister(FunctionCode func_code, proc_func_t func);
    // ----------------------------------
    //! @brief batch register process func
    int FuncBatchRegister();

    // ----------------------------------
    //! @brief encode send data by protocol
    //! @param[out] out_data : buff for saving encoded data.
    //! @param[in] out_len : the length of encoded buff.
    //! @param[in] in_data : data to be encoded.
    //! @param[in] in_len : the length of data to be encoded.
    //! @return
    static int
    DataEncode(uint8_t* out_data, const int out_len, const uint8_t* in_data, const int in_len);
    // ----------------------------------
    //! @brief decode recv data by protocol
    //! @param[out] out_data : buff for saving decoded data.
    //! @param[in] out_len : the length of decoded buff.
    //! @param[in] in_data : data to be decoded.
    //! @param[in] in_len : the length of data to be decoded.
    //! @return
    static int
    DataDecode(uint8_t* out_data, const int out_len, const uint8_t* in_data, const int in_len);
    // ----------------------------------
    //! @brief Send data by socket
    //! @param[in] fd : socket fd.
    //! @param[in] data : Data to be sent.
    //! @param[in] data_len : The length of data to be sent.

    int DmsResultProc(const int fd, const SDResult& result);

    int SendMessage(const int fd,
                    const FunctionCode func_code,
                    const uint8_t* data,
                    const int data_len);
    static int SendData(const int fd, const uint8_t* data, const int data_len);

private:
    void MsgRecvProc(const int fd, const uint8_t* data, const int data_len);
    void DataHexShow(const uint8_t* data, const int data_len);

private:
    static constexpr int max_massage_len{2048 + 4};
    int m_valid_pos{0};
    int m_recv_buff_len{0};
    int m_send_buff_len{0};
    uint8_t* m_recv_buff{nullptr};
    uint8_t* m_send_buff{nullptr};
    std::map< FunctionCode, proc_func_t > m_msg_prov_list;
    std::map< MDrowsinessLevel, uint8_t > m_drow_level_map{
        {M_DROWSINESS_LEVEL_LIGHT, static_cast< uint8_t >(DrowLevel::level_drowsy)},
        {M_DROWSINESS_LEVEL_MEDIUM, static_cast< uint8_t >(DrowLevel::level_drowsy)},
        {M_DROWSINESS_LEVEL_HEAVY, static_cast< uint8_t >(DrowLevel::level_drowsy)},
        {M_DROWSINESS_LEVEL_MICRO_SLEEP, static_cast< uint8_t >(DrowLevel::level_nap)},
        {M_DROWSINESS_LEVEL_SLEEP, static_cast< uint8_t >(DrowLevel::level_sleep)},
        {M_DROWSINESS_LEVEL_UNRESPONSIVE, static_cast< uint8_t >(DrowLevel::level_no_reaction)}};
    std::map< MDistAlarmType, uint8_t > m_dist_level_map{
        {MDistAlarmType::TDistLong, static_cast< uint8_t >(DistLevel::level_long)},
        {MDistAlarmType::TDistShort, static_cast< uint8_t >(DistLevel::level_short)}};
};

} // namespace BYDD9Msg
