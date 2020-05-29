#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "winbgi2.h"
#include "rk4.h"			//niezbedna biblioteka z funkcja obliczajaca metoda rk4

//Prototypy funkcji
void scan(double* x);
void funk2(double t, double* F, double* F1);

double g = 9.80665;		//przyspieszenie ziemskie
double a = 0.0;			//kat nachylenia dziala
double v0 = 1.0;		//predkosc poczatkowa
double x0 = 0.0;		//wspolrzedna pocztkowa
double y00 = 0.0;		//wspolrzedna poczatkowa
double m = 1;		//masa kulki

void main()
{
	double h = 0.001;			//krok czasowy, mniejszy znacznie spowalnia program
	double  t = 0;				//zmienna czasu
	double* F, * F1;			//tablice u¿ywane do obliczen
	double n = 4;				//ilosc rownan
	int d = 0;					//zmienna pomocnicza
	int czasp, czask;			//czasp poczatkowy i koncowy
	double ekin, epot;			//energia kinetyczna i potencjalna

	//obsluga pliku z wynikami
	FILE* p;					
	p = fopen("wyniki.txt", "w");
	if (!p)
	{
		printf("Wystapil blad przy otwieraniu pliku!");
		exit(1);
	}
	fprintf(p,"Wspolrzedna X \t Wspolrzedna Y \t Predkosc Vx \t Predkosc Vy \t En. Kin. \t En. Pot. \n");
	
	//alokacja pamieci do tablic dynamicznych
	F = (double*)malloc(n * sizeof(double));
	F1 = (double*)malloc(n * sizeof(double));

	do
	{
		printf("Podaj predkosc poczatkowa: \n");
		scan(&v0);
		if (v0 <= 0) printf("Predkosc poczatkowa nie moze byc mniejsza lub rowna 0! \n");
	} while (v0 <= 0);

	do
	{
		printf("Podaj kat nachylenia dziala w stopniach od <0,90>: \n");
		scan(&a);
		if (a < 0 || a > 90) printf("Kat alfa musi zawierac sie pomiedzy 0 a 90 stopni! \n");
	} while (a < 0 || a > 90);
	a = a * 3.14159265359 / 180;

	do
	{
		printf("Podaj wspolrzedne konca dziala [x0,y0]: \n");
		scan(&x0);
		scan(&y00);
		if (x0 < 0 || y00 < 0) printf("Wspolrzedne musza byc dodatnie! \n");
	} while (x0 < 0 || y00 < 0);
	
	do
	{
		printf("Podaj mase kulki: \n");
		scan(&m);
		if (m <= 0) printf("Masa nie moze byc mniejsza lub rowna 0! \n");
	} while (m <= 0);

	printf("WYBIERZ WYKRES \n y(x) - Wcisnij 1 \n y(t) - Wcisnij 2 \n x(t) - Wcisnij 3 \n Vx(t) - Wcisnij 4 \n Vy(t) - Wcisnij 5 \n Energie(t) - Wcisnij 6 \n");
	scanf("%d",&d);
	
	czasp = time(NULL);			//czas poczatkowy

	//wartosci poczatkowe
	F[0] = y00;				
	F[1] = x0;
	F[2] = v0 * sin(a);
	F[3] = v0 * cos(a);

	F1[0] = y00;
	F1[1] = x0;
	F1[2] = v0 * sin(a);
	F1[3] = v0 * cos(a);

	
	graphics(700,700);		//obsluga grafiki
	switch (d)				//wybor wykresu
	{

		case 1:
			scale(0.0, 0.0, 1.2*y00 + x0 + (v0 * v0) / g * sin(2 * a), (v0 * v0 * sin(a) * sin(a)) / (2 * g) + y00);
			title("x", "y", "Wykres y(x)");
			break;
		case 2:
			scale(0.0, 0.0, ((v0 * sin(a) + sqrt(v0 * sin(a) * (v0 * sin(a))) + 2 * g * y00)) / g, (v0 * v0 * sin(a) * sin(a)) / (2 * g) + y00);
			title("t", "y", "Wykres y(t)");
			setcolor(0.1);
			break;
		case 3:
			scale(0.0, 0.0, ((v0 * sin(a) + sqrt(v0 * sin(a) * (v0 * sin(a))) + 2 * g * y00)) / g, (v0 * v0 * sin(2*a)) / g + x0+y00);
			title("t", "x", "Wykres x(t)");
			setcolor(0.6);
			break;
		case 4:
			scale(0.0, 0.0, ((v0 * sin(a) + sqrt(v0 * sin(a) * (v0 * sin(a))) + 2 * g * y00)) / g, v0);
			title("t", "Vx", "Wykres Vx(t)");
			setcolor(0.8);
			break;
		case 5:
			scale(0.0, -((v0 * sin(a) + sqrt(v0 * sin(a) * (v0 * sin(a))) + 2 * g * y00)), ((v0 * sin(a) + sqrt(v0 * sin(a) * (v0 * sin(a))) + 2 * g * y00)) / g, v0);
			title("t", "Vy", "Wykres Vy(t)");
			setcolor(1);
			break;
		case 6:
			scale(0.0, 0.0, ((v0 * sin(a) + sqrt(v0 * sin(a) * (v0 * sin(a))) + 2 * g * y00)) / g, (m * v0 * v0) / 2 + m * g * y00);
			title("t","Energie","Energia(t): pot. - niebieska , kin. - czerwona");
			break;
		default:
			scale(0.0, 0.0, y00 * x0 + (v0 * v0) / g * sin(2 * a), (v0 * v0 * sin(a) * sin(a)) / (2 * g) + y00);
			title("x", "y", "Wykres y(x)");
			break;
			
	}

	while(F[0]>=0)
	{
		vrk4(t, F, h, n, funk2, F1);			//wywolanie funkcji vrk4
		if (F1[0] < 0) break;			//przerwanie w przypadku gdy w danym kroku osiagnieto wartosi ujemne
		t += h;

		F[0] = F1[0];
		F[1] = F1[1];
		F[2] = F1[2];
		F[3] = F1[3];

		ekin = (m * pow(sqrt(F[3] * F[3] + F[2] * F[2]), 2.0))/ 2;
		epot = m * g * F[0];
		//printf("Energia kin. = %lf \t Energia pot. = %lf \n",ekin, epot);		//znacznie spowalnia program, potrzebne tylko do sprawdzania

		if (F[0] > 80000)			//ukryta funkcja
		{
			title("","","Pocisk opuscil atmosfere!");
			printf("Pocisk opuscil atmosfere!");
			break;
		}

		fprintf(p, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",F[1],F[0],F[3],F[2],ekin,epot);			//zapisywanie do pliku
		//printf("X = %lf Y = %lf Vx = %lf Vy = %lf \n", F[1], F[0], F[3], F[2]);				//znacznie spowalnia program, potrzebne tylko do sprawdzania
		
		switch (d)			//rysowanie wybranego wykresu
		{

			case 1:
				point(F[1], F[0]);
				break;
			case 2:
				point(t, F[0]);
				break;
			case 3:
				point(t, F[1]);
				break;
			case 4:
				point(t, F[3]);
				break;
			case 5:
				point(t, F[2]);
				break;
			case 6:
				setcolor(0);
				point(t, ekin);
				setcolor(1);
				point(t, epot);
				break;
			default:
				point(F[1], F[0]);
				break;

		}
	}

	czask = time(NULL);				//czas koncowy
	printf("Program rozwiazal zadanie w czasie %d sekund! \n", czask-czasp);

	fclose(p);
	free(F);
	free(F1);
	wait();
}

void scan(double* x)		//funkcja do wczytywania i sprawdzania zgodnosci inputu
{
	double temp;
	while (scanf("%lf", &temp) != 1)
	{
		printf("Nieprawidlowy format danych! \n");
		int n;
		while ((n = getchar()) != EOF && n != '\n');
	}
	*x = temp;
}

void funk2(double t, double* F, double* F1)			//funckja obliczajaca prawe strony
{
	F1[1] = F[3];
	F1[0] = F[2];
	F1[2] = -g;
	F1[3] = 0;
}