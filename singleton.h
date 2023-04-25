#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <stack>

template<typename Type>
struct DefaultSingletonTraits {
  static Type* New() {
    return new Type();
  }

  static void Delete(Type* x) {
    delete x;
  }
};

class SingletonManager {
public:
  static void Initialize();

  static void Uninitialize();

  static void ProcessCallbackNow();

  static void RegisterCallback(void (*destructor)());

  static void RegisterTask(std::function<void()> task);

private:

  std::mutex mutex_;
  std::stack<std::function<void()>> stack_;
};

template<typename Type, typename Traits = DefaultSingletonTraits<Type>>
class Singleton
{
private:
  friend Type;
  static Type* get() {
    uintptr_t instance = instance_.load(std::memory_order_relaxed);
    if (!instance && !destroyed_.load(std::memory_order_relaxed)) {
      instance = reinterpret_cast<uintptr_t>(Traits::New());
      instance_.store(instance, std::memory_order_relaxed);
      SingletonManager::RegisterCallback(OnDestroyed);

    }
    return reinterpret_cast<Type*>(instance);
  }

  static Type* GetIfExists() {
    if (!instance_.load(std::memory_order_relaxed)) {
      return nullptr;
    }

    return get();
  }

  void static OnDestroyed() {
    destroyed_.store(true, std::memory_order_relaxed);
    Traits::Delete(reinterpret_cast<Type*>(instance_.load(), std::memory_order_relaxed));
    instance_.store(0, std::memory_order_relaxed);
  }
  
  static std::atomic<uintptr_t> instance_;
  static std::atomic<bool> destroyed_;
};

template<typename Type, typename Traits>
std::atomic<uintptr_t> Singleton<Type, Traits>::instance_ = 0;

template<typename Type, typename Traits>
std::atomic<bool> Singleton<Type, Traits>::destroyed_ = false;

