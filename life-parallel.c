#include "life.h"
#include <pthread.h>
#include <semaphore.h>


typedef struct {
    int numWork;                // Line available to compute, which must not less than 0
    int compleWork;             // Completed works
    int stop;                   // Flag to stop
    int ret;                    // Flag to return
    int threads;                // Number of threads
    pthread_mutex_t mutex;      // Mutex
    pthread_cond_t worker_cv;   // Worker condition variable
    pthread_cond_t main_cv;     // Main thread condition variable
    LifeBoard* state;           // State to observe
    LifeBoard* next_state;      // New state
} META;


#define MAX_THREADS 100
META* metaData = NULL;

//
// Threads Pool Version
//
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
            pthread_cond_wait(&metaData->worker_cv, &metaData->mutex);
        }
        mywork = metaData->numWork--;
        pthread_mutex_unlock(&metaData->mutex);
        if(metaData->ret) break;

        // Finish my work
        LifeBoard* state = metaData->state;
        LifeBoard* next_state = metaData->next_state;

        for(int x=1; x<state->width-1; x++){
            int live_neighbors = count_live_neighbors(state, x, mywork);
            LifeCell current_cell = at(state, x, mywork);
            LifeCell new_cell = (live_neighbors == 3) || (live_neighbors == 4 && current_cell == 1) ? 1 : 0;
            set_at(next_state, x, mywork, new_cell);
        }

        // Completed!
        pthread_mutex_lock(&metaData->mutex);
        metaData->compleWork ++;
        if(metaData->compleWork==state->height-2){
            pthread_cond_broadcast(&metaData->main_cv);
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
    }
    return ;
}

void destroy_metaData(META* metaData, pthread_t* pools){
	//
	//	Destroy metaData structure
	//
    metaData->ret = 1;
    pthread_cond_broadcast(&metaData->worker_cv);

    void* result;
    for(int i=0; i<metaData->threads; i++){
        pthread_join(pools[i], &result);
    }
    // printf("Free all threads in Pools!\n");
	pthread_cond_destroy(&metaData->worker_cv);
    pthread_cond_destroy(&metaData->main_cv);
	pthread_mutex_destroy(&metaData->mutex);
	return ;
}

//
//  Traditional Version
//

typedef struct{
    int index;
    int num;
    LifeBoard* state;
    LifeBoard* next_state;
} MD;

void* func(void* args)
{
    MD* metaData = (MD*)args;

    for(int y=metaData->index; y<metaData->state->height-1; y+=metaData->num){
        for(int x=1; x<metaData->state->width-1; x++){
            int live_neighbors = count_live_neighbors(metaData->state, x, y);
            LifeCell current_cell = at(metaData->state, x, y);
            LifeCell new_cell = (live_neighbors == 3) || (live_neighbors == 4 && current_cell == 1) ? 1 : 0;
            set_at(metaData->next_state, x, y, new_cell);
        }
    }

    return (void*)0;


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
    pthread_cond_init(&metaData->worker_cv, NULL);
    pthread_cond_init(&metaData->main_cv, NULL);
    metaData->numWork = state->height-2;
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
        pthread_cond_broadcast(&metaData->worker_cv);
        
        while(metaData->compleWork!=state->height-2){
            pthread_cond_wait(&metaData->main_cv, &metaData->mutex);
        }

        metaData->stop = 1;
        swap(state, next_state);
        metaData->numWork = state->height-2;
        metaData->compleWork = 0;
        pthread_mutex_unlock(&metaData->mutex);
    }

    // Free
    destroy_metaData(metaData, pools);
    destroy_life_board(next_state);
    

    // LifeBoard* next_state = create_life_board(state->width, state->height);
    // pthread_t t[MAX_THREADS];
    // MD* metaData[MAX_THREADS];
    // for(int i=0; i<threads; i++){
    //     metaData[i] = (MD*)malloc(sizeof(MD));
    //     metaData[i]->index = i+1;
    //     metaData[i]->num = threads;
    //     metaData[i]->state = state;
    //     metaData[i]->next_state = next_state;
    // }

    // for(int _=0; _<steps; _++){
    //     for(int i=0; i<threads; i++){
    //         pthread_create(t+i, NULL, func, (void*)(metaData[i]));
    //     }

    //     void* result;
    //     for(int i=0; i<threads; i++){
    //         pthread_join(t[i], &result);
    //     }

    //     swap(state, next_state);

    // }

    // for(int i=0; i<threads; i++){
    //     free(metaData[i]);       
    // }
    // destroy_life_board(next_state);
    
    
    return;
}



