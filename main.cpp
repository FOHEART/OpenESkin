#include "ui/MainWindow.h"
#include "inc/appinit.h"
#include <QApplication>
#include <QFile>
#include <QFont>
#include <QtGlobal>
#include <QSurfaceFormat>

#ifdef Q_OS_LINUX
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif

// Linux 下自动提权: 如果不是 root 用户运行，则通过 pkexec 重新启动（会弹出图形化密码框）
// 设置环境变量 OPENESKIN_NO_ELEVATE=1 可跳过提权，以普通用户身份运行
#ifdef Q_OS_LINUX
static bool elevatePrivileges(int &argc, char* argv[])
{
    // 已经是 root 用户，无需提权
    if (geteuid() == 0)
        return true;

    // 允许通过环境变量跳过提权
    if (qEnvironmentVariableIsSet("OPENESKIN_NO_ELEVATE"))
        return false;

    // 检查 DISPLAY 或 WAYLAND_DISPLAY 是否可用，避免提权后无法连接显示
    if (!qEnvironmentVariableIsSet("DISPLAY") && !qEnvironmentVariableIsSet("WAYLAND_DISPLAY"))
        return false;

    // 检查 pkexec 是否可用
    if (access("/usr/bin/pkexec", X_OK) != 0)
        return false;

    // 构建 pkexec 命令: pkexec /path/to/exe [args...]
    // 不使用 --disable-internal-agent: 在终端中运行时 pkexec 会用内置代理提示输入密码，
    // 在没有图形化 polkit 代理的环境（如 VS Code 终端）中也能正常认证。
    // 在桌面环境中双击运行时，系统图形化代理也会自动接管。
    QByteArray appPath = QByteArray(argv[0]);
    QByteArrayList args;
    args << "pkexec" << appPath;
    for (int i = 1; i < argc; ++i)
        args << QByteArray(argv[i]);

    // 转换为 execvp 格式
    QList<QByteArray> argvStorage = args;
    QVector<char*> execArgv;
    for (auto &arg : argvStorage)
        execArgv.append(arg.data());
    execArgv.append(nullptr);

    // 先用 fork+exec 尝试 pkexec 提权
    // 如果提权后的进程启动失败（如无法连接显示），则回退到普通用户运行
    pid_t pid = fork();
    if (pid == 0)
    {
        // 子进程: 将 stderr 重定向到 /dev/null，避免提权失败时的错误信息干扰用户
        int devNull = open("/dev/null", O_WRONLY);
        if (devNull != -1)
            dup2(devNull, STDERR_FILENO);
        // 通过 pkexec 以 root 身份启动应用
        execvp("pkexec", execArgv.data());
        // execvp 失败则退出
        _exit(127);
    }
    else if (pid > 0)
    {
        // 父进程: 等待子进程执行
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
        {
            int exitCode = WEXITSTATUS(status);
            // pkexec 成功运行了应用（应用正常退出或被窗口关闭）
            if (exitCode == 0)
                _exit(0);
            // 常见的失败原因:
            //   127 - pkexec 未找到或 exec 失败
            //   134 - 应用崩溃（如无法连接显示，SIGABRT）
            //   126 - pkexec 认证失败
            // 这些情况下回退到普通用户运行
        }

        // pkexec 启动失败，回退到普通用户权限
        return false;
    }

    // fork 失败，直接以当前权限运行
    return false;
}
#endif

void initHighDpi()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
	QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
}

/*
UI工程使用的是这里的：
https://blog.csdn.net/hwc3737/article/details/108377607
*/
int main(int argc, char* argv[])
{
#ifdef Q_OS_LINUX
	// Linux 下自动提权: 如非 root 则通过 pkexec 重新启动
	if (!elevatePrivileges(argc, argv))
	{
		// 提权失败，显示警告但继续运行（可能有 SUID 或用户只需普通权限）
		// 静默处理，在真正需要 root 权限的操作时会出错
	}
#endif

	initHighDpi();

	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":logo.ico"));
	//加载样式表
	QFile file(":/css/index.css");
	if (file.open(QFile::ReadOnly))
	{
		QString qss = QLatin1String(file.readAll());
		qApp->setStyleSheet(qss);
		file.close();
	}

	//全局字体
#ifdef Q_OS_WIN
	QFont font("Microsoft YaHei", 10);
	font.setStyleStrategy(QFont::NoSubpixelAntialias);
	a.setFont(font);
#elif defined(Q_OS_LINUX)
	QFont font("Noto Sans SC", 10);
	font.setStyleStrategy(QFont::NoSubpixelAntialias);
	a.setFont(font);
#elif defined(Q_OS_MAC)
	
#endif
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	QSurfaceFormat::setDefaultFormat(format);


	//QSurfaceFormat format;
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

	//屏幕拖动
	AppInit::Instance()->start();

	MainWindow w;
	w.show();

	return a.exec();
}
