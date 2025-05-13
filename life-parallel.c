#include "life.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

typedef struct {
    int numWork;                // Line available to compute, which must not less than 0
    int compleWork;             // Completed works
    int stop;                   // Flag to stop
    int ret;                    // Flag to return
    int threads;                // Number of threads
    pthread_mutex_t mutex;      // Mutex
    pthread_cond_t cv;          // Wait function
    LifeBoard* state;           // State to observe
    LifeBoard* next_state;      // New state
} META;


#define MAX_THREADS 20
META* metaData = NULL;


void* threadfunc(void* args){
    //
    //  Thread function, follow three steps
    //      1. Monitor tasks queue
    //      2. Obtain task if available else sleep
    //      3. Complete task
    //      4. Go back to step one
    //

    while(1){
        // First step: Monitor
        int mywork=-1;
        pthread_mutex_lock(&metaData->mutex);
        while((metaData->numWork<=0 || metaData->stop) && !metaData->ret){
            pthread_cond_wait(&metaData->cv, &metaData->mutex);
        }
        mywork = metaData->numWork--;
        pthread_mutex_unlock(&metaData->mutex);
        if(metaData->ret) break;


        // Finish my work
        LifeBoard* state = metaData->state;
        LifeBoard* next_state = metaData->next_state;

        for (int x = 1; x < state->width - 1; x++) {
            int live_neighbors = count_live_neighbors(state, x, mywork-1);
            LifeCell current_cell = at(state, x, mywork-1);
            LifeCell new_cell = (live_neighbors == 3) || (live_neighbors == 4 && current_cell == 1) ? 1 : 0;
            set_at(next_state, x, mywork-1, new_cell);
        }

        // Completed!
        pthread_mutex_lock(&metaData->mutex);
        metaData->compleWork++;
        if(metaData->compleWork==state->height){
            pthread_cond_broadcast(&metaData->cv);
        }
        pthread_mutex_unlock(&metaData->mutex);
    }

    return  NULL;
}

void create_pools(int threads, pthread_t* pools){
    //
    //  Create threads pools
    //
    
    for(int i=0; i<threads; i++){
        pthread_create(pools+i, NULL, threadfunc, NULL);
        // printf("Create Threads: %lu\n", pools[i]);
    }
    return ;
}

void destroy_metaData(META* metaData, pthread_t* pools){
	//
	//	Destroy metaData structure
	//
    metaData->ret = 1;
    pthread_cond_broadcast(&metaData->cv);

    void* result;
    for(int i=0; i<metaData->threads; i++){
        pthread_join(pools[i], &result);
    }
    printf("Free all threads in Pools!\n");
	pthread_cond_destroy(&metaData->cv);
	pthread_mutex_destroy(&metaData->mutex);
	return ;
}



void simulate_life_parallel(int threads, LifeBoard *state, int steps) {
    // 
    //  Divide work of caculation into $(threads) blocks in turns,
    //  run threads parallel
    //
    if(steps==0) return;

    // Init meta data
    metaData = (META*)malloc(sizeof(META));
    pthread_mutex_init(&metaData->mutex, NULL);
    pthread_cond_init(&metaData->cv, NULL);
    metaData->numWork = state->height;
    metaData->compleWork = 0;
    LifeBoard* next_state = create_life_board(state->width, state->height);
    metaData->next_state = next_state;
    metaData->state = state;
    metaData->stop = 1;
    metaData->ret = 0;
    metaData->threads = threads;


    // Create threads pool
    pthread_t pools[MAX_THREADS];
    create_pools(threads, pools);


    for(int time=0; time<steps; time++){
        // Begin
        pthread_mutex_lock(&metaData->mutex);
        metaData->stop = 0;
        pthread_mutex_unlock(&metaData->mutex);
        // Broadcast
        pthread_cond_broadcast(&metaData->cv);
        // Sleep
        pthread_mutex_lock(&metaData->mutex);
        while(metaData->compleWork!=state->height){
            pthread_cond_wait(&metaData->cv, &metaData->mutex);
        }

        metaData->stop = 1;
        swap(state, next_state);
        metaData->numWork = state->height;
        metaData->compleWork = 0;
        pthread_mutex_unlock(&metaData->mutex);
    }

    // Free
    destroy_life_board(next_state);
    destroy_metaData(metaData, pools);
}



