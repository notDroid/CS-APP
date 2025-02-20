/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"
#include <math.h>

#define k 16

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    int tmp[k][k], tmp_transpose[k][k];
    int i, j, u,v;

    // Slide a k x k temporary matrix along A
    // Transpose each matrix, which can be done fast since the small k x k matrix will fit into the cache
    // Copy the output of each transposed matrix to B

    for (i = 0; i < N/k; i += 1) {
        for (j = 0; j < M/k; j += 1) {
            // Copy from A, A -> tmp
            for (u = 0; u < k; u++) {
                for (v = 0; v < k; v++) {
                    tmp[u][v] = A[k*i + u][k*j + v];
                }
            }

            // Transpose, tmp -> tmp_transpose
            for (u = 0; u < k; u++) {
                for (v = 0; v < k; v++) {
                    tmp_transpose[v][u] = tmp[u][v];
                }
            }

            // Copy to B, tmp_transpose -> B
            for (v = 0; v < k; v++) {
                for (u = 0; u < k; u++) {
                    B[k*j + v][k*i + u] = tmp_transpose[v][u];
                }
            }
        }
    }

    // Deal with bottom and right sides if they don't fit perfectly into the k x k matrix
    if (k*i < N) {
        for (j = 0; j < M/k; j += 1) {
            // Copy from A, A -> tmp
            for (u = 0; u < N - i*k; u++) {
                for (v = 0; v < k; v++) {
                    tmp[u][v] = A[k*i + u][k*j + v];
                }
            }

            // Transpose, tmp -> tmp_transpose
            for (u = 0; u < N - i*k; u++) {
                for (v = 0; v < k; v++) {
                    tmp_transpose[v][u] = tmp[u][v];
                }
            }

            // Copy to B, tmp_transpose -> B
            for (v = 0; v < k; v++) {
                for (u = 0; u < N - i*k; u++) {
                    B[k*j + v][k*i + u] = tmp_transpose[v][u];
                }
            }
        }
    }

    if (k*j < M) {
        for (i = 0; i < N/k; i += 1) {
            // Copy from A, A -> tmp
            for (u = 0; u < k; u++) {
                for (v = 0; v < M - j*k; v++) {
                    tmp[u][v] = A[k*i + u][k*j + v];
                }
            }

            // Transpose, tmp -> tmp_transpose
            for (u = 0; u < k; u++) {
                for (v = 0; v < M - j*k; v++) {
                    tmp_transpose[v][u] = tmp[u][v];
                }
            }

            // Copy to B, tmp_transpose -> B
            for (v = 0; v < M - j*k; v++) {
                for (u = 0; u < k; u++) {
                    B[k*j + v][k*i + u] = tmp_transpose[v][u];
                }
            }
        }
    }

    // Bottom right edge
    if (k*j < M || k*i < N) {
        // Copy from A, A -> tmp
        for (u = 0; u < N - i*k; u++) {
            for (v = 0; v < M - j*k; v++) {
                tmp[u][v] = A[k*i + u][k*j + v];
            }
        }

        // Transpose, tmp -> tmp_transpose
        for (u = 0; u < N - i*k; u++) {
            for (v = 0; v < M - j*k; v++) {
                tmp_transpose[v][u] = tmp[u][v];
            }
        }

        // Copy to B, tmp_transpose -> B
        for (v = 0; v < M - j*k; v++) {
            for (u = 0; u < N - i*k; u++) {
                B[k*j + v][k*i + u] = tmp_transpose[v][u];
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

