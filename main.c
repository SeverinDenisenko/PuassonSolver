#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define I 1000
#define Q 10

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

    for (int i = 0; i < I; i++)
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

        if (i % Q == 0)
        {
            //                   ____  __.---""---.__  ____
            //                  /####\/              \/####\
            //                 (   /\ )              ( /\   )
            //                 \____/                \____/
            //                __/                          \__
            //             .-"    .                      .    "-.
            //             |  |   \.._                _../   |  |
            //              \  \    \."-.__________.-"./    /  /
            //                \  \    "--.________.--"    /  /
            //              ___\  \_                    _/  /___
            //            ./    )))))                  (((((    \.
            //            \                                      /
            //             \           \_          _/           /
            //               \    \____/""-.____.-""\____/    /
            //                 \    \                  /    /
            //                  \.  .|                |.  ./
            //                ." / |  \              /  | \  ".
            //             ."  /   |   \            /   |   \   ".
            //            /.-./.--.|.--.\          /.--.|.--.\.-.|

            char filename[1000];
            sprintf(filename, "DATA/result%03d.dat", i / Q);

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
            printf("file %03d generated.\n",  i / Q);
        }
    }

    return 0;
}
