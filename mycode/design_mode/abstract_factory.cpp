#include <iostream>
#include <string>
#include <memory>

/*
以「跨平台 UI 组件」为例：
产品族：Windows 风格组件（Windows 按钮 + Windows 文本框）、Mac 风格组件（Mac 按钮 + Mac 文本框）；
抽象工厂：定义「创建按钮」「创建文本框」的接口；
具体工厂：Windows 工厂、Mac 工厂，分别创建对应风格的组件；
客户端：通过抽象工厂创建组件，无需关心是 Windows 还是 Mac 风格。
*/

// ====================== 抽象产品 ======================
// 抽象产品1：按钮
class Button {
public:
    virtual void render() = 0;
    virtual ~Button() = default;
};

// 抽象产品2：文本框
class TextBox {
public:
    virtual void render() = 0;
    virtual ~TextBox() = default;
};

// ====================== 具体产品（Windows 产品族） ======================
// 具体产品1：Windows 按钮
class WindowsButton : public Button {
public:
    void render() override {
        std::cout << "[Windows Button] 渲染灰色边框、矩形按钮" << std::endl;
    }
};

// 具体产品2：Windows 文本框
class WindowsTextBox : public TextBox {
public:
    void render() override {
        std::cout << "[Windows TextBox] 渲染白色背景、黑色文字的文本框" << std::endl;
    }
};

// ====================== 具体产品（Mac 产品族） ======================
// 具体产品1：Mac 按钮
class MacButton : public Button {
public:
    void render() override {
        std::cout << "[Mac Button] 渲染圆角、磨砂质感按钮" << std::endl;
    }
};

class MacTextBox : public TextBox {
public:
    void render() override {
        std::cout << "[Mac TextBox] 渲染无边框、浅灰色背景的文本框" << std::endl;
    }
};

// ====================== 抽象工厂 ======================
class UIFactory {
public:
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<TextBox> createTextBox() = 0;
    virtual ~UIFactory() = default;
};

// ====================== 具体工厂 ======================
// 具体工厂1：Windows UI 工厂（创建 Windows 产品族）
class WindowsUIFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }

    std::unique_ptr<TextBox> createTextBox() override {
        return std::make_unique<WindowsTextBox>();
    }
};

// 具体工厂2：Mac UI 工厂（创建 Mac 产品族）
class MacUIFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<MacButton>();
    }

    std::unique_ptr<TextBox> createTextBox() override {
        return std::make_unique<MacTextBox>();
    }
};

// ====================== 客户端 ======================
// 客户端只依赖抽象工厂和抽象产品，无需知道具体平台
void renderUI(std::unique_ptr<UIFactory> factory) {
    std::unique_ptr<Button> button = factory->createButton();
    std::unique_ptr<TextBox> textbox = factory->createTextBox();

    std::cout << "===== 渲染 UI 组件 =====" << std::endl;
    button->render();
    textbox->render();
}

int main() {
    // 场景1：渲染 Windows 风格 UI
    std::cout << "--- Windows 平台 UI ---" << std::endl;
    std::unique_ptr<UIFactory> windows_factory = std::make_unique<WindowsUIFactory>();
    renderUI(std::move(windows_factory));

    // 场景2：渲染 Mac 风格 UI（仅切换工厂，客户端逻辑无需修改）
    std::cout << "\n--- Mac 平台 UI ---" << std::endl;
    std::unique_ptr<UIFactory> mac_factory = std::make_unique<MacUIFactory>();
    renderUI(std::move(mac_factory));

    return 0;
}