#include <coroutine>
#include <future>
#include <iostream>
#include <string>
#include <thread>

namespace {

struct TaskPromise {
  std::promise<std::string> p;
  std::future<std::string> get_return_object() { return p.get_future(); }
  std::suspend_never initial_suspend() { return {}; }
  std::suspend_never final_suspend() noexcept { return {}; }
  void return_value(std::string value) { p.set_value(value); }
  void unhandled_exception() { p.set_exception(std::current_exception()); }
};

struct Task {
  using promise_type = TaskPromise;
  std::future<std::string> fut;

  Task(std::future<std::string> f) : fut(std::move(f)) {}

  std::string wait() { return fut.get(); }
};

Task generatePhrase(const std::string_view name) {
  std::cout << "\ngeneratePhrase: stated in thread "
            << std::this_thread::get_id() << std::endl;

  std::string result = "Hello, ";
  result += name;
  result += "!";
  co_return result;
}

}  // namespace

void execute() {
  std::cout << "Main: stated in thread " << std::this_thread::get_id()
            << std::endl;

  Task task = generatePhrase("Eugene");

  std::cout << "\nWaiting for task in thread " << std::this_thread::get_id()
            << std::endl;

  std::cout << task.wait() << std::endl;

  std::cout << "\nFinished for task in thread " << std::this_thread::get_id()
            << std::endl;
}
