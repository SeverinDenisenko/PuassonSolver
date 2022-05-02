#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define I 1000
#define Q 100

int zero_process();
int secondary_processes(int process_Rank);

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int process_Rank, size_Of_Cluster;

    MPI_Comm_size(MPI_COMM_WORLD, &size_Of_Cluster);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_Rank);

    if (process_Rank == 0)
    {
        zero_process();
    }
    else
    {
        secondary_processes(process_Rank);
    }

    MPI_Finalize();
    return 0;
}

int zero_process()
{
    // Чтение исходных данных
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

    // Отправка данных

    MPI_Ssend(&n, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    MPI_Ssend(&n, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
    MPI_Ssend(&n, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
    MPI_Ssend(&n, 1, MPI_INT, 4, 0, MPI_COMM_WORLD);

    for (int i = 0; i < n / 2; i++)
    {
        MPI_Ssend(h_sqare_ro[i], n / 2, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
        MPI_Ssend(h_sqare_ro[i + n / 2], n / 2, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD);
    }
    for (int i = 0; i < n / 2; i++)
    {
        MPI_Ssend(h_sqare_ro[i] + n / 2, n / 2, MPI_DOUBLE, 3, 0, MPI_COMM_WORLD);
        MPI_Ssend(h_sqare_ro[i + n / 2] + n / 2, n / 2, MPI_DOUBLE, 4, 0, MPI_COMM_WORLD);
    }

    // Получение данных, вывод

    double **phi = (double **)malloc(n * sizeof(double *));

    for (int i = 0; i < n; i++)
    {
        phi[i] = (double *)malloc(n * sizeof(double));
    }

    for (int i = 0; i < I / Q; i++)
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
        sprintf(filename, "DATA/result%03d.dat", i);

        MPI_Status status;
        for (int k = 0; k < n / 2; k++)
        {
            MPI_Recv(phi[k], n / 2, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(phi[k + n / 2], n / 2, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD, &status);
        }
        for (int k = 0; k < n / 2; k++)
        {
            MPI_Recv(phi[k] + n / 2, n / 2, MPI_DOUBLE, 3, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(phi[k + n / 2] + n / 2, n / 2, MPI_DOUBLE, 4, 0, MPI_COMM_WORLD, &status);
        }

        FILE *result = fopen(filename, "w");

        fprintf(result, "# %d\n", n);

        for (int k = 0; k < n; k++)
        {
            for (int l = 0; l < n; l++)
            {
                fprintf(result, "%f ", phi[k][l]);
            }

            fprintf(result, "\n");
        }

        fclose(result);
        printf("file %03d generated.\n", i);
    }

    return 0;
}

int secondary_processes(int process_Rank)
{
    int n;
    MPI_Status status;
    MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    // Место под phi, h_sqare_ro
    double **phi1 = (double **)malloc((n / 2 + 1) * sizeof(double *));
    double **phi2 = (double **)malloc((n / 2 + 1) * sizeof(double *));

    for (int i = 0; i < n; i++)
    {
        phi1[i] = (double *)malloc((n / 2 + 1) * sizeof(double));
        phi2[i] = (double *)malloc((n / 2 + 1) * sizeof(double));
    }

    double **h_sqare_ro = (double **)malloc((n / 2) * sizeof(double *));
    for (int i = 0; i < n; i++)
    {
        h_sqare_ro[i] = (double *)malloc((n / 2) * sizeof(double));
    }

    // Получение исходных данных

    for (int i = 0; i < n / 2; i++)
    {
        MPI_Recv(h_sqare_ro[i], n / 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
    }

    // Вычисления

    for (int i = 0; i < I; i++)
    {
        // Обмен граничными значениями

        double *send1 = (double *)malloc((n / 2) * sizeof(double));
        double *send2 = (double *)malloc((n / 2) * sizeof(double));
        double *get1 = (double *)malloc((n / 2) * sizeof(double));
        double *get2 = (double *)malloc((n / 2) * sizeof(double));

        if (process_Rank == 1)
        {
            for (int k = 0; k < n / 2; k++)
            {
                send1[k] = phi1[n / 2 - 1][k];
                send2[k] = phi1[k][n / 2 - 1];
            }

            MPI_Ssend(send1, n / 2, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD);
            MPI_Recv(get1, n / 2, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD, &status);
            MPI_Ssend(send2, n / 2, MPI_DOUBLE, 3, 0, MPI_COMM_WORLD);
            MPI_Recv(get2, n / 2, MPI_DOUBLE, 3, 0, MPI_COMM_WORLD, &status);

            for (int k = 0; k < n / 2; k++)
            {
                phi1[n / 2][k] = get1[k];
                phi1[k][n / 2] = get2[k];
            }
        }
        else if (process_Rank == 2)
        {
            for (int k = 0; k < n / 2; k++)
            {
                send1[k] = phi1[k][1];
                send2[k] = phi1[n / 2 - 1][k];
            }

            MPI_Recv(get1, n / 2, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);
            MPI_Ssend(send1, n / 2, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(get2, n / 2, MPI_DOUBLE, 4, 0, MPI_COMM_WORLD, &status);
            MPI_Ssend(send2, n / 2, MPI_DOUBLE, 4, 0, MPI_COMM_WORLD);

            for (int k = 0; k < n / 2; k++)
            {
                phi1[k][0] = get1[k];
                phi1[n / 2][k] = get2[k];
            }
        }
        else if (process_Rank == 3)
        {
            for (int k = 0; k < n / 2; k++)
            {
                send1[k] = phi1[1][k];
                send2[k] = phi1[k][n / 2 - 1];
            }

            MPI_Recv(get1, n / 2, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);
            MPI_Ssend(send1, n / 2, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
            MPI_Ssend(send2, n / 2, MPI_DOUBLE, 4, 0, MPI_COMM_WORLD);
            MPI_Recv(get2, n / 2, MPI_DOUBLE, 4, 0, MPI_COMM_WORLD, &status);

            for (int k = 0; k < n / 2; k++)
            {
                phi1[0][k] = get1[k];
                phi1[k][n / 2] = get2[k];
            }
        }
        else if (process_Rank == 4)
        {
            for (int k = 0; k < n / 2; k++)
            {
                send1[k] = phi1[1][k];
                send2[k] = phi1[k][1];
            }

            MPI_Ssend(send1, n / 2, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD);
            MPI_Recv(get1, n / 2, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(get2, n / 2, MPI_DOUBLE, 3, 0, MPI_COMM_WORLD, &status);
            MPI_Ssend(send2, n / 2, MPI_DOUBLE, 3, 0, MPI_COMM_WORLD);

            for (int k = 0; k < n / 2; k++)
            {
                phi1[0][k] = get1[k];
                phi1[k][0] = get2[k];
            }
        }

        for (int k = 1; k < n / 2; k++)
        {
            for (int l = 1; l < n / 2; l++)
            {
                phi2[k][l] = (phi1[k][l - 1] + phi1[k - 1][l] + phi1[k + 1][l] + phi1[k][l + 1] + h_sqare_ro[k][l]) / 4;
            }
        }

        double **tmp = phi1;
        phi1 = phi2;
        phi2 = tmp;

        if (i % Q == 0)
        {
            for (int k = 0; k < n / 2; k++)
            {
                MPI_Ssend(phi1[k], n / 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            }
        }
    }

    return 0;
}
