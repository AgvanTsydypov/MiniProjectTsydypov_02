#include <pthread.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <map>
#include <vector>
#include <semaphore.h>
#include <zconf.h>

using namespace std;

int count;
sem_t mute;
sem_t semWriter;
vector<int> marks;

//поток читателя закрывает блокировку для чтения перед получением значения из массива
void *ReadInfo(void *param) {
    srand(time(nullptr));
    int rNum = *((int *) param);
    int i = marks.size();

    while (i > 0) {
        sleep(1);
        sem_wait(&mute);
        count++;
        if (count == 1)
            sem_wait(&semWriter);
        sem_post(&mute);

        int index = rand() % marks.size();
        int element = marks.at(index);
        cout <<"num "<< rNum << " reader " << ": Mark is " << element << "for element[" << index << "].\n";

        sem_wait(&mute);
        if (--count == 0)
            sem_post(&semWriter);
        sem_post(&mute);
        i--;
    }
    return nullptr;
}

//поток читателя закрывает блокировку для записи перед измением элемента массива
void *WriteInfo(void *param) {
    srand(time(nullptr));
    int wNum = *((int *) param);
    int i = marks.size();

    while (i > 0) {
        sem_wait(&semWriter);
        sleep(1);

        int index = rand() % marks.size();
        marks.at(index) = rand() % 10 + 1;
        cout <<"num "<< wNum << " writer " << ": element[" << index << "] = " << marks.at(index) << "\n";

        sem_post(&semWriter);
        i--;
    }
    return nullptr;
}

//проверка входных данных
int GetNumber(int left, int right)
{
    int temp;
    while (true)
    {
        std::cout << "The number must fall within the interval [" << left << ";" << right << "]" << std::endl;
        std::cin >> temp;
        if (temp < left || temp > right)
            continue;
        else
            break;
    }
    return temp;
}

int main() {

    srand(time(nullptr));

    //Проверка ввода
    printf("Enter the number of readers:\n");
    int numReaders = GetNumber(0,50); //кол-во писателей


    printf("Enter the number of writers:\n");
    int numWriters = GetNumber(0,50); //кол-во читателей


    sem_init(&mute, 0, 1);
    sem_init(&semWriter, 0, 1);

    // Заполнем бд
    cout<<"Marks:\n";
    for (int i = 0; i < 10; i++) {
        marks.push_back(rand()%10);
        cout<<marks[i]<<" ";
    }
    cout<<"\n";

    vector<pthread_t> threadR = vector<pthread_t>(numReaders);
    int readers[numReaders];
    for (int i = 0; i < numReaders; i++) {
        readers[i] = i + 1;
        pthread_create(&threadR[i], nullptr, ReadInfo,(void *) (readers + i));
    }

    vector<pthread_t> threadW = vector<pthread_t>(numWriters);
    int writers[numWriters];
    for (int i = 0; i < numWriters; i++) {
        writers[i] = i + 1;
        pthread_create(&threadW[i], nullptr, WriteInfo,(void *) (writers + i));
    }

    //Ожидание завершения всех созданных потоков
    for (auto t: threadR) {
        pthread_join(t, NULL);
    }

    for (auto t: threadW) {
        pthread_join(t, NULL);
    }

    //Уничтожаем семафоры
    sem_destroy(&mute);
    sem_destroy(&semWriter);

    for (int i = 0; i < 10; i++) {
        cout<<marks[i]<<" ";
    }

    system("pause");
    return 0;
}