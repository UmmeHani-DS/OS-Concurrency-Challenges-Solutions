#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib> 
#include <ctime>

using namespace std;

#define MAX_CARS 4
#define MAX_BUSES 1

int bridgeCars = 0;
int bridgeBus = 0;

pthread_mutex_t mutexBridge = PTHREAD_MUTEX_INITIALIZER;


bool isBridgeAvailable(int vehicleType) 
{
    if (vehicleType == 1)  // Vehicle is a bus
    {
        if (bridgeCars == 0 && bridgeBus == 0)     // Check if the bridge has no cars or buses currently on it
        {
            return true; 
        }
        else 
        {
            return false; 
        }
    } 
    else  // Not a bus
    {
        if (bridgeCars < MAX_CARS && bridgeBus == 0)  // Check if there are less than four cars on the bridge and no buses
        {
            return true;      
        } 
        else 
        {
            return false; 
        }
    }
}


void ArriveAtBridge( int direction , int vehicleType) 
{
        pthread_mutex_lock(&mutexBridge);

        while (!isBridgeAvailable(vehicleType)) 
        {
            pthread_mutex_unlock(&mutexBridge);
            pthread_mutex_lock(&mutexBridge);
        }

        if (vehicleType == 0)   // Vehicle is car
        { 
            bridgeCars++;

             if (direction == 0) 
            { 
                cout << "Car is crossing bridge from right to left." << endl;
                cout << "Cars on bridge: " << bridgeCars << endl;
            } 
            else 
            {
                cout << "Car is crossing bridge from left to right." << endl;
                cout << "Cars on bridge: " << bridgeCars << endl;
            }

        } 
        else    // Vehicle is bus
        { 
            bridgeBus++;

            if (direction == 0) 
            {
                cout << "Bus is crossing bridge from right to left." << endl;
                cout << "Buses on bridge: " << bridgeBus << endl;
            } 
            else 
            {
                cout << "Bus is crossing bridge from left to right." << endl;
                cout << "Buses on bridge: " << bridgeBus << endl;
            }
        }

        pthread_mutex_unlock(&mutexBridge);

}

void ExitTheBridge(int direction, int vehicleType) 
{
    pthread_mutex_lock(&mutexBridge);

    if (vehicleType == 0)       // Vehicle is a car
    {
        bridgeCars--;

         if (direction == 0) 
        { 
            cout << "Car is exiting bridge from right to left." << endl;
            cout << "Cars on bridge: " << bridgeCars << endl;
        } 
        else 
        {
            cout << "Car is exiting bridge from left to right." << endl;
            cout << "Cars on bridge: " << bridgeCars << endl;
          
        }
    } 

    else                       // Vehicle is bus
    {
        bridgeBus--;

        if (direction == 0) 
        {
            cout << "Bus is exiting bridge from right to left." << endl;
            cout << "Buses on bridge: " << bridgeBus << endl;
        } 
        else 
        {
            cout << "Bus is exiting bridge from left to right." << endl;
            cout << "Buses on bridge: " << bridgeBus << endl;
        }

    }

    pthread_mutex_unlock(&mutexBridge);
}

void* VehicleThread(void* args) 
{
    int* params = (int*)args;
    int direction = params[0];
    int vehicleType = params[1];

    
    ArriveAtBridge(direction, vehicleType);
        
    usleep(100000);   

    ExitTheBridge(direction, vehicleType);
        
    usleep(100000);   

    pthread_exit(NULL);
}

int main()
{
    srand(time(0));

    const int numCars = 4;
    const int numBuses = 2;
    const int t_vehicles = numCars + numBuses; 

    pthread_t threads[t_vehicles];

    for (int i = 0; i < t_vehicles; i++) 
    {
        int direction = rand() % 2;                  // Random direction right = 0 or left = 1
        int vehicleType = (i < numBuses) ? 1 : 0;   // Set as bus = 1 and car = 0

        int* args = new int[2];
        args[0] = direction;
        args[1] = vehicleType;

        pthread_create(&threads[i], nullptr, VehicleThread, (void*)args);
    }

    for (int i = 0; i < numCars + numBuses; i++) 
    {
        pthread_join(threads[i], nullptr);
    }

    pthread_mutex_destroy(&mutexBridge);

    return 0;
}