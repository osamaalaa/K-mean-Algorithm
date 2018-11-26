
#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <chrono>
#include <omp.h>
#include <fstream>
#include <random>

using namespace std;

void sequential_kmean(int N, double* inputArray, int K, double* KmeanArray)
{
	double *PrevKmeanValues = new double[K]();
	double **DifferenceArray = new double *[K]();
	double **clusterGroups = new double *[K]();

	for (int i = 0; i < K; i++)
		DifferenceArray[i] = new double[N]();


	for (int i = 0; i < K; i++)
		clusterGroups[i] = new double[N]();

	int iterationNumber = 0, clusterNumber = 0;
	double clusterLength = 0, valueInCurrentCluster = 0, sum = 0, minimumMean = 0;

	int stop = 1;
	const clock_t begin_time = clock();
	while (iterationNumber < 19)
	{
		if (stop == 1)
		{
			for (int i = 0; i < K; i++)
			{
				for (int j = 0; j < N; j++) //minus each cluster mn el array elly m3aya w bashuf a2rab wa7ed leha 
				{
					DifferenceArray[i][j] = abs(KmeanArray[i] - inputArray[j]);
				}
			}


			for (int i = 0; i < N; i++)
			{
				minimumMean = 100000;
				for (int j = 0; j < K; j++)
				{
					if (DifferenceArray[j][i] < minimumMean)
					{
						minimumMean = DifferenceArray[j][i];
						valueInCurrentCluster = inputArray[i];
						clusterNumber = j;
					}
				}
				clusterGroups[clusterNumber][i] = valueInCurrentCluster;
			}

				for (int i = 0; i < K; i++)
				{
					sum = 0;
					clusterLength = 0;

					for (int j = 0; j < N; j++)
					{
						if (clusterGroups[i][j] != NULL)
						{
							sum += clusterGroups[i][j];
							clusterLength++;
						}

					}

					PrevKmeanValues[i] = KmeanArray[i];

					if (clusterLength!=0)
						KmeanArray[i] = sum / clusterLength;

					
				}


				//empty the clustersGroups array as we need it empty in the next iteration
				for (int i = 0; i < K; i++)
				{
					for (int j = 0; j < N; j++)
					{
						clusterGroups[i][j] = NULL;
					}
				}

				//termination check

				stop = 0;
				for (int i = 0; i < K; i++)
				{
					if (PrevKmeanValues[i] != KmeanArray[i])
						stop = 1;
				}
			}

		else
			break;

		iterationNumber++;
	}

	cout << "results: " << endl;

	for (int i = 0; i < K; i++)
		cout << KmeanArray[i] << endl;

	cout << "time= " << float(clock() - begin_time) / CLOCKS_PER_SEC << endl;

	delete[] PrevKmeanValues;
	for (int i = 0; i < K; i++)
	{
		delete[]DifferenceArray[i];
	}
	delete[] DifferenceArray;

	for (int i = 0; i < K; i++)
	{
		delete[]clusterGroups[i];
	}
	delete[] clusterGroups;
	
}
void parallel_kmean(int N, double* inputArray, int K, double* KmeanArray)
{
	int clusterNumber, i, j, x, y, q, w;
	double  iterationNumber = 0, clusterCounter = 0, minimumMean=0, valueInCurrentCluster, sum = 0;

	double *PrevKmeanValues = new double[K]();
	double **DifferenceArray = new double *[K]();
	double **clusterGroups = new double *[K]();

	for (int i = 0; i < K; i++)
		DifferenceArray[i] = new double[N]();

	for (int i = 0; i <K; i++)
     clusterGroups[i] = new double[N]();

	int stop = 1;
	const clock_t begin_time2 = clock();
	while (iterationNumber < 19)
	{
		if (stop == 1)
		{
			omp_set_num_threads(10);
           #pragma omp parallel private (i,j,x,y,q,w) shared (K, N,sum,KmeanArray,DifferenceArray,minimumMean,clusterNumber)
			{

                 #pragma omp for ordered nowait schedule(guided)
                   for (i = 0; i < K; i++)
					{
						for (j = 0; j < N; j++)  
						{
							DifferenceArray[i][j] = abs(KmeanArray[i] - inputArray[j]);
						}
					}
			


                #pragma omp single
				{
			     	for (x = 0; x < N; x++)
				    {
					   minimumMean = 100000;
					   for (y = 0; y < K; y++)
					   {
						if (DifferenceArray[y][x] < minimumMean)
						{
							minimumMean = DifferenceArray[y][x];
							valueInCurrentCluster = inputArray[x];
							clusterNumber = y;
						}
					}
					clusterGroups[clusterNumber][x] = valueInCurrentCluster;
				  }
		    	}
		
                #pragma omp single
				{

					for (int i = 0; i < K; i++)
					{
						sum = 0;
						clusterCounter = 0;
						for (int j = 0; j < N; j++)
						{
							if (clusterGroups[i][j] != NULL)
							{
								sum += clusterGroups[i][j];
								clusterCounter++;
							}
						}
					

						PrevKmeanValues[i] = KmeanArray[i];

						if (clusterCounter != 0)
							KmeanArray[i] = sum / clusterCounter;

					}
				}

                  #pragma omp for nowait schedule(guided)
					for (int i = 0; i < K; i++)
					{
						for (int j = 0; j < N; j++)
						{
							clusterGroups[i][j] = NULL;
						}
					}
				

				//check condition of termination

				stop = 0;
                #pragma omp for nowait schedule(guided)
				for (int i = 0; i < K; i++)
				{
					if (PrevKmeanValues[i] != KmeanArray[i])
					{
						stop = 1;

					}
				}
			}

		}
			else
				break;
		

            #pragma omp atomic
		     	iterationNumber++;
		}

	cout << "results: " << endl;
	for (int i = 0; i < K; i++)
		cout << KmeanArray[i] << endl;

	cout << float(clock() - begin_time2) / CLOCKS_PER_SEC << endl;

	   delete[] PrevKmeanValues;
    	for (int i = 0; i < K; i++)
		{
			delete[]DifferenceArray[i];
		}
		delete[] DifferenceArray;

		for (int i = 0; i < K; i++)
		{
			delete[] clusterGroups[i];
		}
		delete[] clusterGroups;
}
double* readFromFile(int N)
{
	int ArrayLength = 10000000;
	ArrayLength = N;
	double X;
	double *FileArray = new double[ArrayLength]();

	?
	file.open("data.txt");

	if (!file.is_open())
		cout << " Cannot open file!" << endl;
	else
	{

		for (int i = 0; i < N; i++)
		{
			X = abs(10.556*rand() / RAND_MAX - 1.768);
			file << X << endl;
			FileArray[i] = X;
		}

		file.seekg(ios::beg);

		//for (int i = 0; i<N; ++i)
		//{
		//	double k;
		//	file >> k;
		//	cout << k << endl;
		//}
	}

	file.close();

	return FileArray;
}
int main()
{

	int choice1, choice2;
	char c='y';
	while (c == 'y')
	{
		c = 'n';
		cout << "Please choose one of the following:" << endl;
		cout << "Enter 1 for sequential K-Mean, or 2 for parallel K-Mean:" << endl;
		cin >> choice1;
		if (choice1 != 1 && choice1 != 2)
		{
			cout << "Wrong choice! Please reenter your choice" << endl;
			cin >> choice1;
		}

		cout << "if you want to enter the N number by yourself enter 1, " << endl;
		cout << "or enter 2 if you wan to read it from a file? ";
		cin >> choice2;

		if (choice2 != 1 && choice2 != 2)
		{
			cout << "Wrong choice! Please reenter your choice" << endl;
			cin >> choice2;
		}

		int N, K;

		if (choice1 == 1)
		{
			cout << "----------------------------------Sequential K-Mean-----------------------------------" << endl;

			//readFromUser
			if (choice2 == 1)
			{
				cout << "How many numbers you want to enter: ";
				cin >> N;
				if (N > 10000000)
				{
					cout << "WARNING! You can't enter more than 10000000, please enter a number <= 10000000:";
					cin >> N;
				}

				cout << "How many clusters you want to enter: ";
				cin >> K;
				if (K > N / 2)
				{
					cout << "WARNING! The maximum number of clusters is " << N / 2;
					cout << "Please enter a number <= " << N / 2 << " : ";
					cin >> K;
				}
				double* inputArray = new double[N];
				double* kmeanArray = new double[K];

				for (int i = 0; i < N; i++)
				{
					cout << "Enter number " << i + 1 << " : ";
					cin >> inputArray[i];
				}


				//choose random number for initial clusters values
				int randomNumber;
				for (int i = 0; i < K; i++)
				{
					randomNumber = rand() % N;
					kmeanArray[i] = inputArray[randomNumber];
				}


				sequential_kmean(N, inputArray, K, kmeanArray);

				delete[] inputArray;
				delete[] kmeanArray;
			}

			else
			{

				//readFromFile
				cout << "How many numbers you want to read from file: ";
				cin >> N;
				if (N > 10000000)
				{
					cout << "WARNING! You can't enter more than 10000000, please enter a number <= 10000000:";
					cin >> N;
				}

				cout << "How many clusters you want: ";
				cin >> K;
				if (K > N / 2)
				{
					cout << "WARNING! The maximum number of clusters is " << N / 2;
					cout << "Please enter a number <= " << N / 2 << " : ";
					cin >> K;
				}

				double* inputArray = new double[N];
				double* kmeanArray = new double[K];

				inputArray = readFromFile(N);

				//choose random number for initial clusters values
				int randomNumber;
				for (int i = 0; i < K; i++)
				{
					randomNumber = rand() % N;
					kmeanArray[i] = inputArray[randomNumber];
				}

				sequential_kmean(N, inputArray, K, kmeanArray);

				delete[] inputArray;
				delete[] kmeanArray;
			}

			cout << "Do you want to continue? enter y to continue and n to exit" << endl;
			cin >> c;
			if (c == 'Y')
				c = 'y';
			else if (c == 'N')
				c = 'n';
		}

		else
		{
			cout << "----------------------------------Parallel K-Mean----------------------------------" << endl;

			if (choice2 == 1)
			{
				cout << "How many numbers you want to enter: ";
				cin >> N;
				if (N > 10000000)
				{
					cout << "WARNING! You can't enter more than 10000000, please enter a number <= 10000000:";
					cin >> N;
				}

				cout << "How many clusters you want to enter: ";
				cin >> K;
				if (K > N / 2)
				{
					cout << "WARNING! The maximum number of clusters is " << N / 2;
					cout << "Please enter a number <= " << N / 2 << " : ";
					cin >> K;
				}
				double* inputArray = new double[N];
				double* kmeanArray = new double[K];

				for (int i = 0; i < N; i++)
				{
					cout << "Enter number " << i + 1 << " : ";
					cin >> inputArray[i];
				}

				//choose random number for initial clusters values
				int randomNumber;
				for (int i = 0; i < K; i++)
				{
					randomNumber = rand() % N;
					kmeanArray[i] = inputArray[randomNumber];
				}

				parallel_kmean(N, inputArray, K, kmeanArray);

				delete[] inputArray;
				delete[] kmeanArray;
			}

			else
			{

				//readFromFile
				cout << "How many numbers you want to read from file: ";
				cin >> N;
				if (N > 10000000)
				{
					cout << "WARNING! You can't enter more than 10000000, please enter a number <= 10000000:";
					cin >> N;
				}

				cout << "How many clusters you want: ";
				cin >> K;
				if (K > N / 2)
				{
					cout << "WARNING! The maximum number of clusters is " << N / 2;
					cout << "Please enter a number <= " << N / 2 << " : ";
					cin >> K;
				}

				double* inputArray = new double[N];
				double* kmeanArray = new double[K];

				inputArray = readFromFile(N);

				//choose random number for initial clusters values
				int randomNumber;
				for (int i = 0; i < K; i++)
				{
					randomNumber = rand() % N;
					kmeanArray[i] = inputArray[randomNumber];
				}

				parallel_kmean(N, inputArray, K, kmeanArray);

				delete[] inputArray;
				delete[] kmeanArray;
			}

			cout << "Do you want to continue? enter y to continue and n to exit" << endl;
			cin >> c;
			if (c == 'Y')
				c = 'y';
			else if (c == 'N')
				c = 'n';
		}
	}
}