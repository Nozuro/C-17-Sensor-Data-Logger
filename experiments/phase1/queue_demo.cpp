#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

queue<int> q;
mutex m;
condition_variable cv;
bool done = false;

void producer() {
	for (int i = 0; i < 10; i++) {
		sleep_for(milliseconds(100));
		{
			lock_guard<mutex> lock(m);
			q.push(i);
			cout << "Produced " << i << "\n";
		}
		cv.notify_one();
	}
	{
		lock_guard<mutex> lock(m);
		done = true;
	}
	cv.notify_one();
}

void consumer() {
	while (true) {
		unique_lock<mutex> lock(m);
		cv.wait(lock, [] { return !q.empty() || done; });
		if (!q.empty()) {
			int val = q.front(); q.pop();
			lock.unlock();
			cout << "Consumend " << val << "\n";
		}
		else if (done)
			break;
	}
}

void run_queue_demo() {
	thread t1(producer);
	thread t2(consumer);
	t1.join();
	t2.join();
}
