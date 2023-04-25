#include "singleton.h"



static SingletonManager* global_singleton_manager = nullptr;

void SingletonManager::ProcessCallbackNow() {
  if (!global_singleton_manager) {
    return;
  }

  std::stack<std::function<void()>> tasks;

  {
    std::lock_guard<std::mutex> lock(global_singleton_manager->mutex_);
    tasks.swap(global_singleton_manager->stack_);
  }

  while (!tasks.empty()) {
    std::move(tasks.top())();
    tasks.pop();
  }
}

void SingletonManager::Initialize() {
  static SingletonManager manager;
  global_singleton_manager = &manager;
}

void SingletonManager::Uninitialize() {
  if (!global_singleton_manager) {
    return;
  }

  ProcessCallbackNow();
  global_singleton_manager = nullptr;
}

void SingletonManager::RegisterCallback(void(*func)()) {
  if (!func) {
    return;
  }

  RegisterTask(std::bind(func));
}

void SingletonManager::RegisterTask(std::function<void()> task) {
  if (!global_singleton_manager) {
    return;
  }

  std::lock_guard<std::mutex> lock(global_singleton_manager->mutex_);
  global_singleton_manager->stack_.push(std::move(task));
}