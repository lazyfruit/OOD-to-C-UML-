#include<iostream>
#include<thread>
#include<chrono>
#include<Windows.h>
//using namespace std;
using namespace std::chrono_literals;
using std::cin;
using std::cout;
using std::endl;

bool finish = false;
bool flag = true;

void plus()
{
	while (!finish)
	{
		std::cout << "+ ";
		//Sleep(100);
		std::this_thread::sleep_for(10ms);
	}
}
void minus()
{
	while (!finish)
	{
		std::cout << "- ";
		//Sleep(100);
		std::this_thread::sleep_for(10ms);
	}
}

void main()
{
	//plus();
	//minus();

	std::thread plus_thread(plus);
	std::thread minus_thread(minus);

	std::cin.get();//Ожидает нажатие Enter
	finish = true;

	plus_thread.join();
	minus_thread.join();
}