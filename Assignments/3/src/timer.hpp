#include <chrono>

class Timer {
public:
    Timer() {
        start = std::chrono::high_resolution_clock::now();
    }

    void reset() {
        start = std::chrono::high_resolution_clock::now();
    }

    float elapsed() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};