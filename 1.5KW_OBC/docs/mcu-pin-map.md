# GD32F303RET6 引脚映射

来源：`hardware/schematic/Main_wf2.pdf`。网络名保留原理图拼写。

## 电源、时钟与调试

| 引脚 | MCU 功能 | 原理图网络 | 用途 |
|---|---|---|---|
| 5 | PD0/OSC_IN | OSCIN | 8 MHz HSE |
| 6 | PD1/OSC_OUT | OSCOUT | 8 MHz HSE |
| 7 | NRST | NRST | 外部复位 |
| 12 | VSSA | AGND | 模拟地 |
| 13 | VDDA | A3V3 | 模拟 3.3 V |
| 46 | PA13 | SWD_DIO | SWD 数据 |
| 49 | PA14 | SWD_CLK | SWD 时钟 |

## 模拟采样

| 引脚 | ADC 通道 | 原理图网络 | 信号 |
|---|---|---|---|
| 15 / PA1 | ADC_IN1 | AC_VOL_SENSE | 交流输入电压 |
| 16 / PA2 | ADC_IN2 | FAN_CS | 风扇电流 |
| 17 / PA3 | ADC_IN3 | BUS_VOL_SENSE | PFC 母线电压 |
| 20 / PA4 | ADC_IN4 | T_SENSE_CASE | 机壳温度 |
| 21 / PA5 | ADC_IN5 | VOUT_SENSE | 输出电压 |
| 22 / PA6 | ADC_IN6 | I_SENSE | 输出电流 |
| 23 / PA7 | ADC_IN7 | T_SENSE_PFC_MOS | PFC MOS 温度 |
| 26 / PB0 | ADC_IN8 | BUS_VOL_ADJ | PFC 母线调节回读/节点 |
| 27 / PB1 | ADC_IN9 | T_SENSE_LLCMOS | LLC MOS 温度 |
| 24 / PC4 | ADC_IN14 | T_SENSE_TR_1 | 变压器温度 |
| 25 / PC5 | ADC_IN15 | VBT_SENSE | 电池电压 |

> 原理图另有 `T_SENSE_INDUCTOR` 网络，但未连接到 U17 的已用 ADC
> 引脚。需要在 PCB/网表中再次确认是否遗漏或改名。

## 控制与状态

| 引脚 | 原理图网络 | 初始方向 | 安全默认值 | 说明 |
|---|---|---:|---:|---|
| 41 / PA8 | CV_PWM | 输出 | 低/0% | 恒压设定 PWM |
| 42 / PA9 | OUT_RELAY | 输出 | 低 | 输出继电器，低为关闭 |
| 51 / PC10 | RELAY_PFC_EN_1 | 输出 | 低 | PFC 输入继电器，低为关闭 |
| 52 / PC11 | LLC_EN | 未配置输入 | - | 与 PB15 同名；初版禁止配置成输出 |
| 53 / PC12 | FAN_CTL | 输出 | 低 | 风扇低边 MOS 控制 |
| 35 / PB14 | HARD_FAULT_CLR | 输出 | 高 | 硬件故障锁存清除，低脉冲有效 |
| 36 / PB15 | LLC_EN | 输出 | 低 | U17 实际 LLC_EN 网络位于 PB15 |
| 34 / PB13 | CUR_PWM | 输出 | 低/0% | 恒流设定 PWM |
| 33 / PB12 | LLC_FAULT_CHECK | 输入 | - | 硬件故障锁存状态 |
| 37 / PC6 | TEST1 | 测试点 | - | 预留测试 |
| 39 / PC8 | RED_LED | 输出 | 低 | 红灯驱动 |
| 40 / PC9 | GREEN_LED | 输出 | 低 | 绿灯驱动 |

### 原理图标注冲突

U17 顶部的 PC11 标注为 `LLC_EN`，右侧 PB15 也标注为 `LLC_EN`。
固件只把 PB15 作为 LLC 管理使能；PC11 暂不配置，等待 PCB 网表确认。
这是上电前必须关闭的原理图审查项。

## 通信

| 引脚 | 外设 | 原理图网络 | 说明 |
|---|---|---|---|
| 44 / PA11 | CAN0_RX | CAN_RXD | ISO1050 逻辑侧接收 |
| 45 / PA12 | CAN0_TX | CAN_TXD | ISO1050 逻辑侧发送 |
| 29 / PB10 | USART2_TX | TXD | 调试串口 |
| 30 / PB11 | USART2_RX | RXD | 调试串口 |
| 58 / PB6 | I2C0_SCL | SCL | 预留 I2C |
| 59 / PB7 | I2C0_SDA | SDA | 预留 I2C |
| 57 / PB5 | GPIO | WP1 | 外部器件写保护 |
