#pragma once

#include <Windows.h>

template <typename T>
class SafeQueue
{
	T* pElements;
	int size;
	int capacity;

public:
	SafeQueue(int size = 10);

	void PushElement(T element);
	T PopElement();
	
	void Display();
};

template<typename T>
inline SafeQueue<T>::SafeQueue(int _maxsize)
{
	size = 0;
	capacity = _maxsize;
	pElements = new T[capacity];
}

template<typename T>
inline void SafeQueue<T>::PushElement(T element)
{
	//InitializeCriticalSection(&element);
	if (size == capacity)
	{
		capacity += 10;
		T* tempPtr = new T[size];
		for (int i = 0; i < size; i++)
		{
			tempPtr[i] = pElements[i];
		}

		delete[]pElements;

		pElements = new T[capacity];
		for (int i = 0; i < size; i++)
		{
			pElements[i] = tempPtr[i];
		}
		delete[]tempPtr;
	}
	pElements[size++] = element;
}

template<typename T>
inline T SafeQueue<T>::PopElement()
{
	T returnElement = pElements[0];

	for (int i = 0; i + 1 < size; i++)
	{
		pElements[i] = pElements[i + 1];
	}
	size -= 1;

	return returnElement;
}

template<typename T>
inline void SafeQueue<T>::Display()
{
	for (int i = 0; i < size; i++)
	{
		std::cout << pElements[i] << ",";
	}
}

