# 设置CMake最低版本要求
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)


# 设置项目名称
project(OpenESkin VERSION 1.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用自动处理Qt的元对象编译（moc）、用户界面编译（uic）和资源编译（rcc）
set(CMAKE_AUTOMOC ON)   # 自动处理Q_OBJECT等宏[1](@ref)
set(CMAKE_AUTOUIC ON) # 自动处理.ui文件[1](@ref)
set(CMAKE_AUTORCC ON)   # 自动处理.qrc资源文件[1](@ref)

# 手动指定源文件（请根据您的实际文件修改以下路径）
# CPP文件
set(SRC_FILES
main.cpp
src/appinit.cpp
src/console.cpp
src/crc16_modbus.cpp
ui/AboutDialog.cpp
ui/ExitDialog.cpp
ui/frameDisplay.cpp
ui/MainWindow.cpp
ui/MainWindowSerial.cpp
ui/matrixDisplay.cpp
ui/openSourceDialog.cpp
ui/settingsdialog.cpp
ui/thermometerWidget.cpp
gl/glAxis.cpp
gl/glFsr.cpp
gl/glGrid.cpp
gl/glLoadStl.cpp
gl/glTrackballCamera.cpp
gl/glwidget.cpp
app.rc
    # 添加其他.cpp文件
)

# 头文件
set(HEADER_FILES
inc/appinit.h
inc/commonDef.h
inc/console.h
inc/crc16_modbus.h
inc/modbusMsg.h
ui/AboutDialog.h
ui/ExitDialog.h
ui/frameDisplay.h
ui/MainWindow.h
ui/matrixDisplay.h
ui/openSourceDialog.h
ui/settingsdialog.h
ui/thermometerWidget.h
gl/glAxis.h
gl/glFsr.h
gl/glGrid.h
gl/glInclude.h
gl/glLoadStl.h
gl/glTrackballCamera.h
gl/glwidget.h
    # 添加其他.h文件
)

# UI文件（Qt Designer文件）
set(UI_FILES
ui/AboutDialog.ui
ui/ExitDialog.ui
ui/frameDisplay.ui
ui/MainWidget.ui
ui/matrixDisplay.ui
ui/openSourceDialog.ui
ui/settingsdialog.ui
    # 添加其他.ui文件
)

# QRC文件（资源文件）
set(QRC_FILES
    qss.qrc
    # 添加其他.qrc文件
)

# 查找Qt5库（根据需要添加其他组件，如Core、Gui、Widgets、Network等）
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets SerialPort)

# 创建可执行文件，包含所有手动指定的文件
add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
    ${QRC_FILES}
)

# MSVC 编译选项
if(MSVC)
    # 设置 UTF-8 源字符集和执行字符集，确保中文等 Unicode 字符在 QString 中正确显示
    # 若不设置，MSVC 默认使用本地编码（中文 Windows 为 GBK），而 Qt 5.15+ 的 tr() 默认期望 UTF-8
    add_compile_options(/utf-8)

    # MSVC 多核编译配置：允许用户通过 -DPARALLEL_BUILD_JOBS=<N> 设置 /MP 的参数，默认 4
    # 使用 -DPARALLEL_BUILD_JOBS=0 将传递不带数字的 `/MP`（由编译器选择），使用 -DPARALLEL_BUILD_JOBS=-1 可禁用此行为
    set(PARALLEL_BUILD_JOBS "4" CACHE STRING "Number of parallel compile processes for MSVC (/MP). Set to 0 to pass /MP without number. Set to -1 to disable adding /MP.")
    if(NOT PARALLEL_BUILD_JOBS STREQUAL "-1")
        if(PARALLEL_BUILD_JOBS STREQUAL "0")
            set(MSVC_MP_FLAG "/MP")
        else()
            # 简单拼接数字到 /MP，未经严格验证；非数字会被直接拼接， 用户应传入数字或 0/-1
            set(MSVC_MP_FLAG "/MP${PARALLEL_BUILD_JOBS}")
        endif()
        target_compile_options(${PROJECT_NAME} PRIVATE ${MSVC_MP_FLAG})
        message(STATUS "MSVC multi-processor compilation enabled: ${MSVC_MP_FLAG} (override with -DPARALLEL_BUILD_JOBS)")
    else()
        message(STATUS "MSVC multi-processor compilation disabled (PARALLEL_BUILD_JOBS=-1)")
    endif()
endif()


# 链接Qt库
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::SerialPort
)

# 包含头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE .)

# 在Windows下设置为GUI应用程序（不显示控制台窗口）
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
endif()

# 自动复制 ucrtbased.dll（Debug CRT 调试库）到输出目录
# 该 DLL 位于 Windows Kits 目录中，但默认不在 PATH 中
if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(UCRT_DEBUG_DLL "$ENV{WindowsSdkDir}bin/$ENV{WindowsSDKLibVersion}x64/ucrt/ucrtbased.dll")
    if(NOT EXISTS "${UCRT_DEBUG_DLL}")
        # 备用查找路径
        set(UCRT_DEBUG_DLL "D:/Windows Kits/10/bin/10.0.26100.0/x64/ucrt/ucrtbased.dll")
    endif()
    if(EXISTS "${UCRT_DEBUG_DLL}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${UCRT_DEBUG_DLL}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
            COMMENT "Copying ucrtbased.dll to output directory..."
        )
    else()
        message(WARNING "ucrtbased.dll not found. Please install Windows SDK or copy it manually.")
    endif()
endif()

# 为该目标设置 Debug 版本的后缀为 'd'
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
endif()

# 为该目标设置属性，要求管理员权限
if(WIN32 AND MSVC)
set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
    )
endif()

# Linux 下设置管理员权限（SUID bit）
# 构建后在可执行文件上设置 SUID 位，使其以 root 权限运行
# 注意: 需要 sudo 执行此命令，因为 chown root 需要提权
if(UNIX AND NOT APPLE)
    # 方式一: 通过 install 设置 SUID（推荐，运行 sudo cmake --install . 时生效）
    # 使用 SETUID 后，可执行文件将始终以 root 权限运行（相当于 Windows 的 requireAdministrator）
    install(TARGETS ${PROJECT_NAME} DESTINATION bin
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                        GROUP_READ GROUP_EXECUTE
                        WORLD_READ WORLD_EXECUTE
            SETUID)

    # 方式二: 构建后自动设置 SUID（取消注释启用，需要 sudo 权限）
    # add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    #     COMMAND sudo chown root:root "$<TARGET_FILE:${PROJECT_NAME}>"
    #     COMMAND sudo chmod u+s "$<TARGET_FILE:${PROJECT_NAME}>"
    #     COMMENT "正在设置 SUID 权限，使 ${PROJECT_NAME} 以 root 权限运行..."
    # )

    # 方式三: 使用 pkexec + polkit（会弹出图形化密码框，类似 Windows UAC）
    # 启用此方式需要: 1) 安装 polkit 策略文件  2) 通过 pkexec 启动程序
    # 参看下方 add_custom_target 中的 polkit 相关命令
endif()

if(WIN32)
# Make VS .vcxproj.filters mirror the real directory tree from the repo root
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES 
    ${SRC_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
    ${QRC_FILES})
endif()

if(WIN32)
# 针对不同构建类型设置不同的链接器选项
if(MSVC)
    # Debug模式：链接到控制台子系统
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
    )
    # Release模式：链接到Windows子系统，并确保使用main函数作为入口点[7,9](@ref)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
elseif(MINGW) 
    # 对于MinGW等GCC套件，可以使用-mwindows选项来隐藏控制台[6,10](@ref)
    # 可以类似地通过条件判断只为Release模式添加-mwindows
endif()

endif()

#win ubuntu下都可用
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/models
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/models
    COMMENT "Copying models directory to executable location"
)


#  自动部署Qt依赖库功能
if(WIN32)
    # 获取qmake路径并推导windeployqt路径
    get_target_property(QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)
    
    # 查找windeployqt工具
    find_program(WINDEPLOYQT_EXECUTABLE 
        NAMES windeployqt
        HINTS "${QT_BIN_DIR}"
        DOC "Qt Windows部署工具"
    )
    
    if(WINDEPLOYQT_EXECUTABLE)
        message(STATUS "找到windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
        
        # 添加构建后自动部署命令[1,3](@ref)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E 
                env PATH="${QT_BIN_DIR}" "${WINDEPLOYQT_EXECUTABLE}"
                --verbose 1
                --no-compiler-runtime
                --no-webkit2
                --no-angle
                --no-opengl-sw
                "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMENT "自动部署Qt运行时库..."
        )
        
        message(STATUS "已配置构建后自动运行windeployqt")
    else()
        message(WARNING "未找到windeployqt工具，自动部署功能将不可用")
    endif()
endif()

# ============================================================
# Linux 下 pkexec/Polkit 支持（图形化密码弹窗）
# ============================================================
if(UNIX AND NOT APPLE)
    # 配置 polkit 策略文件路径
    set(POLKIT_ACTION_ID "io.github.openeskin")
    set(POLKIT_POLICY_FILE "${CMAKE_BINARY_DIR}/${POLKIT_ACTION_ID}.policy")
    set(POLKIT_INSTALL_DIR "/usr/share/polkit-1/actions")

    # 生成 polkit 策略文件（用于 pkexec 图形化授权）
    file(WRITE "${POLKIT_POLICY_FILE}" "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<!DOCTYPE policyconfig PUBLIC
 \"-//freedesktop//DTD PolicyKit Policy Configuration 1.0//EN\"
 \"http://www.freedesktop.org/standards/PolicyKit/1/policyconfig.dtd\">
<policyconfig>
  <action id=\"${POLKIT_ACTION_ID}\">
    <description>Run ${PROJECT_NAME} with elevated privileges</description>
    <message>Authentication is required to run ${PROJECT_NAME}</message>
    <icon_name>${PROJECT_NAME}</icon_name>
    <defaults>
      <allow_any>auth_admin</allow_any>
      <allow_inactive>auth_admin</allow_inactive>
      <allow_active>auth_admin_keep</allow_active>
    </defaults>
    <annotate key=\"org.freedesktop.policykit.exec.path\">${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}</annotate>
    <annotate key=\"org.freedesktop.policykit.exec.allow_gui\">true</annotate>
  </action>
</policyconfig>
")

    # 生成 pkexec 启动器脚本（会弹出图形化密码框后启动程序）
    set(LAUNCHER_SCRIPT "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-pkexec.sh")
    file(WRITE "${LAUNCHER_SCRIPT}" "#!/bin/bash
# ${PROJECT_NAME} pkexec launcher
# 使用 pkexec 以 root 权限启动程序，会自动弹出图形化密码输入框
# 用法: 直接运行此脚本，或设置桌面文件 Exec 为此脚本路径
PKEXEC_BIN=\"/usr/bin/pkexec\"
APP_BIN=\"\$(dirname \"\$0\")/${PROJECT_NAME}\"

if [ ! -f \"\${APP_BIN}\" ]; then
    APP_BIN=\"${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}\"
fi

if [ -f \"\${PKEXEC_BIN}\" ]; then
    exec \"\${PKEXEC_BIN}\" \"\${APP_BIN}\" \"\$@\"
else
    # 回退: 直接运行（pkexec 未安装）
    echo \"Warning: pkexec not found, running directly without elevation.\"
    exec \"\${APP_BIN}\" \"\$@\"
fi
")
    # 给启动器脚本添加可执行权限
    file(CHMOD "${LAUNCHER_SCRIPT}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                                              GROUP_READ GROUP_EXECUTE
                                              WORLD_READ WORLD_EXECUTE)

    # 安装 polkit 策略文件（需要 sudo）
    install(FILES "${POLKIT_POLICY_FILE}" DESTINATION "${POLKIT_INSTALL_DIR}")

    # 安装 pkexec 启动器脚本
    install(PROGRAMS "${LAUNCHER_SCRIPT}" DESTINATION bin
            RENAME "${PROJECT_NAME}-pkexec")

    # 添加一个自定义目标，一键完成所有 Linux 权限设置
    add_custom_target(setup-linux-privileges ALL
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "============================================"
        COMMAND ${CMAKE_COMMAND} -E echo "  ${PROJECT_NAME} - Linux Privilege Setup"
        COMMAND ${CMAKE_COMMAND} -E echo "============================================"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "Option 1 - Set SUID bit:"
        COMMAND ${CMAKE_COMMAND} -E echo "  sudo chown root:root ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}"
        COMMAND ${CMAKE_COMMAND} -E echo "  sudo chmod u+s ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "Option 2 - Use pkexec with password dialog:"
        COMMAND ${CMAKE_COMMAND} -E echo "  sudo cp ${POLKIT_POLICY_FILE} ${POLKIT_INSTALL_DIR}/"
        COMMAND ${CMAKE_COMMAND} -E echo "  Then run: ${LAUNCHER_SCRIPT}"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "Option 3 - Run with sudo directly:"
        COMMAND ${CMAKE_COMMAND} -E echo "  sudo ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMENT "显示 Linux 权限设置说明"
    )

    message(STATUS "Linux 权限配置完成")
    message(STATUS "  - Polkit 策略文件: ${POLKIT_POLICY_FILE}")
    message(STATUS "  - pkexec 启动器: ${LAUNCHER_SCRIPT}")
    message(STATUS "")
    message(STATUS "Set SUID bit - no password prompt, runs as root:")
    message(STATUS "  sudo chown root:root ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} && sudo chmod u+s ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}")
    message(STATUS "")
    message(STATUS "Use pkexec - shows password dialog, recommended:")
    message(STATUS "  sudo cp ${POLKIT_POLICY_FILE} ${POLKIT_INSTALL_DIR}/")
    message(STATUS "  ${LAUNCHER_SCRIPT}")
    message(STATUS "")
endif()
