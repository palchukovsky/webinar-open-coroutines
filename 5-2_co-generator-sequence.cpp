#include <coroutine>
#include <exception>
#include <iostream>
#include <string>

namespace {

struct Generator {
  struct promise_type;
  using handle_type = std::coroutine_handle<promise_type>;

  struct promise_type {
    std::string m_value;
    std::exception_ptr m_exception;

    Generator get_return_object() {
      return Generator(handle_type::from_promise(*this));
    }

    std::suspend_always initial_suspend() { return {}; }

    std::suspend_always final_suspend() noexcept { return {}; }

    void unhandled_exception() { m_exception = std::current_exception(); }

    template <std::convertible_to<std::string> From>
    std::suspend_always yield_value(From&& from) {
      m_value = std::forward<From>(from);
      return {};
    }

    void return_void() {}
  };

  handle_type m_handle;

  Generator(handle_type handle) : m_handle(handle) {}

  ~Generator() { m_handle.destroy(); }

  explicit operator bool() {
    fill();
    return !m_handle.done();
  }

  std::string operator()() {
    fill();
    m_full = false;
    return std::move(m_handle.promise().m_value);
  }

 private:
  void fill() {
    if (!m_full) {
      m_handle();
      if (m_handle.promise().m_exception) {
        std::rethrow_exception(m_handle.promise().m_exception);
      }

      m_full = true;
    }
  }

  bool m_full = false;
};

Generator phraseSequence() {
    std::cout << "phraseSequence: started in thread "
            << std::this_thread::get_id() << std::endl;

  co_yield "What";

  std::cout << "phraseSequence: co_yield \"What\" in thread "
            << std::this_thread::get_id() << std::endl;

  co_yield "did";

  std::cout << "phraseSequence: co_yield \"did\" in thread "
            << std::this_thread::get_id() << std::endl;

  co_yield "I";

  std::cout << "phraseSequence: co_yield \"I\" in thread "
            << std::this_thread::get_id() << std::endl;

  co_yield "see";

  std::cout << "phraseSequence: co_yield \"see\" in thread "
            << std::this_thread::get_id() << std::endl;

  co_yield "now?";

  std::cout << "phraseSequence: co_yield \"now?\" in thread "
            << std::this_thread::get_id() << std::endl;

  co_return;
}

}  // namespace

void execute() {
  auto gen = phraseSequence();

  std::cout << "\nStated in thread " << std::this_thread::get_id() << std::endl;

  std::cout << gen();

  std::cout << "\nGen 1 in thread " << std::this_thread::get_id() << std::endl;

  std::cout << gen();

  std::cout << "\nGen 2 in thread " << std::this_thread::get_id() << std::endl;

  std::cout << gen();

  std::cout << "\nGen 3 in thread " << std::this_thread::get_id() << std::endl;

  std::cout << gen();

  std::cout << "\nGen 4 in thread " << std::this_thread::get_id() << std::endl;

  std::cout << gen();

  std::cout << "\nGen 5 in thread " << std::this_thread::get_id() << std::endl;

  std::cout << gen();  //! ?????

  std::cout << "\nGen 6 in thread " << std::this_thread::get_id() << std::endl;
}
