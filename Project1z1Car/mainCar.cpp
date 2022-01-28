#include<iostream>
using namespace std;
using std::cin;
using std::cout;
using std::endl;

#define MIN_TANK_VOLUME 40
#define MAX_TANK_VOLUME 80

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

void main()
{
	setlocale(LC_ALL, "");

	Tank tank(120);
	tank.info();
	int fuel;
	while (true)
	{
		cout << "Введите количество заправляемых литров топлива:"; cin >> fuel;
		tank.fill(fuel);
		tank.info();
	}
}