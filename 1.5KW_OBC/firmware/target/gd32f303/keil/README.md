# Keil MDK 工程

工程入口：`OBC_1p5KW.uvprojx`

## 环境

- Keil MDK 5.27 或更高版本
- Arm Compiler 6
- `GigaDevice.GD32F30x_DFP.2.6.0.pack`

器件选择为 `GD32F303RET6`，Flash 512 KB，SRAM 64 KB。初版系统时钟
安全地使用板载 8 MHz HSE；若晶振启动失败则保持 8 MHz IRC。

## 编译

1. 安装 GD32F30x DFP。
2. 用 Keil 打开 `OBC_1p5KW.uvprojx`。
3. 执行 `Rebuild`。
4. 输出位于 `Objects/`：
   - `OBC_1p5KW_Initial.axf`
   - `OBC_1p5KW_Initial.hex`

## 安全限制

- `APP_ALLOW_POWER_ENABLE=0` 必须保持不变。
- PFC 继电器、LLC、输出继电器和两路 CV/CC 调节输出保持关闭。
- PC11 不配置；LLC 仅映射到 PB15，等待 PCB 网表确认同名网络问题。
- 本版本只用于低压上电、SWD、ADC 原始采样和硬件故障链检查。

仓库中的 GCC 构建使用同一组应用源码和裸机板级实现，输出
`.elf/.hex/.bin`，用于持续验证启动文件、链接地址和目标代码可编译性。
