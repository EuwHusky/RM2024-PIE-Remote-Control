# RoboMaster 遥控系统

## 请自行添加配置文件

## 文件名

config.h

## 文件内容

```C

#ifndef _RC_CONFIG_H__
#define _RC_CONFIG_H__

// 是否启用图传键鼠数据
#define USE_VT_LINK (true)
// 是否启用自定义控制器
#define USE_CUSTOMER_CONTROLLER (false)

// 控制配置
#define RC_KEY_PRESS_JUDGE_TIME (6)
#define RC_DT7_ROCKER_DEADLINE (2)

#endif /* _RC_CONFIG_H__ */


```
