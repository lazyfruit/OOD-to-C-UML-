#include<iostream>
#include<conio.h>
#include<Windows.h>
#include<chrono>
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
		if (fuel_level + fuel > VOLUME)fuel_level = VOLUME;//ƒаже если мы зальем в бак 200 литров топлива,\
														   в нем все равно будет 40 литров, а все остальное выльетс€ наружу
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
	bool start()
	{
		is_started = true;
	}
	bool stop()
	{
		is_started = false;
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

class Car
{
	Tank tank;
	Engine engine;
	bool driver_inside;//¬одитель в машине
public:
	Car(double engine_consumtion, unsigned int tank_volume) :engine(engine_consumtion), tank(tank_volume)
	{
		driver_inside = false;
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
		if (tank.get_fuel_level())engine.start();
	}
	void stop_engine()
	{
		engine.stop();
	}
	void get_in()
	{
		driver_inside = true;
	}
	void get_out()
	{
		driver_inside = false;
	}
	void controle()
	{
		char key;
		do
		{
			key = _getch();
			switch (key)
			{
			case 13://—есть в машину по Enter. Ќужно отобразить панель приборов.
				break;
			case'F':case'f'://«аправить машину
				double fuel;
				cout << "—колько выхотите заправить?"; cin >> fuel;
				fill(fuel);
				break;
			case'I':case'i'://«ажигание (ignition) - завести машину
				break;
			}
		} while (key != 27);
	}
	void control_panel()
	{
		while (driver_inside)
		{
			cout << "Fuel level: " << tank.get_fuel_level() << " liters.\n";
			cout << "Engine is " << (engine.start() ? "started" : "stopped") << endl;
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
		cout << "¬ведите количество заправл€емых литров топлива:"; cin >> fuel;
		tank.fill(fuel);
		tank.info();
	}
#endif // TANK_CHECK

#ifdef ENGINE_CHECK
	Engine engine(9);
	engine.info();
#endif // ENGINE_CHECK

#ifdef CAR_CHECK
	Car BMW(8, 80);
	BMW.info();
#endif // CAR_CHECK
}