
#include <iostream>

#include "SafeQueue.h"


int main()
{

    SafeQueue<int> Test(10);
    Test.PushElement(5);
    Test.PushElement(8);
    Test.PushElement(10);
    Test.PushElement(350);
    Test.Display();
    std::cout << "\n";

    Test.PopElement();
    Test.PushElement(5);
    Test.PushElement(10);
    Test.PushElement(30);
    Test.PushElement(2);
    Test.PushElement(1);
    Test.PushElement(7);
    Test.PushElement(11);
    Test.PushElement(22);
    Test.Display();
}

