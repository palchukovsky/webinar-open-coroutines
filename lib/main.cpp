#include <exception>
#include <iostream>

void execute();

int main() {
  try {
    execute();
  } catch (const std::exception &ex) {
    std::cerr << "Unexpected error:" << ex.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Unexpected UNKNOWN error!" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}