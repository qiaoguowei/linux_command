#include <iostream>
#include <string>
#include <memory>

/*
以企业「请假审批」为例：
请假 ≤ 1 天：小组长审批；
1 天 < 请假 ≤ 3 天：部门经理审批；
请假 > 3 天：总经理审批；
责任链：小组长 → 部门经理 → 总经理。
*/

class Approver {
public:
    Approver(const std::string& name)
        : next_approver_(nullptr)
        , name_(name)
    {}

    void setNextApporver(std::unique_ptr<Approver> next) {
        next_approver_ = std::move(next);
    }

    virtual void handleRequest(int leave_day) = 0;

    virtual ~Approver() = default;

protected:
    std::unique_ptr<Approver> next_approver_;
    std::string name_;
};

class GroupLeader : public Approver {
public:
    GroupLeader(const std::string& name) : Approver(name) {}

    void handleRequest(int leave_days) override {
        if (leave_days <= 1) {
            std::cout << "【小组长 " << name_ << "】审批：同意请假 " << leave_days << " 天" << std::endl;
        } else if (next_approver_) {
            std::cout << "【小组长 " << name_ << "】：请假 " << leave_days << " 天超出权限，转交给下一级审批" << std::endl;
            next_approver_->handleRequest(leave_days);
        } else {
            std::cout << "【小组长 " << name_ << "】：无后续审批者，请假 " << leave_days << " 天审批失败" << std::endl;
        }
    }
};

class DepartmentManager : public Approver {
public:
    DepartmentManager(const std::string& name) : Approver(name) {}

    void handleRequest(int leave_days) override {
        if (leave_days > 1 && leave_days <= 3) {
            std::cout << "【部门经理 " << name_ << "】审批：同意请假 " << leave_days << " 天" << std::endl;
        } else if (next_approver_) {
            std::cout << "【部门经理 " << name_ << "】：请假 " << leave_days << " 天超出权限，转交给下一级审批" << std::endl;
            next_approver_->handleRequest(leave_days);
        } else {
            std::cout << "【部门经理 " << name_ << "】：无后续审批者，请假 " << leave_days << " 天审批失败" << std::endl;
        }
    }
};

class GeneralManager : public Approver {
public:
    GeneralManager(const std::string& name) : Approver(name) {}

    void handleRequest(int leave_days) override {
        if (leave_days > 3) {
            std::cout << "【总经理 " << name_ << "】审批：同意请假 " << leave_days << " 天" << std::endl;
        } else if (next_approver_) {
            std::cout << "【总经理 " << name_ << "】：请假 " << leave_days << " 天无需我审批，转交给下一级" << std::endl;
            next_approver_->handleRequest(leave_days);
        } else {
            std::cout << "【总经理 " << name_ << "】：请假 " << leave_days << " 天不符合规则，审批失败" << std::endl;
        }
    }
};

int main() {
    // 构建责任链 + 发起请求
    std::unique_ptr<Approver> group_leader = std::make_unique<GroupLeader>("zhangsan");
    std::unique_ptr<Approver> dept_manager = std::make_unique<DepartmentManager>("lisi");
    std::unique_ptr<Approver> general_manager = std::make_unique<GeneralManager>("wangwu");

    dept_manager->setNextApporver(std::move(general_manager));
    group_leader->setNextApporver(std::move(dept_manager));

    std::cout << "--------------请假3天-----------------------" << std::endl;
    group_leader->handleRequest(3);
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "--------------请假5天-----------------------" << std::endl;
    group_leader->handleRequest(5);
    std::cout << "--------------------------------------------" << std::endl;

    return 0;
}
