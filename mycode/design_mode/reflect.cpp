#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

// ====================== 第一步：定义反射核心组件 ======================
// 1. 抽象基类：所有需要反射的类都继承此接口（保证统一的对象类型）
class Reflectable {
public:
    virtual ~Reflectable() = default;
    virtual void showInfo() = 0;
};

// 2. 抽象工厂基类：定义创建 Reflectable 对象的接口
class ObjectFactory {
public:
    virtual ~ObjectFactory() = default;
    virtual std::unique_ptr<Reflectable> createObject() = 0;
};

// 3. 全局注册表（单例模式）：维护 类名 → 工厂 的映射
class ClassRegistry {
public:
    // 获取单例实例
    static ClassRegistry& getInstance() {
        static ClassRegistry instance;
        return instance;
    }

    // 注册工厂：将类名和工厂绑定
    void registerClass(const std::string& className, std::unique_ptr<ObjectFactory> factory) {
        factory_map_[className] = std::move(factory);
    }

    // 反射创建对象：根据类名创建实例
    std::unique_ptr<Reflectable> createObject(const std::string& className) {
        auto it = factory_map_.find(className);
        if (it == factory_map_.end()) {
            std::cerr << "类 " << className << " 未注册！" << std::endl;
            return nullptr;
        }
        return it->second->createObject();
    }
private:
    ClassRegistry() = default;
    ClassRegistry(const ClassRegistry&) = delete;
    ClassRegistry& operator=(const ClassRegistry&) = delete;

private:
    // 映射表：key=类名字符串，value=工厂指针
    std::unordered_map<std::string, std::unique_ptr<ObjectFactory>> factory_map_;
};

// 4. 模板工厂类：为任意 Reflectable 子类自动生成工厂（避免重复写工厂）
template <typename T>
class ConcreteFactory : public ObjectFactory {
public:
    static_assert(std::is_base_of<Reflectable, T>::value, "T 必须继承 Reflectable！");

    std::unique_ptr<Reflectable> createObject() override {
        return std::make_unique<T>(); // 创建 T 类型的对象
    }
};

// 5. 注册宏：简化类的注册（核心语法糖，降低使用成本）
#define REGISTER_CLASS(className) \
    class className##Register {\
    public:\
        className##Register() {\
            ClassRegistry::getInstance().registerClass(\
                #className, \
                std::make_unique<ConcreteFactory<className>>()\
            );\
        }\
    };

#define IMPLEMENT_REGISTER_CLASS(className) \
    className##Register g_##className##Register;

// ====================== 第二步：定义业务类并注册 ======================
// 业务基类：Shape（继承 Reflectable）
class Shape : public Reflectable {
public:
    virtual float getArea() = 0;

    void showInfo() override {
        std::cout << "形状类型：" << getClassName() << "，面积：" << getArea() << std::endl;
    }

    virtual std::string getClassName() = 0;
};

// 业务子类1：Circle（圆形）
class Circle : public Shape {
public:
    float getArea() override {
        return 3.14159 * radius_ * radius_;
    }

    std::string getClassName() override {
        return "Circle";
    }

private:
    float radius_ = 5.0f;
};
// 注册 Circle 类（编译时自动注册到注册表）
REGISTER_CLASS(Circle);
IMPLEMENT_REGISTER_CLASS(Circle);

// 业务子类2：Rectangle（矩形）
class Rectangle : public Shape {
public:
    float getArea() override {
        return width_ * height_;
    }

    std::string getClassName() override {
        return "Rectangle";
    }

private:
    float width_ = 4.0f;
    float height_ = 6.0f;
};
// 注册 Rectangle 类
REGISTER_CLASS(Rectangle);
IMPLEMENT_REGISTER_CLASS(Rectangle);

// ====================== 第三步：测试反射功能 ======================
int main() {
    // 1. 反射创建 Circle 对象（仅需类名字符串）
    std::string className1 = "Circle";
    auto circleObj = ClassRegistry::getInstance().createObject(className1);
    if (circleObj) {
        std::cout << "===== 反射创建 " << className1 << " =====" << std::endl;
        circleObj->showInfo();

        Shape* circle = dynamic_cast<Shape*>(circleObj.get());
        if (circle) {
            std::cout << "Circle 面积（直接调用）：" << circle->getArea() << std::endl;
        }
    }

    // 2. 反射创建 Rectangle 对象
    std::string className2 = "Rectangle";
    auto rectObj = ClassRegistry::getInstance().createObject(className2);
    if (rectObj) {
        std::cout << "\n===== 反射创建 " << className2 << " =====" << std::endl;
        rectObj->showInfo();
    }

    // 3. 尝试创建未注册的类（测试错误处理）
    std::string className3 = "Triangle";
    auto triObj = ClassRegistry::getInstance().createObject(className3);
    if (!triObj) {
        std::cout << "\n===== 反射创建 " << className3 << " =====" << std::endl;
        std::cout << "创建失败：" << className3 << " 未注册" << std::endl;
    }

    return 0;
}