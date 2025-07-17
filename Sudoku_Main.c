#include "Sudoku_methods.c"

//SEQUENTIAL METHON
bool sequential_method(int **sudoku, int N) {
    is_sudoku_valid = true;
    
    //Check rows
    for (int i = 0; i < N; i++) {
        bool row_valid = check_row(sudoku, i);
        fprintf(o_fp, "Thread %d checks row %d and is %s.\n", i + 1, i+1, row_valid ? "valid" : "invalid"); //Ternary opreator to print
        if (!row_valid) {
            is_sudoku_valid = false;
            //return false; // Early termination 
        }
    }

    //Check columns
    for (int i = 0; i < N; i++) {
        bool col_valid = check_column(sudoku, i);
        fprintf(o_fp, "Thread %d checks column %d and is %s.\n", N + i + 1, i + 1, col_valid ? "valid" : "invalid"); //Ternary opreator to print
        if (!col_valid) {
            is_sudoku_valid = false;
            //return false;  // Early termination
        }
    }

    //Check subgrids
    int n = (int)sqrt(N);
    for (int i = 0; i < n * n; i++) {
        bool grid_valid = check_subgrid(sudoku, i);
        fprintf(o_fp, "Thread %d checks grid %d and is %s.\n", 2*N + i + 1, i + 1, grid_valid ? "valid" : "invalid"); //Ternary opreator to print
        if (!grid_valid) {
            is_sudoku_valid = false;
            //return false;  // Early termination 
        }
    }

    return is_sudoku_valid;
}


//CHUNK METHOD
void chunk_method(void *(*checks)(void *), int K, int N) {
    pthread_t threads[K];
    info_thread thread_data[K];
    int chunk_size = N / K;
    //int created_threads = 0;

    for (int i = 0; i < K; i++) {
        thread_data[i].index = i + 1;
        thread_data[i].first_pos = i * chunk_size;
        if (i == K - 1) {
           thread_data[i].last_pos = N - 1; 
        } else { 
           thread_data[i].last_pos = (i + 1) * chunk_size - 1;
        }   
        /*if (!is_sudoku_valid) {
             for (int j = 0; j < created_threads; j++) {
                 pthread_join(threads[j], NULL);
             }
             return; // Early termination
        }   
        if (pthread_create(&threads[i], NULL, checks, (void *)&thread_data[i]) == 0) {
           created_threads++;  // Track successfully created threads
        }*/
        pthread_create(&threads[i], NULL, checks, (void *)&thread_data[i]);
    }

    for (int i = 0; i < K; i++) {
        pthread_join(threads[i], NULL);
    }
}



//MIXED METHOD
void mixed_method(void *(*checks)(void *), int K) {
    pthread_t threads[K];
    info_thread thread_data[K];

    for (int i = 0; i < K; i++) {
        thread_data[i].index = i;
        thread_data[i].thread_limit = K;
        /*if (!is_sudoku_valid) {
             for (int j = 0; j < created_threads; j++) {
                 pthread_join(threads[j], NULL);
             }
             return; // Early termination
        }   
        if (pthread_create(&threads[i], NULL, checks, (void *)&thread_data[i]) == 0) {
           created_threads++;  // Track successfully created threads
        }*/
        pthread_create(&threads[i], NULL, checks, (void *)&thread_data[i]);
    }

    for (int i = 0; i < K; i++) {
        pthread_join(threads[i], NULL);
    }
}


int main() {
    FILE *i_fp = fopen("inp.txt", "r");
    o_fp = fopen("output.txt", "w");

    if (i_fp == NULL || o_fp == NULL) {
        printf("File don't exist.\n");
        return 1;
    }
    
    fscanf(i_fp, "%d %d", &K, &N);
    

    // Allocate memory for Sudoku
    sudoku = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        sudoku[i] = (int *)malloc(N * sizeof(int));
        for (int j = 0; j < N; j++) {
            fscanf(i_fp, "%d", &sudoku[i][j]);
        }
    }
    
    pthread_mutex_init(&lock, NULL);  //Initialize the mutex lock for synchronization
    //Starts calculating execution time
    clock_t start_time = clock();
    
    int choice;
    printf("\n1.chunk method\n2.Mixed method\n3.sequential method\n");
    printf("Enter the method you want to implement:  ");
    scanf("%d",&choice);
    
    switch (choice) {
          case 1:
                 chunk_method(check_rows_chunk, K, N);
                 //if (is_sudoku_valid) {
                    chunk_method(check_columns_chunk, K, N);
                 //}  
                 //if (is_sudoku_valid) {
                    chunk_method(check_subgrids_chunk, K, N);
                 //}  
                 break;
          case 2: 
                mixed_method(check_rows_mixed, K);
                //if (is_sudoku_valid) {
                   mixed_method(check_columns_mixed, K);
                //}   
                //if (is_sudoku_valid) {
                   mixed_method(check_subgrids_mixed, K);
                //}   
                break;
          case 3:   
                is_sudoku_valid = sequential_method(sudoku, N);
                break;
          default:
                 printf("Invalid choice!!\n");
    }            
                
    fprintf(o_fp, "\nSudoku is %s.\n", is_sudoku_valid ? "valid" : "invalid"); //Ternary opreator to print
     
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000000; // Microseconds

    switch (choice) {
          case 1:
                 fprintf(o_fp, "The total time taken is %.2f microseconds for chunk method execution\n", time_taken);
                 break;
          case 2: 
                fprintf(o_fp, "The total time taken is %.2f microseconds for mixed method execution\n", time_taken);
                break;
          case 3:   
                fprintf(o_fp, "The total time taken is %.2f microseconds for sequential method execution\n", time_taken);
                break;
          default:
                 printf("Invalid choice!!!\n");
    }         
    
    fclose(i_fp);
    fclose(o_fp);

    //Free allocated memory
    for (int i = 0; i < N; i++) {
        free(sudoku[i]);
    }
    free(sudoku);
    
    pthread_mutex_destroy(&lock);  //Destroy mutex lock
    return 0;
}
