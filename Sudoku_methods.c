#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>


//Global variables
int N;                 //Dimension of Sudoku (N x N)
int K;                 // No.of threads
int **sudoku;        
bool is_sudoku_valid = true; 
FILE *o_fp;    


//Validation of Row, Column and Subgrid for SEQUENTIAL METHOD 
bool check_row(int **sudoku, int row) {
    bool check_num[N + 1];
    for (int i = 1; i <= N; i++) {
        check_num[i] = false;
    }
    for (int i = 0; i < N; i++) {
        int num = sudoku[row][i];
        if (num < 1 || num > N || check_num[num]) {
            return false;
        }
        check_num[num] = true;
    }
    return true;
}

bool check_column(int **sudoku, int col) {
    bool check_num[N + 1];
    for (int i = 1; i <= N; i++) {
        check_num[i] = false;
    }
    for (int i = 0; i < N; i++) {
        int num = sudoku[i][col];
        if (num < 1 || num > N || check_num[num]) {
            return false;
        }
        check_num[num] = true;
    }
    return true;
}

bool check_subgrid(int **sudoku, int idx) {
    int n = (int)sqrt(N);
    int row_start = (idx / n) * n;
    int col_start = (idx % n) * n;
    
    bool check_num[N + 1];
    for (int i = 1; i <= N; i++) {
        check_num[i] = false;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int num = sudoku[row_start + i][col_start + j];
            if (num < 1 || num > N || check_num[num]) {
                return false;
            }
            check_num[num] = true;
        }
    }
    return true;
}



//Structure to pass information to thread
typedef struct {
    int thread_limit,index;   
    int first_pos;   
    int last_pos;     
} info_thread;
pthread_mutex_t lock;  //Mutex lock for thread safety and synchronization

//Validation of Row, Column and Subgrid for CHUNK METHOD 
void *check_rows_chunk(void *arg) {
    info_thread *data = (info_thread *)arg;
    for (int i = data->first_pos; i <= data->last_pos; i++) {
        bool local_valid = true;
        /*if (is_sudoku_valid == false) {
            pthread_exit(NULL);
        } */   
        bool check_num[N + 1];
        for (int j = 0; j <= N; j++) {
            check_num[j] = false;
        }    
        for (int j = 0; j < N; j++) {
            int num = sudoku[i][j];
            if (num < 1 || num > N || check_num[num]) { 
                pthread_mutex_lock(&lock); 
                is_sudoku_valid = false;
                local_valid = false;
                pthread_mutex_unlock(&lock);
            }
            check_num[num] = true;
        }
        pthread_mutex_lock(&lock);  //Lock the mutex to safely modify global variables
        fprintf(o_fp, "Thread %d checks row %d and is %s.\n", data->index, i + 1, local_valid ? "valid" : "invalid"); //Ternary opreator to print
        pthread_mutex_unlock(&lock);  //Unlock the mutex
    }
    pthread_exit(NULL);
}

void *check_columns_chunk(void *arg) {
    info_thread *data = (info_thread *)arg;
    for (int i = data->first_pos; i <= data->last_pos; i++) {
        bool local_valid = true;
        /*if (is_sudoku_valid == false) {
            pthread_exit(NULL);
        } */  
        bool check_num[N + 1];
        for (int j = 0; j <= N; j++) {
            check_num[j] = false;
        }
        for (int j = 0; j < N; j++) {
            int num = sudoku[j][i];
            if (num < 1 || num > N || check_num[num]) {
                pthread_mutex_lock(&lock); 
                is_sudoku_valid = false;
                local_valid = false;
                pthread_mutex_unlock(&lock);
            }
            check_num[num] = true;
        }
        pthread_mutex_lock(&lock);
        fprintf(o_fp, "Thread %d checks column %d and is %s.\n", data->index, i + 1, local_valid ? "valid" : "invalid"); //Ternary opreator to print
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}

void *check_subgrids_chunk(void *arg) {
    info_thread *data = (info_thread *)arg;
    int n = (int)sqrt(N); // Dimension of each subgrid
    for (int idx = data->first_pos; idx <= data->last_pos; idx++) {
        bool local_valid = true;
        /*if (is_sudoku_valid == false) {
            pthread_exit(NULL);
        } */
        int row_start = (idx / n) * n;
        int col_start = (idx % n) * n;
        bool check_num[N + 1];
        for (int j = 0; j <= N; j++) {
            check_num[j] = false;
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                int num = sudoku[row_start + i][col_start + j];
                if (num < 1 || num > N || check_num[num]) {
                    pthread_mutex_lock(&lock); 
                    is_sudoku_valid = false;
                    local_valid = false;
                    pthread_mutex_unlock(&lock);
                }
                check_num[num] = true;
            }
        }
        pthread_mutex_lock(&lock);
        fprintf(o_fp, "Thread %d checks grid %d and is %s.\n", data->index, idx + 1, local_valid ? "valid" : "invalid"); //Ternary opreator to print
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}




//Validation of Row, Column and Subgrid for MIXED METHOD 
void *check_rows_mixed(void *arg) {
    info_thread *data = (info_thread *)arg;
    for (int i = data->index; i < N; i += data->thread_limit) {
         bool local_valid = true;
         /*if (is_sudoku_valid == false) {
            pthread_exit(NULL);
        } */   
        bool check_num[N + 1];
        for (int j = 0; j <= N; j++) {
            check_num[j] = false;
        }   
        for (int j = 0; j < N; j++) {
            int num = sudoku[i][j];
            if (num < 1 || num > N || check_num[num]) {
                pthread_mutex_lock(&lock); 
                is_sudoku_valid = false;
                local_valid = false;
                pthread_mutex_unlock(&lock);
            }
            check_num[num] = true;
        }
        pthread_mutex_lock(&lock);  
        fprintf(o_fp, "Thread %d checks row %d and is %s.\n", data->index + 1, i + 1, local_valid ? "valid" : "invalid"); //Ternary opreator to print
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}

void *check_columns_mixed(void *arg) {
    info_thread *data = (info_thread *)arg;
    for (int i = data->index; i < N; i += data->thread_limit) {
        bool local_valid = true;
        /*if (is_sudoku_valid == false) {
            pthread_exit(NULL);
        } */   
        bool check_num[N + 1];
        for (int j = 0; j <= N; j++) {
            check_num[j] = false;
        }    
        for (int j = 0; j < N; j++) {
            int num = sudoku[j][i];
            if (num < 1 || num > N || check_num[num]) {
                pthread_mutex_lock(&lock); 
                is_sudoku_valid = false;
                local_valid = false;
                pthread_mutex_unlock(&lock);
            }
            check_num[num] = true;
        }
        pthread_mutex_lock(&lock);
        fprintf(o_fp, "Thread %d checks column %d and is %s.\n", data->index + 1, i + 1, local_valid ? "valid" : "invalid"); //Ternary opreator to print
        pthread_mutex_unlock(&lock);
    }   
    pthread_exit(NULL);
}

void *check_subgrids_mixed(void *arg) {
    info_thread *data = (info_thread *)arg;
    int n = (int)sqrt(N); // Dimension of each subgrid
    for (int idx = data->index; idx < N; idx += data->thread_limit) {
         bool local_valid = true;
         /*if (is_sudoku_valid == false) {
            pthread_exit(NULL);
        } */   
        int row_start = (idx / n) * n;
        int col_start = (idx % n) * n;
        bool check_num[N + 1];
        for (int j = 0; j <= N; j++) {
            check_num[j] = false;
        }    
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                int num = sudoku[row_start + i][col_start + j];
                if (num < 1 || num > N || check_num[num]) {
                    pthread_mutex_lock(&lock); 
                    is_sudoku_valid = false;
                    local_valid = false;
                    pthread_mutex_unlock(&lock);
                }
                check_num[num] = true;
            }
        }
        pthread_mutex_lock(&lock);
        fprintf(o_fp, "Thread %d grid %d and is %s.\n", data->index + 1, idx + 1, local_valid ? "valid" : "invalid"); //Ternary opreator to print
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}



