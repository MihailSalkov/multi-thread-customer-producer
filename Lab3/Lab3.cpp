#include "pch.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

const int maxSize = 10;

int writeDelay = 100,
	readDelay = 2011;

template<class T>
class QueueItem {
public:
	T value;

	QueueItem *next;

	QueueItem(T value) {
		this->value = value;
		this->next = nullptr;
	}
};

template<class T>
class Queue {
public:
	QueueItem<T> *first, *last;

	int count;

	void push(T i) {
		QueueItem<T> *t = new QueueItem<T>(i);

		if (count++ == 0) {
			first = last = t;
		}
		else {
			last->next = t;
			last = t;
		}
	}

	T pop() {
		T res = first->value;

		first = first->next;

		count--;

		return res;
	}
};

Queue<int> q;
mutex m;
condition_variable cv;

void consumer()
{
	int i;

	while (1)
	{
		this_thread::sleep_for(chrono::milliseconds(readDelay));

		{
			unique_lock<mutex> l(m, defer_lock);

			if (q.count < 1)
			{
				l.lock();
				while (q.count < 1)
					cv.wait(l);
			}

			i = q.pop();

			cv.notify_one();
		}

		cout << "[CONSUMER] " << i << endl;

		QueueItem<int>* t = q.first;
		cout << "{ ";
		while (t != nullptr) {
			cout << t->value << " ";
			t = t->next;
		}
		cout << "}" << endl;
	}
}

void producer()
{
	int i;

	while (1)
	{
		this_thread::sleep_for(chrono::milliseconds(writeDelay));

		i = rand() % 10;

		{
			unique_lock<mutex> l(m, defer_lock);

			if (q.count >= maxSize)
			{
				l.lock();
				while (q.count >= maxSize)
					cv.wait(l);
			}

			q.push(i);

			cv.notify_one();
		}

		cout << "[PRODUCER] " << i << endl;

		QueueItem<int>* t = q.first;
		cout << "{ ";
		while (t != nullptr) {
			cout << t->value << " ";
			t = t->next;
		}
		cout << "}" << endl;
	}
}

int main(int argc, char *argv[])
{
	thread thProducer(producer);
	thread thConsumer(consumer);

	thProducer.join();
	thConsumer.join();

	return 0;
}