#include "MessageService.hpp"

#include <iomanip>
#include <iostream>
#include <string.h>
#include <unistd.h>

namespace BYDD9Msg {
int MessageService::FuncBatchRegister() {
    MsgProcFuncRegister(FunctionCode::qnx_service_get,
                        [&](const int fd, const uint8_t* data, const int16_t data_len) {
                            std::cout << "qnx_service_get" << '\n';
                            SendMessage(fd, FunctionCode::qnx_service_get, nullptr, 0);
                        });
    MsgProcFuncRegister(FunctionCode::dms_switch_set,
                        [&](const int fd, const uint8_t* data, const int16_t data_len) {
                            std::cout << "dms_switch_set" << '\n';
                            SendMessage(fd, FunctionCode::dms_switch_set, nullptr, 0);
                        });
    MsgProcFuncRegister(FunctionCode::dms_service_get,
                        [&](const int fd, const uint8_t* data, const int16_t data_len) {
                            std::cout << "dms_service_get" << '\n';
                            uint8_t dms_status = 0x01;
                            SendMessage(fd, FunctionCode::dms_service_get, &dms_status, 1);
                        });
    MsgProcFuncRegister(FunctionCode::drow_alarm_suspend_15m,
                        [&](const int fd, const uint8_t* data, const int16_t data_len) {
                            std::cout << "drow_alarm_suspend_15m" << '\n';
                            SendMessage(fd, FunctionCode::drow_alarm_suspend_15m, nullptr, 0);
                        });
    MsgProcFuncRegister(FunctionCode::dms_workmode_set,
                        [&](const int fd, const uint8_t* data, const int16_t data_len) {
                            std::cout << "dms_workmode_set, data_len: " << data_len << '\n';
                            if (data_len >= 1) {
                                SDWorkMode work_mode{WORK_MODE_INVALID};
                                if (static_cast< uint8_t >(DmsWorkMode::standard_mode) == data[0]) {
                                    work_mode = STANDARD_MODE;
                                } else if (static_cast< uint8_t >(DmsWorkMode::factory_mode) ==
                                           data[0]) {
                                    work_mode = FACTORY_MODE;
                                }
                                FunctionSwitchManage::Instance().SetDmsWorkMode(work_mode);
                            }
                            SendMessage(fd, FunctionCode::dms_workmode_set, nullptr, 0);
                        });
    MsgProcFuncRegister(FunctionCode::dms_workmode_get,
                        [&](const int fd, const uint8_t* data, const int16_t data_len) {
                            std::cout << "dms_workmode_get" << '\n';
                            uint8_t work_mode = static_cast< uint8_t >(DmsWorkMode::standard_mode);
                            auto ret = FunctionSwitchManage::Instance().GetDmsWorkMode();
                            if (FACTORY_MODE == ret) {
                                work_mode = static_cast< uint8_t >(DmsWorkMode::factory_mode);
                            }
                            SendMessage(fd, FunctionCode::dms_workmode_get, &work_mode, 1);
                        });
    MsgProcFuncRegister(FunctionCode::drow_switch_set,
                        [&](const int fd, const uint8_t* data, const int16_t data_len) {
                            std::cout << "drow_switch_set" << '\n';
                            if (data_len >= 1) {
                                int ret = -1;
                                if (static_cast< uint8_t >(DrowSwitch::drow_open) == data[0]) {
                                    ret = FunctionSwitchManage::Instance().SetFunctionSwitch(
                                        FunctionSwitchManage::FunctionType::drowsiness_type, true);
                                } else if (static_cast< uint8_t >(DrowSwitch::drow_close) ==
                                           data[0]) {
                                    ret = FunctionSwitchManage::Instance().SetFunctionSwitch(
                                        FunctionSwitchManage::FunctionType::drowsiness_type, false);
                                }
                                if (0 == ret) {
                                    std::cout << "Set drow switch success\n";
                                }
                            }
                            SendMessage(fd, FunctionCode::drow_switch_set, nullptr, 0);
                        });
    MsgProcFuncRegister(
        FunctionCode::drow_switch_get,
        [&](const int fd, const uint8_t* data, const int16_t data_len) {
            std::cout << "drow_switch_get" << '\n';
            uint8_t drow_switch = static_cast< uint8_t >(DrowSwitch::drow_invalid);
            bool switch_status{true};
            if (FunctionSwitchManage::Instance().GetFunctionSwitch(
                    FunctionSwitchManage::FunctionType::drowsiness_type, switch_status)) {
                drow_switch = switch_status ? static_cast< uint8_t >(DrowSwitch::drow_open)
                                            : static_cast< uint8_t >(DrowSwitch::drow_close);
            }
            SendMessage(fd, FunctionCode::drow_switch_get, &drow_switch, 1);
        });
    MsgProcFuncRegister(
        FunctionCode::dist_switch_set,
        [&](const int fd, const uint8_t* data, const int16_t data_len) {
            std::cout << "dist_switch_set" << '\n';
            if (data_len >= 1) {
                int ret = -1;
                if (static_cast< uint8_t >(DistSwitch::dist_open) == data[0]) {
                    ret = FunctionSwitchManage::Instance().SetFunctionSwitch(
                        FunctionSwitchManage::FunctionType::distraction_type, true);
                } else if (static_cast< uint8_t >(DistSwitch::dist_close) == data[0]) {
                    ret = FunctionSwitchManage::Instance().SetFunctionSwitch(
                        FunctionSwitchManage::FunctionType::distraction_type, false);
                }
                if (0 == ret) {
                    std::cout << "Set dist switch success\n";
                }
            }
            SendMessage(fd, FunctionCode::dist_switch_set, nullptr, 0);
        });
    MsgProcFuncRegister(
        FunctionCode::dist_switch_get,
        [&](const int fd, const uint8_t* data, const int16_t data_len) {
            std::cout << "dist_switch_get" << '\n';
            uint8_t dist_switch = static_cast< uint8_t >(DistSwitch::dist_invalid);
            bool switch_status{true};
            if (FunctionSwitchManage::Instance().GetFunctionSwitch(
                    FunctionSwitchManage::FunctionType::distraction_type, switch_status)) {
                dist_switch = switch_status ? static_cast< uint8_t >(DistSwitch::dist_open)
                                            : static_cast< uint8_t >(DistSwitch::dist_close);
            }
            SendMessage(fd, FunctionCode::dist_switch_get, &dist_switch, 1);
        });
    MsgProcFuncRegister(FunctionCode::dms_selfcheck_status,
                        [&](const int fd, const uint8_t* data, const int16_t data_len) {
                            std::cout << "dms_selfcheck_status" << '\n';
                            uint8_t dist_switch = 0x00;
                            SendMessage(fd, FunctionCode::dms_selfcheck_status, &dist_switch, 1);
                        });

    return 0;
}

int MessageService::DataEncode(uint8_t* out_data,
                               const int out_len,
                               const uint8_t* in_data,
                               const int in_len) {
    if (nullptr == out_data || nullptr == in_data || 0 >= in_len || in_len * 2 > out_len) {
        return -1;
    }

    int data_idx = 0;
    out_data[data_idx++] = 0xFE;
    for (int i = 0; i < in_len; ++i) {
        if (in_data[i] == 0xFE) {
            out_data[data_idx++] = 0xFD;
            out_data[data_idx++] = 0x02;
        } else if (in_data[i] == 0xFD) {
            out_data[data_idx++] = 0xFD;
            out_data[data_idx++] = 0x01;
        } else {
            out_data[data_idx++] = in_data[i];
        }
    }
    out_data[data_idx] = 0xFE;
    return data_idx + 1;
}

int MessageService::DataDecode(uint8_t* out_data,
                               const int out_len,
                               const uint8_t* in_data,
                               const int in_len) {
    if (nullptr == out_data || nullptr == in_data || 0 >= in_len || in_len > out_len) {
        return -1;
    }

    if (in_data[0] != 0xFE || in_data[in_len - 1] != 0xFE) {
        return -1;
    }

    int data_idx = 0;
    for (int i = 1; i < in_len; ++i) {
        if (in_data[i] == 0xFD && in_data[i + 1] == 0x02) {
            out_data[data_idx++] = 0xFE;
            ++i;
        } else if (in_data[i] == 0xFD && in_data[i + 1] == 0x01) {
            out_data[data_idx++] = 0xFD;
            ++i;
        } else {
            out_data[data_idx++] = in_data[i];
        }
    }
    return data_idx;
}

int MessageService::MessageRecv(int fd) {
    bool find = false;
    int sentry = 0;
    ssize_t ret = read(fd, &m_recv_buff[m_valid_pos], max_massage_len);
    // std::cout << "-- read message, ret: " << ret << "  m_valid_pos: " << m_valid_pos << '\n';
    if (0 < ret) {
        for (int i = 0; i < ret; ++i) {
            std::cout << std::hex << int(m_recv_buff[i]) << " ";
        }
        std::cout << '\n';
        m_valid_pos += ret;
        for (int i = 0; i < m_valid_pos; ++i) {
            if (m_recv_buff[i] == 0xFE && !find) {
                find = true;
            } else if (m_recv_buff[i] == 0xFE && find) {
                MsgRecvProc(fd, m_recv_buff, m_valid_pos);
                sentry = i + 1;
                find = false;
            }
        }

        m_valid_pos = m_valid_pos - sentry;
        if (m_valid_pos > 0) {
            memmove(m_recv_buff, &m_recv_buff[sentry], m_valid_pos);
        }
    } else {
        std::cout << "read data failed, errno=" << errno << '\n';
        return -1;
    }

    return 0;
}

void MessageService::MsgRecvProc(const int fd, const uint8_t* data, const int data_len) {
    std::vector< uint8_t > decode_buff;
    decode_buff.resize((data_len + 4) * 2, 0);

    int ret = DataDecode(decode_buff.data(), decode_buff.size(), data, data_len);
    if (0 < ret) {
        // DataHexShow(decode_buff.data(), ret);
        int16_t load_len = 0;
        memcpy(&load_len, &decode_buff[1], 2);
        // std::cout << "load_len: " << load_len << '\n';

        FunctionCode func_code = static_cast< FunctionCode >(decode_buff[3]);
        if (m_msg_prov_list.count(func_code) > 0) {
            m_msg_prov_list[func_code](fd, &decode_buff[4], load_len);
        } else {
            std::cout << "Invalid function code " << std::hex << int(func_code) << '\n';
        }
    } else {
        std::cout << "Decode error" << '\n';
    }
}

void MessageService::DataHexShow(const uint8_t* data, const int data_len) {
    std::cout << "--------------------- data hex show -----------------------" << '\n';
    for (int i = 0; i < data_len; ++i) {
        std::cout << std::hex << int(data[i]) << " ";
        if ((i + 1) % 31 == 0) {
            std::cout << '\n';
        }
    }
}

bool MessageService::MsgProcFuncRegister(FunctionCode func_code, proc_func_t func) {
    if (!func) {
        return false;
    }
    m_msg_prov_list.emplace(func_code, func);
    return true;
}

int MessageService::SendMessage(const int fd,
                                const FunctionCode func_code,
                                const uint8_t* data,
                                const int data_len) {
    if (fd <= 0) {
        return -1;
    }
    constexpr int fixed_format_len = 4;
    std::vector< uint8_t > send_data;
    int buff_len = (data_len > 0) ? data_len + fixed_format_len : 10;
    send_data.resize(buff_len * 2, 0);

    send_data[0] = 0xFE;
    int16_t load_len = data_len + 1;
    memcpy(&send_data[1], &load_len, 2);
    send_data[3] = static_cast< uint8_t >(func_code);
    if (nullptr != data) {
        memcpy(&send_data[4], data, data_len);
    }
    send_data[fixed_format_len + data_len] = 0xFE;

    return SendData(fd, send_data.data(), load_len + fixed_format_len);
}

int MessageService::SendData(const int fd, const uint8_t* data, const int data_len) {
    std::vector< uint8_t > encode_buff;
    encode_buff.resize(data_len * 2, 0);
    int data_size = DataEncode(encode_buff.data(), encode_buff.size(), data, data_len);
    size_t ret = write(fd, encode_buff.data(), data_size);
    if (0 > ret) {
        return -1;
    }
    // std::cout << "Server send data len: " << ret << " data_size: " << data_size
    //           << " data_len: " << data_len << '\n';
    return static_cast< int >(ret);
}

int MessageService::DmsResultProc(const int fd, const SDResult& result) {
    // camera occlusion status
    const auto& camera_occlusion_result = result.camera_occlusion_result;
    if (MResultValid::RESULT_VALID == camera_occlusion_result.result_valid) {
        if (M_CAMERA_OCCLUSION == camera_occlusion_result.result) {
            uint8_t occ_type = static_cast< uint8_t >(OccType::occ_camera);
            SendMessage(fd, FunctionCode::dms_occlusion_status, &occ_type, sizeof(occ_type));
        }

        static MProductCameraOcclusionResult last_level = M_CAMERA_NOT_OCCLUSION;
        if (last_level != camera_occlusion_result.result) {
            last_level = camera_occlusion_result.result;
            std::cout << "camera_occlusion_result: " << camera_occlusion_result.result << "\n";
        }
    }

    if (result.person_number > 0) {
        // person result:
        const auto& person = result.person_array_result[0];
        // person result ----> face result
        const auto& face_result = person.face_result;
        if (MResultValid::RESULT_VALID == face_result.result_valid) {
            const auto& drow_rst = face_result.drowsiness_result;
            if (MResultValid::RESULT_VALID == drow_rst.result_valid) {
                if (M_DROWSINESS_LEVEL_NONE < drow_rst.level &&
                    M_DROWSINESS_LEVEL_INVALID > drow_rst.level) {
                    uint8_t drow_status = m_drow_level_map.at(drow_rst.level);
                    SendMessage(
                        fd, FunctionCode::dms_drow_status, &drow_status, sizeof(drow_status));
                }
                static MDrowsinessLevel last_level = M_DROWSINESS_LEVEL_NONE;
                if (last_level != drow_rst.level) {
                    last_level = drow_rst.level;
                    std::cout << "Drow alarm level " << drow_rst.level << "\n";
                }
            }
            const auto& dist_rst = face_result.distraction_result;
            if (MResultValid::RESULT_VALID == dist_rst.result_valid) {
                if (M_DISTRACTION_LEVEL_HEAVY == dist_rst.level &&
                    MDistAlarmType::TDistInvalid < dist_rst.dist_type &&
                    MDistAlarmType::TDistNone < dist_rst.dist_type) {
                    uint8_t dist_status = m_dist_level_map.at(dist_rst.dist_type);
                    SendMessage(
                        fd, FunctionCode::dms_dist_status, &dist_status, sizeof(dist_status));
                }

                static MDistAlarmType last_type = MDistAlarmType::TDistInvalid;
                if (last_type != dist_rst.dist_type) {
                    last_type = dist_rst.dist_type;
                    std::cout << "Dist alarm type " << static_cast< int >(dist_rst.dist_type)
                              << "\n";
                }
            }
        }
    }
    return 0;
}

} // namespace BYDD9Msg
