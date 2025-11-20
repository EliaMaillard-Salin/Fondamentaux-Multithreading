#pragma once

#include <Windows.h>

template <typename T>
class SafeQueue
{
	T* pElements;
	int size;
	int capacity;

	bool isDestroyed = false;

	CRITICAL_SECTION   QueueLocked;

public:
	SafeQueue(int size = 10);
	~SafeQueue();

	void Delete();

	bool PushElement(T element);
	T* PopElement();
	T* GetElementList() const;

	CRITICAL_SECTION* GetCriticalSection();
	
	int GetSize();
};

template<typename T>
inline SafeQueue<T>::SafeQueue(int _maxsize)
{
	size = 0;
	capacity = _maxsize;
	pElements = new T[capacity];

	InitializeCriticalSection(&QueueLocked);
}

template<typename T>
inline SafeQueue<T>::~SafeQueue()
{
	DeleteCriticalSection(&QueueLocked);
}

template<typename T>
inline void SafeQueue<T>::Delete()
{
	while(true)
	{
		if(TryEnterCriticalSection(&QueueLocked) != 0)
		{
			isDestroyed = true;
			LeaveCriticalSection(&QueueLocked);
			return;
		}
	}
}

template<typename T>
inline bool SafeQueue<T>::PushElement(T element)
{
	if (isDestroyed)
		return false;
	
	if(TryEnterCriticalSection(&QueueLocked) != 0)
	{
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


		LeaveCriticalSection(&QueueLocked);
		return true;
	}
	return false;
}

template<typename T>
inline T* SafeQueue<T>::PopElement()
{
	if (isDestroyed)
		return nullptr;

	if (size > 0)
	{
		if (TryEnterCriticalSection(&QueueLocked) !=0)
		{
			T returnElement = pElements[0];

			for (int i = 0; i + 1 < size; i++)
			{
				pElements[i] = pElements[i + 1];
			}
			size -= 1;

			LeaveCriticalSection(&QueueLocked);
			return &returnElement;
		}
	}
	return nullptr;
	
}

template<typename T>
inline T* SafeQueue<T>::GetElementList() const
{
	if (isDestroyed)
		return nullptr;

	return pElements;
}

template<typename T>
inline CRITICAL_SECTION* SafeQueue<T>::GetCriticalSection()
{
	return &QueueLocked;
}

template<typename T>
inline int SafeQueue<T>::GetSize()
{
	return size;
}

