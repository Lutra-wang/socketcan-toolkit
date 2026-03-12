# SocketCAN Toolkit (WSL2 Mock)

一个面向车载通信学习的最小可复现项目：在 **WSL2** 环境下用 **CSV Mock** 模拟 CAN 报文流，实现 **报文生成 → 解析/过滤 → 日志录制** 的工程闭环。

覆盖能力点：**Linux 编译/运行/调试（gcc/gdb）**、**构建系统（CMake/Makefile）**、**CAN 帧结构解析（ID/DLC/Data）**、**过滤（mask）**、**可复现测试（CSV 日志）**。

> 说明：WSL2 默认内核通常缺少 `vcan` 模块，无法创建 `vcan0`，因此本项目默认使用 `--mock / --mock_out` 跑通流程。未来在原生 Linux 下可无缝切换到真实 SocketCAN（can0/vcan0）。

---

## Features

- **can_send**：生成 CAN 帧流  
  - WSL2：`--mock_out demo.csv` 生成 CSV 报文流（模拟 ECU 周期报文）

- **can_dump**：解析/过滤/录制  
  - WSL2：`--mock demo.csv` 从 CSV 读取并解析  
  - 支持 `--filter 0x123:0x7FF`（id:mask）过滤  
  - 支持 `--log out.csv` 录制过滤后的日志

---

## Repo Structure

```text
socketcan-toolkit/
  src/
    can_common.h   # 时间戳、hex 编解码、帧打印
    mock_io.h      # CSV mock 读写：csv_read_frame / csv_write_frame
    can_send.c     # --mock_out 生成报文流（demo.csv）
    can_dump.c     # --mock 读取报文流 + --filter 过滤 + --log 录制
  CMakeLists.txt
  Makefile
  README.md
  .gitignore
