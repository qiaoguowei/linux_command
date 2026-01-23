#include <iostream>
#include <string>
#include <memory>

class ServerInterface {
public:
    virtual void accessResource(const std::string& user) = 0;

    virtual ~ServerInterface() = default;
};

class RealServer : public ServerInterface {
public:
    void accessResource(const std::string& user) override {
        std::cout << "[RealServer] 用户 " << user << " 访问核心资源成功！" << std::endl;
    }
};

class ServerProxy : public ServerInterface {
public:
    ServerProxy() : real_server_(std::make_unique<RealServer>()) {}

    ~ServerProxy() override {}

    void accessResource(const std::string& user) {
        bool has_permission = checkPermission(user);
        if (!has_permission) {
            logAccess(user, false);
            return;
        }

        real_server_->accessResource(user);

        logAccess(user, true);
    }

private:
    bool checkPermission(const std::string& user) {
        std::cout << "[ServerProxy] 校验用户 " << user << " 的权限..." << std::endl;
        return user == "admin";
    }

    void logAccess(const std::string& user, bool success) {
        if (success) {
            std::cout << "[ServerProxy] 日志：用户 " << user << " 访问资源成功" << std::endl;
        } else {
            std::cout << "[ServerProxy] 日志：用户 " << user << " 权限不足，访问被拒绝" << std::endl;
        }
    }

private:
    std::unique_ptr<RealServer> real_server_;
};

int main() {
    std::unique_ptr<ServerInterface> proxy = std::make_unique<ServerProxy>();

    std::cout << "===== 测试1：普通用户访问 =====" << std::endl;
    proxy->accessResource("guest");

    std::cout << "\n===== 测试2：管理员访问 =====" << std::endl;
    proxy->accessResource("admin");

    return 0;
}



