# 1.5 kW OBC

## 项目目标

开发一套额定功率为 1.5 kW 的车载充电机（OBC）。

## 已确认硬件架构

- 主控：GD32F303RET6，外部 8 MHz 晶振
- 前级：NCP1654 系列模拟 PFC 控制器
- 后级：NCP4390 系列模拟 LLC 控制器
- MCU 职责：安全时序、CV/CC 调节 PWM、继电器与风扇控制、ADC 采样、CAN 通信和故障管理
- CAN：ISO1050 隔离收发器
- 功率级驱动：隔离式栅极驱动器

> 原理图表明 MCU 不直接产生 PFC/LLC 功率管驱动波形。`CV_PWM` 和
> `CUR_PWM` 是模拟控制环的调节量，功率级开关由专用控制器完成。

## 目录

- `hardware/schematic/`：原理图
- `firmware/`：GD32F303 初始固件
- `docs/`：引脚表、硬件审查结果和待确认项
- `simulation/`：PFC、LLC 与系统仿真（待补充）
- `test/`：测试方案、数据和波形（待补充）

## 当前版本

初始固件采用安全默认值：

- PFC 继电器关闭
- LLC 禁止
- 输出继电器关闭
- `CV_PWM`、`CUR_PWM` 为 0%
- 风扇和状态灯关闭
- 硬件故障清除脚保持非激活
- `APP_ALLOW_POWER_ENABLE=0`，软件层禁止开启功率级

详见 [`firmware/README.md`](firmware/README.md) 和
[`docs/hardware-review.md`](docs/hardware-review.md)。
