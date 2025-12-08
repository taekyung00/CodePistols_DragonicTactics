#include <iostream>
#include <string>
#include <memory> // std::unique_ptr를 사용하기 위함

// --- 1. Product (제품): 생성될 객체들의 공통 인터페이스 ---
// 모든 커피 종류가 가져야 할 기본 기능을 정의합니다.
class Coffee {
public:
    virtual ~Coffee() = default; // 가상 소멸자
    virtual std::string getName() = 0; // 순수 가상 함수
};

// --- 2. ConcreteProduct (구체적인 제품): 실제 생성될 클래스 ---
// 'Product' 인터페이스를 실제로 구현합니다.
class Latte : public Coffee {
public:
    std::string getName() override {
        return "부드러운 라떼";
    }
};

class Americano : public Coffee {
public:
    std::string getName() override {
        return "진한 아메리카노";
    }
};

// --- 3. Creator (생성자): 팩토리 메서드를 선언하는 클래스 ---
// 커피를 만드는 '공장'의 기본 틀입니다.
// ✨ '팩토리 메서드(createCoffee)'를 통해 객체를 생성합니다.
// 어떤 커피가 생성될지는 서브클래스에서 결정됩니다.
class CoffeeFactory {
public:
    virtual ~CoffeeFactory() = default;

    // ✨ 팩토리 메서드: 서브클래스가 오버라이드하여 구체적인 제품을 생성합니다.
    virtual std::unique_ptr<Coffee> createCoffee() = 0;

    // 팩토리 메서드를 사용하여 제품을 주문하고 사용하는 로직
    void orderCoffee() {
        std::unique_ptr<Coffee> coffee = createCoffee();
        std::cout << "주문하신 '" << coffee->getName() << "'가 나왔습니다!" << std::endl;
    }
};

// --- 4. ConcreteCreator (구체적인 생성자): 팩토리 메서드를 구현하는 클래스 ---
// 'Creator'를 상속받아 실제로 어떤 제품을 만들지 결정하고 구현합니다.
class LatteFactory : public CoffeeFactory {
public:
    // 라떼를 생성하는 팩토리 메서드를 실제로 구현합니다.
    std::unique_ptr<Coffee> createCoffee() override {
        return std::make_unique<Latte>();
    }
};

class AmericanoFactory : public CoffeeFactory {
public:
    // 아메리카노를 생성하는 팩토리 메서드를 실제로 구현합니다.
    std::unique_ptr<Coffee> createCoffee() override {
        return std::make_unique<Americano>();
    }
};


// --- Client (클라이언트 코드) ---
int main() {
    // 클라이언트는 구체적인 커피 클래스(Latte, Americano)를 직접 알 필요가 없습니다.
    // 오직 필요한 팩토리(LatteFactory, AmericanoFactory)를 선택하기만 하면 됩니다.

    std::cout << "라떼 공장에 주문합니다." << std::endl;
    auto latteFactory = std::make_unique<LatteFactory>();
    latteFactory->orderCoffee(); // "주문하신 '부드러운 라떼'가 나왔습니다!" 출력

    std::cout << "\n아메리카노 공장에 주문합니다." << std::endl;
    auto americanoFactory = std::make_unique<AmericanoFactory>();
    americanoFactory->orderCoffee(); // "주문하신 '진한 아메리카노'가 나왔습니다!" 출력

    return 0;
}