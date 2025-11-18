// WhackAThread.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <Windows.h>
#include <vector>

struct ThreadParam
{
	int posX;
	int posY;
	int state;
	int difficulty;

	int* points;
	int* lifes;
};


DWORD WINAPI WholeThread(void* params)
{

	HANDLE this_thread = GetCurrentThread();
	bool goToWait = true;
	float currentTime = 0.0f;


	ThreadParam* pParams = (ThreadParam*)params;

	srand((pParams->posX * pParams->posY) * time(NULL) + pParams->posY);
	char rChar = rand() % (89 - 65 + 1) + 65;

	int respawnCoolDown = 50 / pParams->difficulty;

	float TimeUP = 5000.0f / pParams->difficulty;

	float upCD = 0.0f;

	while(true)
	{
		if (goToWait == false)
		{
			pParams->state = (int)rChar;

			if (GetAsyncKeyState(rChar))
			{
				pParams->state = 1;
				goToWait = true;
				*pParams->points += 1;
			}

			upCD += 0.001f;
			if (upCD >= TimeUP)
			{
				pParams->state = 1;
				goToWait = true;
				*pParams->lifes -= 1;
			}
		}
		else
		{
			upCD = 0.0f;
			goToWait = false;
			int rTime = rand() % (respawnCoolDown - 5 + 1) + 5;
			WaitForSingleObject(this_thread, rTime * 100);
			rChar = rand() % (90 - 65 + 1) + 65;
			pParams->posX = rand() % (100 - 2 + 1) + 2;
		}
	}
}

int main()
{
	std::cout << "\33[?25l";
	std::cout << "\33[1;33m \t  ==== WHACK A THREAD ==== \33[m \n";
	std::cout << "\33[1m  Press the letter in () to kill the Thread\33[m";

	std::cout << "\33[2;100H";
	std::cout << "SCORE:";

	std::cout << "\33[2;70H";
	std::cout << "HEALTH:";

	DWORD threadId;

	int points = 0;
	int hp = 3;

	int threadCount = 7;
	int difficulty = 1;


	std::vector<ThreadParam*> threadList = std::vector<ThreadParam*>(threadCount);
	std::vector<HANDLE> tHandle = std::vector<HANDLE>(threadCount);

	int j = 0;
	for (int i = 0; i < threadCount; i++)
	{
		threadList[i] = new ThreadParam();
		threadList[i]->posX = (i + 5) + 2;
		threadList[i]->posY = (i + 5) + j;
		threadList[i]->state = 0;
		threadList[i]->points = &points;
		threadList[i]->lifes = &hp;
		threadList[i]->difficulty = difficulty;
		tHandle[i] = CreateThread(NULL, 0, WholeThread, threadList[i], 0, &threadId);

		if (tHandle[i] == NULL) {
			std::cout <<"Error when creating thread at id : " << threadId << std::endl;
			ExitProcess(3);
		}
		j += 2;
	}
	
	bool run = true;

	while(run)
	{
		for (int i = 0; i < threadCount; i++)
		{
			if (threadList[i]->state > 1)
			{
				std::cout << "\33[" << threadList[i]->posY << ";" << threadList[i]->posX << "H";
				std::cout << "THREAD (" << "\33[1m" << (char)threadList[i]->state << "\33[m)";
			}
			else if (threadList[i]->state == 1)
			{
				std::cout << "\33[" << threadList[i]->posY << ";" << threadList[i]->posX << "H";
				std::cout << "\33[0K";
				threadList[i]->state = 0;
			}
		}

		// Draw Points 

		std::cout << "\33[3;100H";
		std::cout << points;

		std::cout << "\33[3;70H";
		std::cout << hp;

		if (hp <= 0)
			run = false;
	}
	
	std::cout << "\33[2J";
	std::cout << "\33[10;35H";
	std::cout << "GAME OVER";
	std::cout << "\33[11;35H";
	std::cout << "FINAL SCORE : " << points;

	for (int i = 0; i < 10; i++)
		std::cout << "\n";

	for (int i = 0; i < threadCount; i++)
	{
		delete threadList[i];

		BOOL thread_destruction_res = CloseHandle(tHandle[i]);
		if (thread_destruction_res == false)
			std::cout << "Error while destroying\n";
	}

	threadList.clear();

	return 1;
}
