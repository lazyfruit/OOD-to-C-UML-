#include<iostream>
#include<conio.h>
#include<Windows.h>
#include<chrono>
#include<thread>
using namespace std;

#define MIN_TANK_VOLUME 40
#define MAX_TANK_VOLUME 80
#define MIN_ENGINE_CONSUMPTION	4
#define MAX_ENGINE_CONSUMPTION	25

class Tank
{
	const unsigned int VOLUME;
	double fuel_level;
public:
	const unsigned int get_VOLUME() const
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
		if (fuel_level + fuel > VOLUME)return fuel_level = VOLUME;
		//if (fuel_level + fuel >= 0 && fuel_level + fuel <= VOLUME)fuel_level += fuel;
		else return fuel_level += fuel;
	}
	double give_fuel(double fuel)
	{
		fuel_level -= fuel;
		if (fuel_level < 0)fuel_level = 0;
		return fuel_level;
	}
	Tank(unsigned int volume)
		:VOLUME(volume >= MIN_TANK_VOLUME && volume <= MAX_TANK_VOLUME ? volume : MAX_TANK_VOLUME),
		fuel_level(0)
	{
		//this->VOLUME = volume;
		cout << "Топливная система активна.\t" << this << endl;
	}
	~Tank()
	{
		cout << "Топливная система отключена.\t" << this << endl;
	}
	void info()const
	{
		cout << "Объем бензобака:\t" << VOLUME << endl;
		cout << "Уровень топлива:\t" << fuel_level << endl;
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
	bool started()const
	{
		return is_started;
	}
	bool start()
	{
		return is_started = true;
	}
	bool stop()
	{
		return is_started = false;
	}
	void set_consumtion(double consumption)
	{
		if (consumption >= MIN_ENGINE_CONSUMPTION && consumption <= MAX_ENGINE_CONSUMPTION)
			this->consumption = consumption;
		else
			this->consumption = MAX_ENGINE_CONSUMPTION / 2;
		consumption_per_second = this->consumption * .3e-4;
	}
	explicit Engine(double consumption)
	{
		set_consumtion(consumption);
		is_started = false;
		cout << "Двигатель исправен.\t" << this << endl;
	}
	~Engine()
	{
		cout << "Двигатель отключен.\t" << this << endl;
	}
	void info()const
	{
		cout << "Расход топлива:\t" << consumption << endl;
		cout << "Моментальный расход:\t" << consumption_per_second << endl;
		cout << "Двигатель " << (is_started ? "запущен." : "заглушен.") << endl;
	}
};

#define Enter	13
#define Escape	27

#define MAX_SPEED_LOW 120
#define MAX_SPEED_HIGH 400

class Car
{
	Tank tank;
	Engine engine;
	bool driver_inside;
	struct Control
	{
		std::thread panel_thread;			//Отображение панели приборов
		std::thread engine_idle_thread;		//Холостой ход двигателя
		std::thread free_wheeling_thread;	//Движение машины по инерции
	}control;

	const int MAX_SPEED;
	int speed;
public:
	Car(double engine_consumption, unsigned int tank_volume, int max_speed) :
		engine(engine_consumption),
		tank(tank_volume),
		MAX_SPEED(max_speed >= MAX_SPEED_LOW && max_speed <= MAX_SPEED_HIGH ? max_speed : 200)
	{
		driver_inside = false;//Когда машина сходит с конвеера, в ней нет водителя
		speed = 0;
		cout << "Ваш автомобиль активен\t" << this << endl;
	}
	~Car()
	{
		cout << "Ваш автомобиль отключен" << endl;
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
		control.panel_thread = std::thread(&Car::control_panel, this);	//Запускаем метод control_panel в отдельном потоке
	}
	void get_out()
	{
		driver_inside = false;
		if (control.panel_thread.joinable())control.panel_thread.join();	//Останавливаем выполнение потока panel_thread.
		system("CLS");
		cout << "Вы вышли из машины" << endl;
	}

	void control_car()
	{
		char key;
		do
		{
			key = _getch();
			switch (key)
			{
			case Enter://Сесть в машину. Нужно отобразить панель приборов
				if (driver_inside)get_out();
				else get_in();
				break;
			case 'F':case 'f'://Заправить машину
				double fuel;
				cout << "Сколько бензина вы хотите заправить?\t";
				cin.clear();
				cin >> fuel;
				fill(fuel);
				break;
			case 'I':case 'i'://Ignition - зажигание
				if (engine.started())stop_engine();
				else start_engine();
				break;
			case 'W':case 'w':
				if (engine.started() && speed <= MAX_SPEED)speed += 10;
				if (!control.free_wheeling_thread.joinable())
				{
					control.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
				}
				std::this_thread::sleep_for(.4s);
				break;
			case 'S':case 's':
				if (speed > 0)speed -= 10;
				if (speed < 0)speed = 0;
				std::this_thread::sleep_for(.4s);
				break;
			case Escape:
				//if (control.panel_thread.joinable())
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
		} while (key != 27);
	}

	void engine_idle()
	{
		while (
			engine.started() &&
			tank.give_fuel(engine.get_consumption_per_second())
			)
			std::this_thread::sleep_for(1s);
	}
	void free_wheeling()
	{
		//Свободное торможение, в результате силы трения
		while (speed)
		{
			speed--;
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
				if (i > 20 / 4)SetConsoleTextAttribute(hConsole, 0x0A);
				if (i > 100 / 4)SetConsoleTextAttribute(hConsole, 0x0E);
				if (i > 200 / 4)SetConsoleTextAttribute(hConsole, 0x0C);
				cout << "|";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			cout << endl;
			cout << "Уровень топлива: " << tank.get_fuel_level() << " литров.";
			if (tank.get_fuel_level() < 5)
			{
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 0x0C);
				cout << "\tВНИМАНИ! Низкий уровень топлива!";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			if (!engine.started())cout << "\nМоментальный расход: 0 литров.\n";
			else cout << "\nМоментальный расход: " << engine.get_consumption_per_second() << " литров.\n";
			cout << "Двигатель " << (engine.started() ? "запущен." : "заглушен.") << endl;
			cout << "Скорость: " << speed << " км/ч.\n";
			std::this_thread::sleep_for(1s);
		}
	}

	void info()const
	{
		tank.info();
		engine.info();
	}
};

//#define TANK_CHECK
//#define ENGINE_CHECK

void main()
{
	setlocale(LC_ALL, "");
#ifdef TANK_CHECK
	Tank tank(120);
	tank.info();
	int fuel;
	while (true)
	{
		cout << "Введите объем:"; cin >> fuel;
		tank.fill(fuel);
		tank.info();
	}
#endif // DEBUG

#ifdef ENGINE_CHECK
	Engine engine(9);
	engine.info();
#endif // ENGINE_CHECK

	Car bmw(20, 80, 250);
	cout << "Автомобиль готов. Вы можете сесть в машину." << endl;
	bmw.control_car();
}