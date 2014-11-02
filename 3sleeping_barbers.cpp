/* From William Stalling's OS book (Barbershop problem given in the appendix) 
 * $ g++ ./3usleeping_barbers.cpp -pthread                                         */

#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
using namespace std;

const int totalChairs = 10;
const int totalCustomers = 50;
const int totalBarbers = 3;

sem_t max_capacity;
sem_t sofa, barber_chair, coord, mutex1, mutex2;
sem_t cust_ready, leave_b_chair, payment, receipt;
sem_t finished[totalCustomers];

int custq[totalCustomers + 1], front = 0, rear = 0;

void insert(int id)
{
    custq[rear++] = id;
}

int rem()
{
    return custq[front++];
}

void initializeSemaphores()
{
    sem_init(&max_capacity, 0, 20);
    sem_init(&sofa, 0, 4);
    sem_init(&barber_chair, 0, 3);
    sem_init(&coord, 0, 3);
    sem_init(&mutex1, 0, 1);
    sem_init(&mutex2, 0, 1);
    sem_init(&cust_ready, 0, 0);
    sem_init(&leave_b_chair, 0, 0);
    sem_init(&payment, 0, 0);
    sem_init(&receipt, 0, 0);
    for (int i = 0; i < totalCustomers; i++)
        sem_init(&finished[i], 0, 0);
}

class Customer {
    int cid;
public:
    void set(int);
    static void enter_shop(Customer *);
    static void sit_on_sofa(Customer *);
    static void get_up_from_sofa(Customer *);
    static void sit_in_barber_chair(Customer *);
    static void leave_barber_chair(Customer *);
    static void pay(Customer *);
    static void exit_shop(Customer *);
    static void *run(void *);
};

void Customer::set(int id)
{
    cid = id;
}


void Customer::enter_shop(Customer * cust)
{
    cout << "Customer " << cust->cid << " enters barber shop" << endl;
    usleep(5000);
}

void Customer::sit_on_sofa(Customer * cust)
{
    cout << "Customer " << cust->cid << " sits on sofa" << endl;
    usleep(5000);
}

void Customer::get_up_from_sofa(Customer * cust)
{
    cout << "Customer " << cust->cid << " gets up from sofa" << endl;
    usleep(5000);
}

void Customer::sit_in_barber_chair(Customer * cust)
{
    cout << "Customer " << cust->cid << " sits on barber chair" << endl;
    usleep(5000);
}

void Customer::leave_barber_chair(Customer * cust)
{
    cout << "Customer " << cust->cid << " finishes haircut and leaves barber chair" << endl;
    usleep(5000);
}

void Customer::pay(Customer * cust)
{
    cout << "Customer " << cust->cid << " makes payment" << endl;
    usleep(5000);
}

void Customer::exit_shop(Customer * cust)
{
    cout << "Customer " << cust->cid << " exits the barber shop" << endl;
    usleep(5000);
}

void *Customer::run(void *obj)
{
    // mutex1 has been used in some places so as to not mix up print statements of different functions
    Customer *cust = (Customer *) obj;
    sem_wait(&max_capacity);
    sem_wait(&mutex1);
    enter_shop(cust);
    sem_post(&mutex1);
    sem_wait(&mutex1);
    int custnr = cust->cid;
    sem_post(&mutex1);
    sem_wait(&sofa);
    sem_wait(&mutex1);
    sit_on_sofa(cust);
    sem_post(&mutex1);
    sem_wait(&barber_chair);
    sem_wait(&mutex1);
    get_up_from_sofa(cust);
    sem_post(&mutex1);
    sem_post(&sofa);
    sem_wait(&mutex1);
    sit_in_barber_chair(cust);
    sem_post(&mutex1);
    sem_wait(&mutex2);
    insert(custnr);
    sem_post(&cust_ready);
    sem_post(&mutex2);
    sem_wait(&finished[custnr]);
    sem_wait(&mutex1);
    leave_barber_chair(cust);
    sem_post(&mutex1);
    sem_post(&leave_b_chair);
    sem_wait(&mutex1);
    pay(cust);
    sem_post(&mutex1);
    sem_post(&payment);
    sem_wait(&receipt);
    sem_wait(&mutex1);
    exit_shop(cust);
    sem_post(&mutex1);
    sem_post(&max_capacity);
}

class Barber {
    int b_id;
public:
    void set(int);
    static void *run(void *);
    static void cut_hair(Barber *, int);
    static void accept_pay(Barber *, int);
};

void Barber::set(int i)
{
    b_id = i;
}

void Barber::cut_hair(Barber * barb, int custid)
{
    cout << "Barber " << barb->b_id << " cuts hair of Customer " << custid << endl;
}

void Barber::accept_pay(Barber * barb, int custid)
{
    cout << "Cashier accepts payment of Customer " << custid << endl;
}

void *Barber::run(void *obj)
{
    Barber *barb = (Barber *) obj;
    int b_cust;
    while (true) {
        sem_wait(&cust_ready);
        sem_wait(&mutex2);
        b_cust = rem();               // remove from queue
        sem_post(&mutex2);
        sem_wait(&coord);
        sem_wait(&mutex1);
        cut_hair(barb, b_cust);
        sem_post(&mutex1);
        sem_post(&coord);
        sem_post(&finished[b_cust]);
        sem_wait(&leave_b_chair);
        sem_post(&barber_chair);
        sem_wait(&payment);
        sem_wait(&coord);
        sem_wait(&mutex1);
        accept_pay(barb, b_cust);
        sem_post(&mutex1);
        sem_post(&coord);
        sem_post(&receipt);
    }
}

int main()
{
    Customer cust[totalCustomers];
    Barber barb[totalBarbers];
    
    for (int i = 0; i < totalCustomers; i++)
        cust[i].set(i);
    for (int i = 0; i < totalBarbers; i++)
        barb[i].set(i);
    initializeSemaphores();
    pthread_t barberThread[totalBarbers], customerThread[totalCustomers];
    for (int i = 0; i < totalBarbers; i++) {
        pthread_create(&barberThread[i], NULL, &Barber::run, (void *)&barb[i]);
    }
    for (int i = 0; i < totalCustomers; i++) {
        pthread_create(&customerThread[i], NULL, &Customer::run, (void *)&cust[i]);
    }
    for (int i = 0; i < totalCustomers; i++) {
        pthread_join(customerThread[i], NULL);
    }
    return 0;
}
