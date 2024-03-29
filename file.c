#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_STUDENTS 5
#define NUM_CHAIRS 3

sem_t taSemaphore;
sem_t chairsSemaphore[NUM_CHAIRS];
pthread_mutex_t mutex;

int waitingStudents = 0;

void *studentBehavior(void *id) {
    while (1) {
        // Simulate programming
        sleep(rand() % 5);

        pthread_mutex_lock(&mutex);
        if (waitingStudents < NUM_CHAIRS) {
            waitingStudents++;
            printf("Student %ld sitting in waiting chair. %d waiting.\n", (long)id, waitingStudents);
            pthread_mutex_unlock(&mutex);

            // Wait for TA
            sem_wait(&taSemaphore);

            pthread_mutex_lock(&mutex);
            waitingStudents--;
            pthread_mutex_unlock(&mutex);

            // Getting help
            printf("Student %ld getting help.\n", (long)id);
            sleep(rand() % 5); // Simulate time taken for getting help
        } else {
            pthread_mutex_unlock(&mutex);
            // Go back to programming
            printf("Student %ld will try later.\n", (long)id);
        }
    }
}

void *taBehavior(void *) {
    while (1) {
        // Check for waiting students
        pthread_mutex_lock(&mutex);
        if (waitingStudents > 0) {
            // Help a student
            sem_post(&taSemaphore);
            pthread_mutex_unlock(&mutex);

            // Simulate helping a student
            printf("TA helping a student.\n");
            sleep(rand() % 5);
        } else {
            pthread_mutex_unlock(&mutex);
            // Take a nap
            printf("TA taking a nap.\n");
            sem_wait(&taSemaphore); // Wait to be woken up by a student
        }
    }
}

int main() {
    pthread_t students[NUM_STUDENTS], ta;
    pthread_mutex_init(&mutex, NULL);
    sem_init(&taSemaphore, 0, 1);

    // Initialize chair semaphores
    for (int i = 0; i < NUM_CHAIRS; i++) {
        sem_init(&chairsSemaphore[i], 0, 1);
    }

    pthread_create(&ta, NULL, taBehavior, NULL);
    for (long i = 0; i < NUM_STUDENTS; i++) {
        pthread_create(&students[i], NULL, studentBehavior, (void *)i);
    }

    pthread_join(ta, NULL);
    for (int i = 0; i < NUM_STUDENTS; i++) {
        pthread_join(students[i], NULL);
    }

    return 0;
}
