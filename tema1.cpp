#include <fstream>
#include <math.h>
#include <iomanip>
#include <stdlib.h>

using namespace std;

ifstream f1("ram.in");
ofstream g1("ram.out");
ifstream f2("operations.in");
ofstream g2("cache.out");


// functia de hash pt L1
int hashL1(int key)
{
	int k = 0, hkey = 0, base2[100] = { 0 };
	while (key != 0)
	{
		base2[k++] = key % 2;
		key = key / 2;
	}
	for (int i = 2; i <= 12; i++)
	{
		hkey += base2[i] * pow(2, i - 2);
	}
	return hkey;
}

// functia de hash pt L2
int hashL2(int key)
{
	int k = 0, hkey = 0, base2[100] = { 0 };
	while (key != 0)
	{
		base2[k++] = key % 2;
		key = key / 2;
	}
	for (int i = 2; i <= 14; i++)
	{
		hkey += base2[i] * pow(2, i - 2);
	}
	return hkey;
}

// structura set
struct set
{
	int key[2];
	int value[2];
	bool dirtyBit[2];
	int modified;
};

// structura pt retinerea valorilo din ram.in
struct Ram
{
	int key;
	int value;
};

class Hashtable
{
private:


	int size;
	struct set *hashtable;
	int(*hash) (int);


public:

	// constructor
	Hashtable(int(*h) (int), int size)
	{
		this->size = size;
		hashtable = new struct set[size];

		for (int i = 0; i < size; i++)
		{
			hashtable[i].key[0] = -1;
			hashtable[i].key[1] = -1;
			hashtable[i].dirtyBit[0] = false;
			hashtable[i].dirtyBit[1] = false;
			hashtable[i].modified = 0;
		}

		hash = h;
	}

	// destructor
	~Hashtable()
	{
		delete[] hashtable;
	}
	// functia put care insereaza o pereche cheie-valoare in hashtable
	void put(int key, int value)
	{
		int i, k = -1;
		int indexSet = hash(key);

		for (i = 0; i < 2; i++)
		{
			if (hashtable[indexSet].key[i] == -1)
				i = 2;
			k++;
		}

		if (k != 2)
		{
			hashtable[indexSet].key[k] = key;
			hashtable[indexSet].value[k] = value;
		}
		else
		{
			hashtable[indexSet].key[hashtable[indexSet].modified] = key;
			hashtable[indexSet].value[hashtable[indexSet].modified] = value;
			hashtable[indexSet].modified = abs(hashtable[indexSet].modified - 1);
		}
	}
	//functia care face update unei valori de la o anumita adresa din hashtable
	void update(int key, int value)
	{
		int i;
		int indexSet = hash(key);
		for (i = 0; i < 2; i++)
		{
			if (hashtable[indexSet].key[i] == key)
				hashtable[indexSet].value[i] = value;
		}
	}
	// functia care returneaza valoare corespunzatoare unei key
	int getValue(int key)
	{
		int i;
		int indexSet = hash(key);
		for (i = 0; i < 2; i++)
		{
			if (hashtable[indexSet].key[i] == key)
				return hashtable[indexSet].value[i];
		}
	}
	// functia care verifica daca exista o adresa 
	//in hashtable si daca nu este  dirty
	bool  isKey(int key)
	{
		int indexSet = hash(key);
		if ((hashtable[indexSet].key[0] == key
			&& !hashtable[indexSet].dirtyBit[0])
			|| (hashtable[indexSet].key[1] == key
			&& !hashtable[indexSet].dirtyBit[1]))
			return true;
		else
			return false;
	}
	// functia care verifica daca valoarea de la o adresa e dirty
	bool isDirty(int key)
	{
		int i;
		int indexSet = hash(key);
		for (i = 0; i < 2; i++)
		{
			if (hashtable[indexSet].key[i] == key)
				return hashtable[indexSet].dirtyBit[i];
		}
		return false;
	}
	// functia care seteaza valoarea de la o adresa pe dirty
	void setDirtyBit(int key, bool val)
	{
		int i;
		int indexSet = hash(key);
		for (i = 0; i < 2; i++)
		{
			if (hashtable[indexSet].key[i] == key)
				hashtable[indexSet].dirtyBit[i] = val;
		}
	}
	// functia care printeaza continutul unui hashtable
	void print()
	{

		for (int i = 0; i < size; i++)
		{

			for (int j = 0; j < 2; j++)
			{
				if (hashtable[i].key[j] != -1)
				{
					g2 << i << ' ' << j << ' ' << hashtable[i].key[j] << ' ' << hashtable[i].value[j];
					if (hashtable[i].dirtyBit[j])
					{
						g2 << " *" << '\n';
					}
					else
					{
						g2 << '\n';
					}
				}
			}
		}
	}
};
// functia de read
void read(class Hashtable *L11, class Hashtable *L12, 
	class Hashtable *L2, int core, int key, struct Ram ram[], int lenRam)
{
	
	if (core == 0)
	{
		if (!L11->isKey(key))
		{
			if (L2->isKey(key))
			{
				if (L11->isDirty(key))
				{
					L11->update(key, L2->getValue(key));
					L11->setDirtyBit(key, false);
				}
				else
					L11->put(key, L2->getValue(key));
			}
			else
			{
				int value;
				for (int i = 0; i < lenRam; i++)
				{
					if (ram[i].key == key)
					{
						value = ram[i].value;
						i = lenRam;
					}
				}
				L2->put(key, value);
				L11->put(key, value);

			}
		}
	}
	else
	{
		if (!L12->isKey(key))
		{
			if (L2->isKey(key))
			{

				if (L12->isDirty(key))
				{
					L12->update(key, L2->getValue(key));
					L12->setDirtyBit(key, false);
				}
				else
					L12->put(key, L2->getValue(key));
			}
			else
			{
				int value;
				for (int i = 0; i < lenRam; i++)
				{
					if (ram[i].key == key)
					{
						value = ram[i].value;
						i = lenRam;
					}
				}
				L2->put(key, value);
				L12->put(key, value);

			}
		}
	}
}

	

// functia de write
void write(class Hashtable *L11, class Hashtable *L12, 
	class Hashtable *L2, int core, int key, int newData)
{
	if (core == 0)
	{
		if (L11->isKey(key))
		{
			L11->update(key, newData);
			L2->update(key, newData);
			if (L12->isKey(key))
			{
				L12->setDirtyBit(key, true);
			}
		}
		else
		{
			if (L2->isKey(key))
			{
				if (L11->isDirty(key))
				{
					L2->update(key, newData);
					L11->update(key, newData);
					L11->setDirtyBit(key, false);
					if (L12->isKey(key))
					{
						L12->setDirtyBit(key, true);
					}
				}
				else
				{
					L2->update(key, newData);
					L11->put(key, newData);
					if (L12->isKey(key))
					{
						L12->setDirtyBit(key, true);
					}
				}
			}
			else
			{
				L2->put(key, newData);
				L11->put(key, newData);
			}
		}
	}
	else
	{
		if (L12->isKey(key))
		{
			L12->update(key, newData);
			L2->update(key, newData);
			if (L11->isKey(key))
			{
				L11->setDirtyBit(key, true);
			}
		}
		else
		{
			if (L2->isKey(key))
			{
				if (L12->isDirty(key))
				{
					L2->update(key, newData);
					L12->update(key, newData);
					L12->setDirtyBit(key, false);
					if (L11->isKey(key))
					{
						L11->setDirtyBit(key, true);
					}
				}
				else
				{
					L2->update(key, newData);
					L12->put(key, newData);
					if (L11->isKey(key))
					{
						L11->setDirtyBit(key, true);
					}
				}
			}
			else
			{
				L2->put(key, newData);
				L12->put(key, newData);
			}
		}
	}
}




int main()
{
	struct Ram ram[100001];
	int i, lenRam, core, key, newData;
	char operation;
	// initializare hashtable-uri
	Hashtable L11(hashL1, 2048);
	Hashtable L12(hashL1, 2048);
	Hashtable L2(hashL2, 4096);
	
	i = 0;
	// citesc toate valorile din ram si le memorez in structura ram
	while (true)
	{

		f1 >> ram[i].key >> ram[i].value;
		i++;
		if (f1.eof()) break;
	}
	lenRam = i;
	for (i=0; i<lenRam-1; i++)
	{
		g1 << setfill('0') << setw(10) << ram[i].key;
		g1 << ' ';
		g1 << setfill('0') << setw(10) << ram[i].value;
		g1 << '\n';

	}
	f1.close();
	g1.close();
	i = 0;
	// citesc pe rand operatiile din operations.in ce trebuiesc implementate
	while (true)
	{
		f2 >> core;
		f2 >> operation;
		f2 >> key;
		if (operation == 'w')
		{
			f2 >> newData;
			write(&L11, &L12, &L2, core, key, newData);
		}
		else
		{
			read(&L11, &L12, &L2, core, key, ram, lenRam-1);
		}

		if (f2.eof()) break;
	}
	// afisez continutul hashtable-urilor
	L11.print();
	g2 << '\n';
	L12.print();
	g2 << '\n';
	L2.print();
	f2.close();
	g2.close();

	return 0;
}

