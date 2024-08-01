#ifndef _REMOTE_CONTROL_H__
#define _REMOTE_CONTROL_H__

#include "bsp_dt7_dr16.h"

#include "customer_controller.h"

#include "referee_protocol.h"

// 是否启用图传键鼠数据
#define USE_VT_LINK (true)
// 是否启用自定义控制器
#define USE_CUSTOMER_CONTROLLER (false)

#define RC_FRAME_LENGTH 18u
#define RC_DT7_ROCKER_DEADLINE (2)

typedef enum RemoteControlStatus
{
    RC_USE_DT7 = 0,
    RC_USE_VT_LINK,
    RC_LOSE,
} remote_control_status_e;

typedef enum RemoteControlKey
{
    RC_W = 0,
    RC_S,
    RC_A,
    RC_D,
    RC_SHIFT,
    RC_CTRL,
    RC_Q,
    RC_E,
    RC_R,
    RC_F,
    RC_G,
    RC_Z,
    RC_X,
    RC_C,
    RC_V,
    RC_B,

    RC_LEFT,
    RC_RIGHT,

    RC_KEY_NUM
} remote_control_key_e;

typedef struct RemoteControlKeyStatus
{
    uint8_t is_pressed : 1;
    uint8_t falling_edge_detected : 1; /* 按键被按下时置1 读取后清零 */
    uint8_t rising_edge_detected : 1;  /* 按键被抬起时置1 读取后清零 会有误触发 */
    uint8_t was_pressed : 1;
    uint8_t press_timer;
} remote_control_key_status_s;

typedef struct RemoteControl
{
    remote_control_status_e status;
    bool use_vt_link_control;
    bool vt_link_status;

    const rfl_dt7_dr16_data_s *dt7_dr16_data;
    const vt_link_remote_control_t *vt_link_data;
#if USE_CUSTOMER_CONTROLLER
    const custom_robot_data_t *cc_data;
#endif /* USE_CUSTOMER_CONTROLLER */

    int16_t mouse_x; /* 鼠标左右方向速度 右为正 */
    int16_t mouse_y; /* 鼠标前后方向速度 后为正 */
    int16_t mouse_z; /* 鼠标滚轮速度 向前滚为正 */

    remote_control_key_status_s rc_keys[RC_KEY_NUM];
#if USE_CUSTOMER_CONTROLLER
    remote_control_key_status_s cc_keys[CC_KEY_NUM];
#endif /* USE_CUSTOMER_CONTROLLER */
} remote_control_s;

extern const remote_control_s *getRemoteControlPointer(void);

extern void RemoteControlInit(const rfl_dt7_dr16_data_s *dt7_dr16_data,
                              const vt_link_remote_control_t *vt_link_remote_control
#if USE_CUSTOMER_CONTROLLER
                              ,
                              const custom_robot_data_t *customer_controlle_data
#endif /* USE_CUSTOMER_CONTROLLER */
);
extern void RemoteControlUpdate(bool is_dt7_connected, bool is_vt_connected);

extern remote_control_status_e getRemoteControlStatus(void);
extern bool getVtLinkStatus(void);

extern int16_t getDt7RockerPosition(dt7_rocker_channel_index_e rocker_channel);
extern int16_t getDt7ThumbWheelPosition(void);
extern dt7_toggle_switch_position_e getDt7ToggleSwitchPosition(dt7_toggle_switch_index_e toggle_switch);

extern int16_t getRcMouseX(void);
extern int16_t getRcMouseY(void);
extern int16_t getRcMouseZ(void);
extern uint8_t checkIfRcKeyPressed(remote_control_key_e key);
extern uint8_t checkIfRcKeyFallingEdgeDetected(remote_control_key_e key);
extern uint8_t checkIfRcKeyRisingEdgeDetected(remote_control_key_e key);
extern uint8_t checkIfAnyRcKeyPressed(void);

#if USE_CUSTOMER_CONTROLLER
extern float getCcPose(uint8_t pose_index);
extern uint8_t checkIsCcKeyPressed(customer_controller_key_index_e key);
extern uint8_t checkIfCcKeyFallingEdgeDetected(customer_controller_key_index_e key);
extern uint8_t checkIfCcKeyRisingEdgeDetected(customer_controller_key_index_e key);
#endif /* USE_CUSTOMER_CONTROLLER */

#endif /* _REMOTE_CONTROL_H__ */
