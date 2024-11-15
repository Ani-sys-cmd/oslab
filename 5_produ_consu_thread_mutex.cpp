#include <bits/stdc++.h>
using namespace std;

const int MAX_QUEUE_SIZE = 5;
queue<int> buffer;
mutex mtx;
condition_variable cond_empty; // Condition variable for empty buffer
condition_variable cond_full;  // Condition variable for full buffer

void producer(int id) {
    for (int i = 0; i < 10; ++i) {
        this_thread::sleep_for(chrono::milliseconds(100)); // Simulate work
        unique_lock<mutex> lock(mtx);
        
        cond_empty.wait(lock, [] { return buffer.size() < MAX_QUEUE_SIZE; }); // Wait if buffer is full
        
        cout << "Producer " << id << " is entering critical section." << endl;
        buffer.push(i);
        cout << "Producer " << id << " produced: " << i << endl;
        cout << "Producer " << id << " has exited critical section." << endl;
        
        lock.unlock();
        cond_full.notify_one(); // Notify one consumer
    }
}

void consumer(int id) {
    for (int i = 0; i < 10; ++i) {
        this_thread::sleep_for(chrono::milliseconds(150)); // Simulate work
        unique_lock<mutex> lock(mtx);
        
        cond_full.wait(lock, [] { return !buffer.empty(); }); // Wait if buffer is empty
        
        cout << "Consumer " << id << " is entering critical section." << endl;
        int item = buffer.front();
        buffer.pop();
        cout << "Consumer " << id << " consumed: " << item << endl;
        cout << "Consumer " << id << " has exited critical section." << endl;
        
        lock.unlock();
        cond_empty.notify_one(); // Notify one producer
    }
}

int main() {
    thread producers[2], consumers[2];
    for (int i = 0; i < 2; ++i) {
        producers[i] = thread(producer, i);
        consumers[i] = thread(consumer, i);
    }

    for (int i = 0; i < 2; ++i) {
        producers[i].join();
        consumers[i].join();
    }

    return 0;
}