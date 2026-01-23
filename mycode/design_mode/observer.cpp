#include <iostream>
#include <string>
#include <vector>
#include <memory>

/*
观察者模式包含 4 个核心角色（C++ 实现中通常通过抽象类定义接口）：
抽象主题（Subject）：定义管理观察者的接口（添加、移除观察者），以及通知观察者的接口。
具体主题（ConcreteSubject）：维护自身状态，状态变化时主动调用通知接口，告知所有注册的观察者。
抽象观察者（Observer）：定义接收通知后 “更新自身” 的统一接口（如 update）。
具体观察者（ConcreteObserver）：实现抽象观察者的更新接口，根据主题的状态变化执行具体逻辑（如刷新显示、处理数据）。
*/

// ====================== 抽象观察者（Observer） ======================
// 定义统一的更新接口，所有观察者都需实现
class WeatherObserver {
public:
    virtual void update(float temperature, float humidity) = 0;
    virtual std::string getObserverName() = 0;
    virtual ~WeatherObserver() = default;
};

// ====================== 抽象主题（Subject） ======================
// 定义管理观察者、通知观察者的接口
class WeatherSubjuct {
public:
    virtual void attach(std::shared_ptr<WeatherObserver> observer) = 0;
    virtual void detach(std::shared_ptr<WeatherObserver> observer) = 0;

    virtual void notify() = 0;
    virtual ~WeatherSubjuct() = default;
};

// ====================== 具体主题（ConcreteSubject）：气象站 ======================
class WeatherStation : public WeatherSubjuct {
public:
    void setWeatherData(float temp, float humi) {
        temperature_ = temp;
        humidity_ = humi;
        std::cout << "\n【气象站】更新天气数据：温度 " << temp << "℃，湿度 " << humi << "%\n" << std::endl;
        notify();
    }

    void attach(std::shared_ptr<WeatherObserver> observer) override {
        observers_.push_back(observer);
        std::cout << "【气象站】添加观察者：" << observer->getObserverName() << std::endl;
    }

    void detach(std::shared_ptr<WeatherObserver> observer) override {
        for (auto it = observers_.begin(); it != observers_.end(); it++) {
            if (it->get() == observer.get()) {
                std::cout << "【气象站】移除观察者：" << observer->getObserverName() << std::endl;
                observers_.erase(it);
                break;
            }
        }
    }

    void notify() override {
        for (auto& observer : observers_) {
            observer->update(temperature_, humidity_);
        }
    }

    std::string getSubjuctName() {
        return "气象站";
    }

private:
    float temperature_;
    float humidity_;

    std::vector<std::shared_ptr<WeatherObserver>> observers_;
};

// ====================== 具体观察者（ConcreteObserver） ======================
// 具体观察者1：手机APP
class PhoneDisplay : public WeatherObserver {
public:
    PhoneDisplay(const std::string& name) : name_(name) {}

    void update(float temperature, float humidity) override {
        std::cout << "【手机APP " << name_ << "】实时天气：温度 " << temperature << "℃，湿度 " << humidity << "%" << std::endl;
    }

    std::string getObserverName() override {
        return "手机APP" + name_;
    }

private:
    std::string name_;
};

// 具体观察者2：网页端
class WebDisplay : public WeatherObserver {
public:
    WebDisplay(const std::string& name) : name_(name) {}

    void update(float temperature, float humidity) override {
        std::cout << "【网页端 " << name_ << "】实时天气：温度 " << temperature << "℃，湿度 " << humidity << "%" << std::endl;
    }

    std::string getObserverName() override {
        return "网页端" + name_;
    }

private:
    std::string name_;
};

// 具体观察者3：大屏终端
class ScreenDisplay : public WeatherObserver {
public:
    ScreenDisplay(const std::string& name) : name_(name) {}

    void update(float temperature, float humidity) override {
        std::cout << "【大屏 " << name_ << "】实时天气：温度 " << temperature << "℃，湿度 " << humidity << "%" << std::endl;
    }

    std::string getObserverName() override {
        return "网页端" + name_;
    }

private:
    std::string name_;
};

// ====================== 客户端 ======================
int main() {
    // 1. 创建具体主题（气象站）
    std::shared_ptr<WeatherStation> weather_station = std::make_shared<WeatherStation>();

    // 2. 创建具体观察者
    auto phone_observer = std::make_shared<PhoneDisplay>("小米手机");
    auto web_observer = std::make_shared<WebDisplay>("百度天气");
    auto screen_observer = std::make_shared<ScreenDisplay>("商场大屏");

    // 3. 注册观察者到气象站
    weather_station->attach(phone_observer);
    weather_station->attach(web_observer);
    weather_station->attach(screen_observer);

    // 4. 气象站更新天气数据（触发通知）
    weather_station->setWeatherData(25.5f, 60.0f);

    // 5. 移除一个观察者（网页端）
    weather_station->detach(web_observer);

    // 6. 再次更新天气数据（仅剩余观察者收到通知）
    weather_station->setWeatherData(26.8f, 55.0f);

    return 0;
}