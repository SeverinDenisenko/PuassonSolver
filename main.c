#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ITERATION 1000
#define OUTPUT_FREQUENCY 10

int main(int argc, char **argv)
{
    // Ввод
    int n;

    FILE *data = fopen("data.dat", "r");
    if (data == NULL)
    {
        printf("Error: can't find file\n");
    }

    char a = getc(data);
    if (a != '#')
    {
        printf("Error: wrong file format\n");
    }

    fscanf(data, "%i", &n);
    if (n % 2 != 0)
    {
        printf("Error: wrong file dimentions (n must be even).\n");
    }

    double **h_sqare_ro = (double **)malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++)
    {
        h_sqare_ro[i] = (double *)malloc(n * sizeof(double));
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fscanf(data, "%lf", &h_sqare_ro[i][j]);
        }
    }

    fclose(data);

    // Вычисления

    double **phi1 = (double **)malloc(n * sizeof(double *));
    double **phi2 = (double **)malloc(n * sizeof(double *));

    for (int i = 0; i < n; i++)
    {
        phi1[i] = (double *)malloc(n * sizeof(double));
        phi2[i] = (double *)malloc(n * sizeof(double));
    }

    for (int i = 0; i < ITERATION; i++)
    {
        for (int k = 1; k < n - 1; k++)
        {
            for (int l = 1; l < n - 1; l++)
            {
                phi2[k][l] = (phi1[k][l - 1] + phi1[k - 1][l] + phi1[k + 1][l] + phi1[k][l + 1] + h_sqare_ro[k][l]) / 4;
            }
        }

        double **tmp = phi1;
        phi1 = phi2;
        phi2 = tmp;

        if (i % OUTPUT_FREQUENCY == 0)
        {

            //   \|/ ____ \|/
            //    @~/ ,. \~@
            //   /_( \__/ )_\       
            //      \__U_/

            char filename[2000];
            char str[500];
            filename[0] = '\0';
            sprintf(str, "DATA/result");
            strcat(filename, str);
            snprintf(str, 5, "%03d", i / OUTPUT_FREQUENCY);
            strcat(filename, str);
            sprintf(str, ".dat");
            strcat(filename, str);

            FILE *result = fopen(filename, "w");

            fprintf(result, "# %d\n", n);

            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    fprintf(result, "%f ", phi1[i][j]);
                }

                fprintf(result, "\n");
            }

            fclose(result);
            printf("file %03d generated.\n",  i / OUTPUT_FREQUENCY);
        }
    }

    return 0;
}
