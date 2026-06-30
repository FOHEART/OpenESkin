# 更新日志

## [1.0.1] - 2026-06-30

### 修复

- **修复 Linux (Wayland) 下无法启动的问题**

  在 GNOME on Wayland 环境下通过 VS Code 终端运行 OpenESkin 时，应用内置的 `elevatePrivileges()` 函数会通过 `pkexec` 自动提权为 root。提权后的 root 进程无法访问当前用户的 X11 显示（缺少 XAUTHORITY 认证 cookie），导致 Qt 报错并崩溃：

  ```
  qt.qpa.xcb: could not connect to display
  qt.qpa.plugin: Could not load the Qt platform plugin "xcb" in "" even though it was found.
  ```

  **解决方案**：在 `elevatePrivileges()` 中添加了两层保护：

  1. **`OPENESKIN_NO_ELEVATE=1` 环境变量开关** — 设置后可跳过 pkexec 提权，以当前用户身份直接运行。适用于 VS Code 终端、SSH 等无图形化认证代理的环境。
  2. **Display 检查** — 仅在 `DISPLAY` 或 `WAYLAND_DISPLAY` 已设置时才尝试 pkexec，避免在无显示环境中盲目提权。

  **使用方式**：

  ```bash
  OPENESKIN_NO_ELEVATE=1 ./build/OpenESkin
  # 或 export 后直接运行
  export OPENESKIN_NO_ELEVATE=1
  ./build/OpenESkin
  ```

  > 在桌面环境中直接双击运行仍可正常使用 pkexec 提权（会弹出图形化密码框），不受此改动影响。
