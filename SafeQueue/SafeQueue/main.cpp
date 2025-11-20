
#include <iostream>
#include <vector>

#include "SafeQueue.h"

struct ServerParam
{
    int Key;
    int input;
    SafeQueue<int>* pQueue;
};

struct Particle
{
    int posX;
    int posY;
    char value;
    int color;
    int aliveTime;
};

struct ParticleSystem
{
    int* popCount;
    bool* update;
    int cooldownSpawn;
    int maxParticles;
    int maxAliveTime;
    SafeQueue<Particle>* pParticles;
};

DWORD WINAPI ParticleGenerator(void* params)
{

    HANDLE this_thread = GetCurrentThread();
    bool goToWait = true;
    float currentTime = 0.0f;

    ParticleSystem particles = *(ParticleSystem*)params;
    while (true)
    {
        if (currentTime <= 0.0f &&  particles.pParticles->GetSize() < particles.maxParticles)
        {
            Particle newParticle;
            newParticle.posX = rand() % (100 - 2 + 1) + 2;
            newParticle.posY = 0;
            newParticle.value = 'o';
            newParticle.color = rand() % (39 - 30 + 1) + 30;
            newParticle.aliveTime = 0;
            bool tryPush = true;
            while(tryPush)
            {
                if (particles.pParticles->PushElement(newParticle))
                {
                    currentTime = particles.cooldownSpawn;
                    tryPush = false;
                }
            }
        }
        currentTime -= 0.01f;
    }
    return 0;
}

DWORD WINAPI ParticleCalculator(void* params)
{

    HANDLE this_thread = GetCurrentThread();
    bool goToWait = true;
    float currentTime = 0.0f;

    ParticleSystem particles = *(ParticleSystem*)params;
    *particles.update = true;
    while (true)
    {
        if (TryEnterCriticalSection(particles.pParticles->GetCriticalSection()))
        {
            Particle* pEl = particles.pParticles->GetElementList();
            for (int i = 0; i < particles.pParticles->GetSize(); i++)
            {
                pEl[i].posY += 1;
                pEl[i].aliveTime += 1;
                if (pEl[i].aliveTime >= particles.maxAliveTime)
                    *particles.popCount += 1;
            }
            LeaveCriticalSection(particles.pParticles->GetCriticalSection());
            *particles.update = true;
        }
    }

    return 0;
}

DWORD WINAPI ParticleDrawer(void* params)
{

    HANDLE this_thread = GetCurrentThread();
    bool goToWait = true;
    float currentTime = 0.0f;

    ParticleSystem particles = *(ParticleSystem*)params;
    while (true)
    {
        if (*particles.update == false)
            continue;

        if (TryEnterCriticalSection(particles.pParticles->GetCriticalSection()))
        {
            std::cout << "\33[2J";
            Particle* pEl = particles.pParticles->GetElementList();
            for (int i = 0; i < particles.pParticles->GetSize();i++)
            {
                if (pEl[i].posY > 60)
                    continue;
                std::cout << "\33[" << pEl[i].posY << ";" << pEl[i].posX << "H";
                std::cout << "\33[1;36m" << (char)pEl[i].value << "\33[m";
            }
            LeaveCriticalSection(particles.pParticles->GetCriticalSection());
            *particles.update = false;
        }
    }
    return 0;
}

DWORD WINAPI ParticleDeletor(void* params)
{
    HANDLE this_thread = GetCurrentThread();
    bool goToWait = true;
    float currentTime = 0.0f;

    ParticleSystem particles = *(ParticleSystem*)params;
    while (true)
    {
        while (*particles.popCount > 0)
        {
            if (particles.pParticles->PopElement() != nullptr)
                *particles.popCount -= 1;
        }
    }
    return 0;
}



DWORD WINAPI ServerThread(void* params)
{

	HANDLE this_thread = GetCurrentThread();
	bool goToWait = true;
	float currentTime = 0.0f;

    ServerParam* pParams = (ServerParam*)params;

	while (true)
	{
       pParams->pQueue->PushElement(pParams->input);
	}
    return 0;
}

DWORD WINAPI CalculateParticlesPosition(void* params)
{

    HANDLE this_thread = GetCurrentThread();
    bool goToWait = true;
    float currentTime = 0.0f;

    ServerParam* pParams = (ServerParam*)params;

    while (true)
    {
        pParams->pQueue->PushElement(pParams->input);
    }
    return 0;
}

DWORD WINAPI CustomerThread(void* params)
{

    HANDLE this_thread = GetCurrentThread();
    bool goToWait = true;
    float currentTime = 0.0f;

    ServerParam* pParams = (ServerParam*)params;
    bool tryConsole = true;
    while (true)
    {
        int* res = pParams->pQueue->PopElement();
    }
    return 0;
}



int main()
{
    SafeQueue<Particle> Test(10);
    bool a = false;
    int b = 0;
    ParticleSystem system{};
    system.update = &a;
    system.popCount = &b;
    system.cooldownSpawn = 10000.0f;
    system.maxParticles = 100;
    system.pParticles = &Test;
    system.maxAliveTime = 5000.0f;
    
    int count = 4;
    DWORD id;
    std::vector<HANDLE> tHandle = std::vector<HANDLE>(count);

    tHandle[0] = CreateThread(NULL, 0, ParticleGenerator, &system, 0, &id);
    tHandle[1] = CreateThread(NULL, 0, ParticleDrawer, &system, 0, &id);
    tHandle[2] = CreateThread(NULL, 0, ParticleCalculator, &system, 0, &id);
    tHandle[3] = CreateThread(NULL, 0, ParticleDeletor, &system, 0, &id);


    bool run = true;
    while(run)
    {
        if (GetAsyncKeyState('E'))
            run = false;
    }
    std::cout << "END\n";

    for (int i = 0; i < count; i++)
    {
        BOOL thread_destruction_res = CloseHandle(tHandle[i]);
        if (thread_destruction_res == false)
            std::cout << "Error while destroying\n";
    }

    return 1;
}

