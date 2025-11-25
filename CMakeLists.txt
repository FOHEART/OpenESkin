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