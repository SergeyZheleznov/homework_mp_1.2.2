// homework_mp_1.2.2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
//Задание 2
// 05.03.2024 - 07.03.2024
//Параллельные вычисления

//Напишите программу для расчёта суммы двух векторов.
//Распараллельте эту программу на 2, 4, 8 и 16 потоков.
//Определите, какое количество потоков даёт самый быстрый результат.
//Сравните результаты выполнения для массивов из 1 000, 10 000, 100 000 и 1 000 000 элементов.
//Время выполнения для каждого варианта сведите в таблицу и выведите её в консоль.
//Первый запущенный поток должен вывести на экран доступное количество аппаратных ядер.

#include <iostream>
#include <thread>
#include <execution>
#include <algorithm>
#include <random>
#include <chrono>
#include <vector>
#include <cmath>
#include <windows.h>

// делаем в один поток
namespace {
	auto sum_vec1(std::vector<int> vec1, std::vector<int> vec2, std::vector<int> sum_vec, int size)
	{
		for (int i = 0; i < size; ++i)
		{
			sum_vec[i] = vec1[i] + vec2[i];
		}
		return sum_vec;
	}
	// делаем универсальную функцию для двух, четырёх, восьми и шестнадцати потоков
	auto sum_vec2(std::vector<int> vec1, std::vector<int> vec2, std::vector<int> sum_vec, int begin, int end)
	{
		for (int i = begin; i < end; ++i)
		{
			sum_vec[i] = vec1[i] + vec2[i];
		}
		return sum_vec;
	}
	// функция заполнения вектора случайными числами

	auto random_vec(int size)
	{
		std::vector<int> vec(size);
		std::mt19937 gen;
		std::uniform_int_distribution<int> dist(0, size);
		std::generate(std::execution::par, vec.begin(), vec.end(), [&]() {
			return dist(gen);
			});

		return vec;
	}
}

int main()
{
	setlocale(LC_ALL, "ru");
	std::cout << "Количество аппаратных ядер: " << std::thread::hardware_concurrency() << std::endl;

	// Делаем цикл для одного потока
	int* run_time_1_stream = new int[4];
	int size = 0;
	for (int i = 0; i < 4; ++i)
	{
		size = 1000 * pow(10, i);
		//создаём первый вектор
		std::vector<int> vec1 = random_vec(size);
		// создаём второй вектор
		std::vector<int> vec2 = random_vec(size);
		// создаём вектор, в котором будет сумма двух векторов
		std::vector<int> sum_vec = random_vec(size);
		// здесь мы замеряем время старта
		auto start = std::chrono::steady_clock::now();
		// Запускаем в один поток
		auto sum = sum_vec1(vec1, vec2, sum_vec, size);
		// замеряем время завершения работы
		auto finish = std::chrono::steady_clock::now();
		// здесь время работы программы от начала до конца
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
		// кладём время в массив
		run_time_1_stream[i] = diff.count();
	}
 
	// Делаем цикл для двух, четырёх, восьми и шестнадцати потоков
	int run_time_2_stream[10][4];
	size = 0;
	int max = 0;

	for (int i = 1; i < 8; ++i)
	{
		max = i * 2;
		for (int j = 0; j < 4; ++j)
		{
			size = 1000 * pow(10, j);
			//создаём первый вектор
			std::vector<int> vec1 = random_vec(size);
			// создаём второй вектор
			std::vector<int> vec2 = random_vec(size);
			// создаём вектор, в котором будет сумма двух векторов
			std::vector<int> sum_vec = random_vec(size);
			// здесь мы замеряем время старта
			auto start = std::chrono::steady_clock::now();
			// Запускаем в два потока
			std::vector<std::thread> V;

			for (int k = 0; k < max; ++k)
			{
				V.push_back(std::thread(sum_vec2, vec1, vec2, sum_vec, size * k / max, size * (k + 1) / max));
			}
			for (auto& el : V)
			{
				el.join();
			}
		// замеряем время завершения работы
		auto finish = std::chrono::steady_clock::now();
		// здесь время работы программы от начала до конца
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
		// кладём время в массив
		run_time_2_stream[i-1][j] = diff.count();
		}
	}

	std::cout << "      " << " Время выполнения " << std::endl;
	std::cout << "Размер вектора         " << " 1000   " << " 10000   " << "100000  " << "1000000 " << std::endl;

	std::cout << "Один поток             " << run_time_1_stream[0] << " ms      " << run_time_1_stream[1] << " ms  " << run_time_1_stream[2] << " ms    " <<
		run_time_1_stream[3] << " ms\n";
	std::cout << "Два  потока            " << run_time_2_stream[0][0] << " ms     " << run_time_2_stream[0][1] << " ms  " << run_time_2_stream[0][2] << " ms    " <<
		run_time_2_stream[0][3] << " ms\n";
	std::cout << "Четыре  потока         " << run_time_2_stream[1][0] << " ms     " << run_time_2_stream[1][1] << " ms  " << run_time_2_stream[1][2] << " ms    " <<
		run_time_2_stream[1][3] << " ms\n";
	std::cout << "Восемь потоков         " << run_time_2_stream[2][0] << " ms     " << run_time_2_stream[2][1] << " ms  " << run_time_2_stream[2][2] << " ms    " <<
		run_time_2_stream[2][3] << " ms\n";
	std::cout << "Шестнадцать  потоков   " << run_time_2_stream[3][0] << " ms     " << run_time_2_stream[3][1] << " ms  " << run_time_2_stream[3][2] << " ms    " <<
		run_time_2_stream[3][3] << " ms\n";

	delete[] run_time_1_stream;
	
	return 0;
}
