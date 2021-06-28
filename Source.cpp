#include <queue>
#include <mutex>
#include <thread>
#include <memory>
#include <time.h>
#include <ctime>
#include <random>
#include <iostream>
#include <condition_variable>

using namespace std;

mutex m;
condition_variable cv;
bool ready = false;

class Producer
{
private:
	std::shared_ptr<std::queue<int>> ptrQueue;
	std::thread thread_;
public:

	Producer(const std::shared_ptr<std::queue<int>>& q, const int& id);
	~Producer();
	void createNum(const int& id);
};

class Consumer
{
private:
	std::shared_ptr<std::queue<int>> ptrQueue;
	std::thread thread_;
public:
	void removeItem(const int& id);
	Consumer(const std::shared_ptr<std::queue<int>>& q, const int& id);
	~Consumer();
};

Producer::Producer(const shared_ptr<queue<int>>& q, const int& id) {
	thread_ = thread(&Producer::createNum, this, id);
	ptrQueue = q;
}

Producer::~Producer() {
	thread_.join();
}


void Producer::createNum(const int& id) {
	lock_guard<mutex> lock(m);
	random_device rd;
	mt19937 mt(rd());
	uniform_int_distribution<int> distribution(1, 100);
	int num = distribution(mt);
	
	cout << "Producer thread " << id << " produced the int: " << num << endl;
	ptrQueue->push(num);
	ready = true;
	cv.notify_all();
}

Consumer::Consumer(const std::shared_ptr<std::queue<int>>& q, const int& id) {
	thread_ = thread(&Consumer::removeItem, this, id);
	ptrQueue = q;
}

Consumer::~Consumer() {
	thread_.join();
}

void Consumer::removeItem(const int& id) {
	if (ptrQueue->size() > 0) {
		lock_guard<mutex> lock(m);
		cout << "Consumer thread " << id << " removed number " << ptrQueue->front() << " from the queue." << endl;
		ptrQueue->pop();
	}
	else {
		ready = false;
		cout << "Queue is empty!" << endl;
	}

}

int main() {
    shared_ptr<queue<int>> q = make_shared<queue<int>>();
    //For loop to just test the producer threads not working so far
    for (int i = 0; i < 10; i++) {
        Producer p1(q, 1);
        Producer p2(q, 2);
        Consumer c1(q, 1);
        Consumer c2(q, 2);
        Consumer c3(q, 3);
    }
}