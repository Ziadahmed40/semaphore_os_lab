#include "pthread.h"
#include "stdio.h"
#include "malloc.h"
#include <time.h>
struct row_thread_argg{
    int *row;
    int r_matrix;
    int c_matrix;
    int** matrix;
    int *result;
};
struct element_thread_argg{
    int *row;
    int iterator;
    int** matrix;
    int column_num;
    int *result;
};
void *compute_basic_element(void* arguments){
    struct element_thread_argg *args = arguments;
    int sum =0;
        for (int i = 0; i < args->iterator; i++) {
            sum+=args->row[i]*args->matrix[i][args->column_num];
        }
        args->result[args->column_num]=sum;
    return NULL;
}
void *compute_element_by_element(void* arguments){
    struct row_thread_argg *args = arguments;
    pthread_t threads[args->c_matrix];
    struct element_thread_argg elem[args->c_matrix];
    for (int i = 0; i < args->c_matrix; i++) {
        elem[i].matrix=args->matrix;
        elem[i].iterator=args->r_matrix;
        elem[i].row=args->row;
        elem[i].column_num=i;
        elem[i].result=args->result;
        pthread_create(&threads[i],NULL,compute_basic_element,(void *)&elem[i]);
    }
    for (int i = 0; i < args->c_matrix; i++) {
        pthread_join(threads[i],NULL);
    }
    return NULL;
}
void *compute_row_thread(void* arguments){
    struct row_thread_argg *args = arguments;
    for (int i = 0; i < args->c_matrix; i++) {
        int sum =0;
        for (int j = 0; j < args->r_matrix; j++) {
            sum+=args->row[j]*args->matrix[j][i];
        }
        args->result[i]=sum;
    }
    return NULL;
}
int main() {
    FILE *file_ptr;
    file_ptr = fopen("C:\\Users\\zeyad\\Desktop\\untitled8\\input3-matrix.txt", "r");
    if (file_ptr == NULL) {
        fprintf(stderr, "Error opening file\n");
           return 1;
    }
    int r1,c1,r2,c2;
    fscanf(file_ptr, "%d", &r1);
    fscanf(file_ptr, "%d", &c1);
    int matrix1[r1][c1];
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) {
            fscanf(file_ptr, "%d",&matrix1[i][j]) ;
        }
    }
    printf("\n");
    fscanf(file_ptr, "%d", &r2);
    if (c1!=r2){
        fprintf(stderr, "Cannot compute the matrix multiplication becouse of diffrent size\n");
        return 1;
    }
    fscanf(file_ptr, "%d", &c2);
    int* matrix2[r2];
    for (int i = 0; i < r2; i++)
        matrix2[i] = (int*)malloc(c2 * sizeof(int));
    for (int i = 0; i < r2; i++) {
        for (int j = 0; j < c2; j++) {
            fscanf(file_ptr, "%d",&matrix2[i][j]) ;
        }
    }
    fclose(file_ptr);
    pthread_t threads[r1];
    struct row_thread_argg r[r1];
    int result_matrix[r1][c2];
    clock_t t;
    t = clock();
    for (int i = 0; i < r1; i++) {
        r[i].r_matrix=r2;
        r[i].c_matrix=c2;
        r[i].row=matrix1[i];
        r[i].result=result_matrix[i];
        r[i].matrix=matrix2;
        pthread_create(&threads[i],NULL,compute_row_thread,(void *)&r[i]);
    }
    for (int i = 0; i < r1; i++) {
        pthread_join(threads[i],NULL);
    }
    printf("results by row:\n");
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            printf("%d ",result_matrix[i][j]);
        }
        printf("\n");
    }
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
    printf("took %f seconds to execute \n\n", time_taken);
    pthread_t threads_elemnt[r1];
    struct row_thread_argg r_element[r1];
    int result_matrix_element[r1][c2];
    clock_t t1;
    t1 = clock();
    for (int i = 0; i < r1; i++) {
        r_element[i].r_matrix=r2;
        r_element[i].c_matrix=c2;
        r_element[i].row=matrix1[i];
        r_element[i].result=result_matrix_element[i];
        r_element[i].matrix=matrix2;
        pthread_create(&threads_elemnt[i],NULL,compute_element_by_element,(void *)&r_element[i]);
    }
    for (int i = 0; i < r1; i++) {
        pthread_join(threads_elemnt[i],NULL);
    }
    printf("results by element :\n");
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            printf("%d ",result_matrix_element[i][j]);
        }
        printf("\n");
    }
    t1 = clock() - t1;
    double time_taken1 = ((double)t1)/CLOCKS_PER_SEC; // in seconds

    printf("took %f seconds to execute \n", time_taken1);
    return 0;
}
