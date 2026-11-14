#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include <mutex>

using namespace std;

mutex cout_mutex; // М'ютекс для захисту від гонок даних у консолі

void compute(const string& name, int seconds) {
    this_thread::sleep_for(chrono::seconds(seconds));
    
    // Безпечний вивід замість osyncstream
    lock_guard<mutex> lock(cout_mutex);
    cout << "Finished: " << name << " (" << (seconds == 7 ? "slow" : "quick") << ", " << seconds << "s)\n";
}

void work() {
    auto start_time = chrono::steady_clock::now();

    auto branch2_future = async(launch::async, []() {
        auto f_a3 = async(launch::async, []() { compute("A3", 7); });
        compute("A4", 1);
        
        f_a3.get();
        compute("C", 1);
    });

    auto f_a2 = async(launch::async, []() { compute("A2", 7); });
    compute("A1", 1);

    f_a2.get();
    compute("B", 1);
    compute("D", 1);

    branch2_future.get();

    auto end_time = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();

    lock_guard<mutex> lock(cout_mutex);
    cout << "\nWork is done! Total execution time: " << duration << " seconds.\n";
}

int main() {
    ios_base::sync_with_stdio(false);
    work();
    return 0;
}