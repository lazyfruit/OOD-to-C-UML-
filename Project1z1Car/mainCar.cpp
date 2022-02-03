﻿#include<iostream>
#include<conio.h>
#include<Windows.h>
#include<chrono>
#include<thread>
using namespace std;
using std::cin;
using std::cout;
using std::endl;

#define MIN_TANK_VOLUME 40
#define MAX_TANK_VOLUME 80
#define MIN_ENGINE_CONSUMPTION 4
#define MAX_ENGINE_CONSUMPTION 25

class Tank
{
	const unsigned int VOLUME;
	double fuel_level;
public:
	const unsigned int get_volume()const
	{
		return VOLUME;
	}
	double get_fuel_level()const
	{
		return fuel_level;
	}
	double fill(double fuel)
	{
		if (fuel_level + fuel < 0)return fuel_level = 0;
		if (fuel_level + fuel > VOLUME)fuel_level = VOLUME;//Даже если мы зальем в бак 200 литров топлива,\
														   в нем все равно будет 40 литров, а все остальное выльется наружу
		//if (fuel_level + fuel >=0 && fuel_level + fuel <= VOLUME) fuel_level += fuel;
		else return fuel_level += fuel;
	}
	double give_fuel(double fuel)
	{
		fuel_level -= fuel;
		if (fuel_level < 0) fuel_level = 0;
		return fuel_level;
	}
	Tank(unsigned int volume):VOLUME(volume>=MIN_TANK_VOLUME&&volume<=MAX_TANK_VOLUME?volume:MAX_TANK_VOLUME),
		fuel_level(0)
	{
		//this->VOLUME = volume;
		cout << "Tank is ready\t" << this << endl;
	}
	~Tank()
	{
		cout << "Tank is gone\t" << this << endl;
	}
	void info()const
	{
		cout << "Tank volume:\t" << VOLUME << endl;
		cout << "Fuel level:\t" << fuel_level << endl;
	}
};

class Engine
{
	double consumption;
	double consumption_per_second;
	bool is_started;
public:
	double get_consumption()const
	{
		return consumption;
	}
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()const
	{
		return is_started;
	}
	void set_consumption(double consumption)
	{
		if (consumption >= MIN_ENGINE_CONSUMPTION && consumption <= MAX_ENGINE_CONSUMPTION)
			this->consumption = consumption;
		else this->consumption = MAX_ENGINE_CONSUMPTION / 2;
		consumption_per_second = this->consumption * .3e-4;
	}
	Engine(double consumption)
	{
		set_consumption(consumption);
		is_started = false;
		cout << "Engine is ready:\t" << this << endl;
	}
	~Engine()
	{
		cout << "Engine is gone:\t" << this << endl;
	}
	void info()const
	{
		cout << "Consumtion:\t" << consumption << endl;
		cout << "Consumtion per second:\t" << consumption_per_second << endl;
		cout << "Engine is " << (is_started?"started":"stop") << endl;
	}
};

#define Enter 13
#define Escape 27

#define MAX_SPEED_LOW 120
#define MAX_SPEED_HIGH 300

class Car
{
	Tank tank;
	Engine engine;
	bool driver_inside;//Водитель в машине
	struct Control
	{
		std::thread panel_thread;//Отображение панели приборов
		std::thread engine_idle_thread;//Холостой ход двигателя
		std::thread free_wheeling_thread;//Движение машины по инерции
	}control;
	const int MAX_SPEED;
	int speed;
public:
	Car(double engine_consumtion, unsigned int tank_volume, int max_speed) 
		:engine(engine_consumtion), 
		tank(tank_volume), 
		MAX_SPEED(max_speed>=MAX_SPEED_LOW&&max_speed<=MAX_SPEED_HIGH ? max_speed : 200)
	{
		driver_inside = false;
		speed = 0;
		cout << "Your car is ready to go\t" << this << endl;
	}
	~Car()
	{
		cout << "Your car is over" << this << endl;
	}
	void fill(double fuel)
	{
		tank.fill(fuel);
	}
	void start_engine()
	{
		if (tank.get_fuel_level())
		{
			engine.start();
			control.engine_idle_thread = std::thread(&Car::engine_idle, this);
		}
	}
	void stop_engine()
	{
		engine.stop();
		if (control.engine_idle_thread.joinable())control.engine_idle_thread.join();
	}
	void get_in()
	{
		driver_inside = true;
		control.panel_thread = std::thread(&Car::control_panel, this);//Запускаем метод control_panel в отдельном потоке
	}
	void get_out()
	{
		driver_inside = false;
		if (control.panel_thread.joinable())control.panel_thread.join();//Высвобождаем поток метода control_panel
		system("CLS");
		cout << "You are out of car." << endl;
	}
	void control_car()
	{
		char key;
		do
		{
			key = _getch();
			switch (key)
			{
			case Enter://Сесть в машину по Enter. Нужно отобразить панель приборов.
				//if (driver_inside)//Если водитель внутри
				//{
				//	driver_inside = false;
				//	
				//}
				//else//В противном случае
				//{
				//	driver_inside = true;
				//	control.panel_thread = std::thread(&Car::control_panel, this);//останавливаем поток отображающий панель приборов
				//}
				if (driver_inside)get_out();
				else get_in();
				break;
			case'F':case'f'://Заправить машину
				double fuel;
				cout << "Сколько выхотите заправить?";
				cin.clear();
				cin >> fuel;
				fill(fuel);
				break;
			case'I':case'i'://Зажигание (ignition) - завести машину
				if (engine.started())stop_engine();
				else start_engine();
				break;
			case'W':case'w'://Движение вперед
				if (engine.started() && speed <= MAX_SPEED)speed += 10;
				if (!control.free_wheeling_thread.joinable())
				{
					control.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
				}
				std::this_thread::sleep_for(500ms);
				break;
			case'S':case's':
				if (speed > 0)speed -= 10;
				if (speed < 0)speed = 0;
				std::this_thread::sleep_for(400ms);
				break;
			case Escape:
				//if(control.panel_thread.joinable())get_out();
				stop_engine();
				get_out();
				if (control.free_wheeling_thread.joinable())
				{
					speed = 0;
					control.free_wheeling_thread.join();
				}
				break;
			}
			if (speed == 0 && control.free_wheeling_thread.joinable())control.free_wheeling_thread.join();
		} while (key != Escape);
	}
	void engine_idle()
	{
		while (engine.started()
				&& tank.give_fuel(engine.get_consumption_per_second()))
			std::this_thread::sleep_for(1s);
	}
	void free_wheeling()//Свободное торможение в результате действия силы трения
	{
		while (speed)
		{
			speed-=2;
			if (speed < 0)speed = 0;
			std::this_thread::sleep_for(1s);
		}
	}
	void control_panel()
	{
		while (driver_inside)
		{
			system("CLS");
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			for (int i = 0; i < speed / 4; i++)
			{
				SetConsoleTextAttribute(hConsole, 0x07);
				if (i > 20/4)SetConsoleTextAttribute(hConsole, 0x0A);
				if(i>100/4)SetConsoleTextAttribute(hConsole, 0x0E);
				if(i>200/4)SetConsoleTextAttribute(hConsole, 0x0C);
				cout << "|";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			cout << endl;
			cout << "Fuel level: " << tank.get_fuel_level() << " liters.";
			if (tank.get_fuel_level() < 5)
			{
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 0x0C);
				cout << "\tWARNING! LOW FUEL LEVEL!";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			if(!engine.started())cout << "\nConsumtion: 0 liters.\n";
			else cout << "\nConsumtion: " << engine.get_consumption_per_second() << " liters.\n";
			cout << "Engine is " << (engine.started() ? "started" : "stopped") << endl;
			cout << "Speed: " << speed << " km/h.\n";
			std::this_thread::sleep_for(1s);
		}
	}
	void info()
	{
		tank.info();
		engine.info();
	}
};

//#define TANK_CHECK
//#define ENGINE_CHECK
#define CAR_CHECK

void main()
{
	setlocale(LC_ALL, "");

#ifdef TANK_CHECK
	Tank tank(120);
	tank.info();
	int fuel;
	while (true)
	{
		cout << "Введите количество заправляемых литров топлива:"; cin >> fuel;
		tank.fill(fuel);
		tank.info();
	}
#endif // TANK_CHECK

#ifdef ENGINE_CHECK
	Engine engine(9);
	engine.info();
#endif // ENGINE_CHECK

#ifdef CAR_CHECK
	Car BMW(20, 80, 250);
	cout << "Press Enter to get in" << endl;
	BMW.control_car();
#endif // CAR_CHECK
}