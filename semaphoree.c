#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
struct Queue {
    int front, rear, size;
    unsigned capacity;
    int* array;
};
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue
            = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;

    // This is important, see the enqueue
    queue->rear = capacity - 1;
    queue->array
            = (int*)malloc(queue->capacity * sizeof(int));
    return queue;
}

// Queue is full when size becomes
// equal to the capacity
int isFull(struct Queue* queue)
{
    return (queue->size == queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}
void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
//    printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue.
// It changes front and size
int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

// Function to get rear of queue
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}
struct Queue *queue;
#define N 5
#define B 5
sem_t counter_mutex, buffer_mutex;
int counter = 0;
void sleepRandom() {
    sleep(rand() % 5 + 1);
}

void* mCounter(void* arg) {
    int id = *((int*)arg);
    while (1) {
        sleepRandom();
        printf("Counter thread %d: received a message\n", id);
        sem_wait(&counter_mutex);
        printf("Counter thread %d: waiting to write\n", id);
        counter++;
        printf("Counter thread %d: now adding to counter, counter value=%d\n", id, counter);
        sem_post(&counter_mutex);
    }
    return NULL;
}

void* mMonitor(void* arg) {
    while (1) {
        sleepRandom();
        sem_wait(&counter_mutex);
        printf("Monitor thread: waiting to read counter\n");
        int count_value = counter;
        counter = 0;
        printf("Monitor thread: reading a count value of %d\n", count_value);
        sem_post(&counter_mutex);
        sem_wait(&buffer_mutex);
        if (isFull(queue)) {
            printf("Monitor thread: Buffer full!!\n");
        } else {
            enqueue(queue,count_value);
            printf("Monitor thread: writing to buffer at position %d\n", queue->front);
        }
        sem_post(&buffer_mutex);
    }
    return NULL;
}

void* mCollector(void* arg) {
    while (1) {
        sleepRandom();
        sem_wait(&buffer_mutex);
        if (isEmpty(queue)) {
            printf("Collector thread: nothing is in the buffer!\n");
        } else {
            dequeue(queue);
            printf("Collector thread: reading from the buffer at position %d\n", queue->rear);
        }
        sem_post(&buffer_mutex);
    }
    return NULL;
}

int main() {
    queue= createQueue(1);
    pthread_t mCounterThreads[N], mMonitorThread, mCollectorThread;
    int counterThreadIDs[N];
    sem_init(&counter_mutex, 0, 1);
    sem_init(&buffer_mutex, 0, 1);
    for (int i = 0; i < N; ++i) {
        counterThreadIDs[i] = i + 1;
        pthread_create(&mCounterThreads[i], NULL, mCounter, &counterThreadIDs[i]);
    }
    pthread_create(&mMonitorThread, NULL, mMonitor, NULL);
    pthread_create(&mCollectorThread, NULL, mCollector, NULL);
    pthread_join(mMonitorThread, NULL);
    pthread_join(mCollectorThread, NULL);
    for (int i = 0; i < N; ++i) {
        pthread_join(mCounterThreads[i], NULL);
    }
    sem_destroy(&counter_mutex);
    sem_destroy(&buffer_mutex);

    return 0;
}


