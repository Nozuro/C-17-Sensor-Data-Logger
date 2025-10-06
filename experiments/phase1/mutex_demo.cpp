#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

//class mutex_demo {
//public:
//	void run();
//
//private:
//	vector<int> readings;
//	mutex m;
//	void task(const string& name) {
//		for (int i = 0; i < 5; i++) {
//			lock_guard<mutex> lock(m);
//			readings.push_back(i);
//			cout << name << " pushed " << i << "/n";
//		}
//	}
//};

vector<int> readings;
mutex m;

void task(const string& name) {
	for (int i = 0; i < 5;i++) {
		lock_guard<mutex> lock(m);
		readings.push_back(i);
		cout << name << " pushed " << i << "\n";
	}
}

void run_mutex_demo() {
	//thread t1(&mutex_demo::task, "A");
	//thread t2(&mutex_demo::task, "B");
	thread t1(task, "A");
	thread t2(task, "B");
	t1.join();
	t2.join();
	cout << "Total readings: " << readings.size() << "\n";
}