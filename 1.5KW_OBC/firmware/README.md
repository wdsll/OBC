# 1.5 kW OBC 初始固件

这是根据 `Main_wf2.pdf` 建立的 GD32F303RET6 安全上电骨架。

## 当前能力

- 上电第一时间把 PFC 继电器、LLC、输出继电器、风扇和两路调节 PWM
  置为安全值
- 1 ms 系统节拍
- 11 路 ADC 原始值接口
- 硬件故障锁存监测与低脉冲清除接口
- CAN0 端口和协议服务占位
- 可在主机上编译运行的安全状态机单元测试

## 目录

- `include/`：应用与硬件抽象接口
- `src/`：安全状态机和应用入口
- `target/gd32f303/`：GD32F30x SPL 板级实现
- `tests/`：不依赖 MCU SDK 的主机单元测试

## 主机测试

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## 目标工程接入

目标代码依赖 GigaDevice `GD32F30x_Firmware_Library`。把以下文件加入
Keil/IAR/GCC 工程：

- `src/main.c`
- `src/app.c`
- `src/safety_manager.c`
- `target/gd32f303/board_gd32f303.c`

并加入本目录 `include/`、`target/gd32f303/` 和 GD32 SPL 的头文件路径。
启动文件、链接脚本、`system_gd32f30x.c` 和 SPL 源文件由正式目标工程提供。

## 安全开关

`include/app_config.h` 中：

```c
#define APP_ALLOW_POWER_ENABLE 0
```

初版必须保持为 `0`。完成 `docs/hardware-review.md` 中的低压确认项后，
再通过独立评审提交修改。仅改成 `1` 也不会自动启动功率级，正式启动状态机
仍需后续实现。
