#include "string.h"

#include "remote_control.h"

#include "referee.h"

#define PRESS_SEN 6

remote_control_s remote_control_data;

const remote_control_s *getRemoteControlPointer(void)
{
    return &remote_control_data;
}

void RemoteControlInit(const rfl_dt7_dr16_data_s *dt7_dr16_data, const vt_link_remote_control_t *vt_link_remote_control
#if USE_CUSTOMER_CONTROLLER
                       ,
                       const custom_robot_data_t *customer_controlle_data
#endif /* USE_CUSTOMER_CONTROLLER */
)
{
    memset(&remote_control_data, 0, sizeof(remote_control_s));

    remote_control_data.use_vt_link_control = USE_VT_LINK;

    remote_control_data.dt7_dr16_data = dt7_dr16_data;
    remote_control_data.vt_link_data = vt_link_remote_control;
#if USE_CUSTOMER_CONTROLLER
    remote_control_data.cc_data = customer_controlle_data;
#endif /* USE_CUSTOMER_CONTROLLER */
}

void RemoteControlUpdate(bool is_dt7_connected, bool is_vt_connected)
{
    if (is_dt7_connected)
        remote_control_data.status = RC_USE_DT7;
    else if (is_vt_connected)
        remote_control_data.status = RC_USE_VT_LINK;
    else
        remote_control_data.status = RC_LOSE;

    remote_control_data.use_vt_link_control = is_dt7_connected ? false : true;

    remote_control_data.vt_link_status = is_vt_connected;

    // 常规遥控控制器

    if (remote_control_data.use_vt_link_control)
    {
        int temp_value = 0;
        remote_control_data.mouse_x = (temp_value = remote_control_data.dt7_dr16_data->mouse.x,
                                       temp_value ? temp_value : (remote_control_data.vt_link_data->mouse_x));
        remote_control_data.mouse_y = (temp_value = remote_control_data.dt7_dr16_data->mouse.y,
                                       temp_value ? temp_value : (remote_control_data.vt_link_data->mouse_y));
        remote_control_data.mouse_z = (temp_value = remote_control_data.dt7_dr16_data->mouse.z,
                                       temp_value ? temp_value : (remote_control_data.vt_link_data->mouse_z));
        remote_control_data.rc_keys[RC_LEFT].is_pressed =
            (temp_value = remote_control_data.dt7_dr16_data->mouse.press_l,
             temp_value ? temp_value : ((uint8_t)remote_control_data.vt_link_data->left_button_down));
        remote_control_data.rc_keys[RC_RIGHT].is_pressed =
            (temp_value = remote_control_data.dt7_dr16_data->mouse.press_r,
             temp_value ? temp_value : ((uint8_t)remote_control_data.vt_link_data->right_button_down));
        for (uint8_t i = RC_W; i <= RC_B; i++)
        {
            remote_control_data.rc_keys[i].is_pressed =
                (temp_value = ((remote_control_data.dt7_dr16_data->key.v >> (i - RC_W)) & 1),
                 temp_value ? temp_value : ((remote_control_data.vt_link_data->keyboard_value >> (i - RC_W)) & 1));
        }
    }
    else
    {
        remote_control_data.mouse_x = remote_control_data.dt7_dr16_data->mouse.x;
        remote_control_data.mouse_y = remote_control_data.dt7_dr16_data->mouse.y;
        remote_control_data.mouse_z = remote_control_data.dt7_dr16_data->mouse.z;
        remote_control_data.rc_keys[RC_LEFT].is_pressed = remote_control_data.dt7_dr16_data->mouse.press_l;
        remote_control_data.rc_keys[RC_RIGHT].is_pressed = remote_control_data.dt7_dr16_data->mouse.press_r;
        for (uint8_t i = RC_W; i <= RC_B; i++)
        {
            remote_control_data.rc_keys[i].is_pressed = ((remote_control_data.dt7_dr16_data->key.v >> (i - RC_W)) & 1);
        }
    }

    for (uint8_t i = 0; i < RC_KEY_NUM; i++)
    {
        if (remote_control_data.rc_keys[i].is_pressed)
        {
            remote_control_data.rc_keys[i].press_timer = (remote_control_data.rc_keys[i].press_timer < 255)
                                                             ? (remote_control_data.rc_keys[i].press_timer + 1)
                                                             : 255;
            if (remote_control_data.rc_keys[i].press_timer == PRESS_SEN)
            {
                remote_control_data.rc_keys[i].falling_edge_detected = 1;
                remote_control_data.rc_keys[i].was_pressed = 1;
            }
        }
        else
        {
            remote_control_data.rc_keys[i].press_timer = 0;
        }

        if (remote_control_data.rc_keys[i].was_pressed == 1 && !remote_control_data.rc_keys[i].is_pressed)
        {
            remote_control_data.rc_keys[i].rising_edge_detected = 1;
            remote_control_data.rc_keys[i].was_pressed = 0;
        }
    }

    // 自定义控制器

#if USE_CUSTOMER_CONTROLLER
    for (uint8_t i = 0; i < CC_KEY_NUM; i++)
    {
        remote_control_data.cc_keys[i].is_pressed =
            checkIfCustomerControllerKeyPressed(remote_control_data.cc_data->key, i) ? 1 : 0;
    }

    for (uint8_t i = 0; i < CC_KEY_NUM; i++)
    {
        if (remote_control_data.cc_keys[i].is_pressed)
        {
            remote_control_data.cc_keys[i].press_timer = (remote_control_data.cc_keys[i].press_timer < 255)
                                                             ? (remote_control_data.cc_keys[i].press_timer + 1)
                                                             : 255;
            if (remote_control_data.cc_keys[i].press_timer == PRESS_SEN)
            {
                remote_control_data.cc_keys[i].falling_edge_detected = 1;
                remote_control_data.cc_keys[i].was_pressed = 1;
            }
        }
        else
        {
            remote_control_data.cc_keys[i].press_timer = 0;
        }

        if (remote_control_data.cc_keys[i].was_pressed == 1 && !remote_control_data.cc_keys[i].is_pressed)
        {
            remote_control_data.cc_keys[i].rising_edge_detected = 1;
            remote_control_data.cc_keys[i].was_pressed = 0;
        }
    }
#endif /* USE_CUSTOMER_CONTROLLER */
}

remote_control_status_e getRemoteControlStatus(void)
{
    return remote_control_data.status;
}

bool getVtLinkStatus(void)
{
    return remote_control_data.vt_link_status;
}

int16_t getDt7RockerPosition(dt7_rocker_channel_index_e rocker_channel)
{
    return remote_control_data.dt7_dr16_data->rc.ch[rocker_channel];
}

int16_t getDt7ThumbWheelPosition(void)
{
    return remote_control_data.dt7_dr16_data->rc.ch[4];
}

dt7_toggle_switch_position_e getDt7ToggleSwitchPosition(dt7_toggle_switch_index_e toggle_switch)
{
    return (dt7_toggle_switch_position_e)remote_control_data.dt7_dr16_data->rc.s[toggle_switch];
}

int16_t getRcMouseX(void)
{
    return remote_control_data.mouse_x;
}

int16_t getRcMouseY(void)
{
    return remote_control_data.mouse_y;
}

int16_t getRcMouseZ(void)
{
    return remote_control_data.mouse_z;
}

uint8_t checkIfRcKeyPressed(remote_control_key_e key)
{
    return remote_control_data.rc_keys[key].is_pressed;
}

uint8_t checkIfRcKeyFallingEdgeDetected(remote_control_key_e key)
{
    if (remote_control_data.rc_keys[key].falling_edge_detected)
        return remote_control_data.rc_keys[key].falling_edge_detected = 0, 1;

    return 0;
}

uint8_t checkIfRcKeyRisingEdgeDetected(remote_control_key_e key)
{
    if (remote_control_data.rc_keys[key].rising_edge_detected)
        return remote_control_data.rc_keys[key].rising_edge_detected = 0, 1;

    return 0;
}

uint8_t checkIfAnyRcKeyPressed(void)
{
    for (uint8_t i = 0; i < RC_KEY_NUM; i++)
        if (remote_control_data.rc_keys[i].is_pressed)
            return true;

    return false;
}

#if USE_CUSTOMER_CONTROLLER
float getCcPose(uint8_t pose_index)
{
    return remote_control_data.cc_data->pose[pose_index];
}

uint8_t checkIsCcKeyPressed(customer_controller_key_index_e key)
{
    return remote_control_data.cc_keys[key].is_pressed;
}

uint8_t checkIfCcKeyFallingEdgeDetected(customer_controller_key_index_e key)
{
    if (remote_control_data.cc_keys[key].falling_edge_detected)
        return remote_control_data.cc_keys[key].falling_edge_detected = 0, 1;

    return 0;
}

uint8_t checkIfCcKeyRisingEdgeDetected(customer_controller_key_index_e key)
{
    if (remote_control_data.cc_keys[key].rising_edge_detected)
        return remote_control_data.cc_keys[key].rising_edge_detected = 0, 1;

    return 0;
}

#endif /* USE_CUSTOMER_CONTROLLER */
