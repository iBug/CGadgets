#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool *A, *b, *b0, *x;
int n, n2;

int main() {
    // Read input
    scanf("%d", &n);
    n2 = n * n;
    A = malloc(n2 * n2 * sizeof(*A));
    b = malloc(3 * n2 * sizeof(*b));
    b0 = b + 1 * n2;
    x = b + 2 * n2;
    memset(A, 0, n2 * n2 * sizeof(*A));
    memset(b, 0, 3 * n2 * sizeof(*b));

    for (int i = 0, t; i < n2; i++) {
        scanf("%d", &t);
        b[i] = b0[i] = (t == 0);
    }

    // Populate A
    for (int i = 0; i < n2; i++)
        A[i * n2 + i] = true;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - 1; j++) {
            A[(i * n + j) * n2 + (i * n + j + 1)] = true;
            A[(i * n + j) + n2 * (i * n + j + 1)] = true;
            A[(i + n * j) * n2 + (i + n * j + n)] = true;
            A[(i + n * j) + n2 * (i + n * j + n)] = true;
        }
    }

    // Gaussian elimination
    for (int i = 0, j, r = 0; i < n2 - 1; i++) {
        for (j = r; j < n2; j++)
            if (A[j * n2 + i])
                break;
        if (j == n2)
            continue;
        if (j != r) {
            bool t;
            for (int k = 0; k < n2; k++) {
                t = A[r * n2 + k];
                A[r * n2 + k] = A[j * n2 + k];
                A[j * n2 + k] = t;
            }
            t = b[r];
            b[r] = b[j];
            b[j] = t;
        }
        for (j = r + 1; j < n2; j++) {
            if (!A[j * n2 + i])
                continue;
            for (int k = i; k < n2; k++) {
                A[j * n2 + k] ^= A[r * n2 + k];
            }
            b[j] ^= b[r];
        }
        r++;
    }

#ifdef DEBUG
    for (int i = 0; i < n2; i++) {
        for (int j = 0; j < n2; j++)
            printf("%c ", "-o"[A[i * n2 + j]]);
        printf("  %c\n", "-o"[b[i]]);
    }
#endif

    // Back substitution
    for (int i = n2 - 1; i >= 0; i--) {
        bool sum = b[i];
        for (int j = n2 - 1; j > i; j--)
            sum ^= x[j] & A[i * n2 + j];
        x[i] = sum;
    }

    // Output
    char c, *s;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            c = b0[i * n + j] ? '-' : 'o';
            s = x[i * n + j] ? "[%c]" : " %c ";
            printf(s, c);
        }
        puts("");
    }

    free(A);
    free(b);
    return 0;
}
