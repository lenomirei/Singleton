#include <iostream>
#include "singleton.h"


class TestSingleton
{
public:
  static TestSingleton* GetInstance() {
    return Singleton<TestSingleton>::get();
  }

  friend struct DefaultSingletonTraits<TestSingleton>;
};

int main() {
  SingletonManager::Initialize();


  auto test = TestSingleton::GetInstance();

  SingletonManager::Uninitialize();

  test = TestSingleton::GetInstance();
  return 0;
}