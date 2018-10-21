// TestConsoleProgram.cpp : Defines the entry point for the console application.
//

#include <chrono>
#include <future>
#include <iostream>
#include <random>
#include <thread>
#include <vector>


size_t const static TOTAL_THREAD_COUNT = 4;
size_t const static SIZE_N = 10000;
size_t const static SIZE_M = 10000;

template <typename T>
struct MinMaxPair
{
	T min;
	T max;
};


template <typename T>
MinMaxPair<T> GetMinMaxInRow(std::vector<T> const& row)
{
	T min = row[0];
	T max = row[0];

	for (size_t j = 1; j < row.size(); ++j)
	{
		if (max < row[j])
		{
			max = row[j];
		}
		else if (min > row[j])
		{
			min = row[j];
		}
	}
	return{ min, max };
}

template <typename T>
MinMaxPair<T> GetMinMaxForRows(std::vector<std::vector<T>> const& matrix, size_t const startRow, size_t const rowCount)
{
	MinMaxPair<T> minPair = GetMinMaxInRow(matrix[startRow]);
	for (size_t i = startRow + 1; i < rowCount || i < matrix.size(); ++i)
	{
		MinMaxPair<T> currentPair = GetMinMaxInRow(matrix[i]);
		if (minPair.max < currentPair.max)
		{
			minPair.max = currentPair.max;
		}
		if (minPair.min > currentPair.min)
		{
			minPair.min = currentPair.min;
		}
	}
	return minPair;
}


template <typename T>
MinMaxPair<T> GetMinMaxInPair(std::vector<MinMaxPair<T>> const& row)
{
	T min = row[0].min;
	T max = row[0].max;

	for (size_t j = 1; j < row.size(); ++j)
	{
		if (max < row[j].max)
		{
			max = row[j].max;
		}
		else if (min > row[j].min)
		{
			min = row[j].min;
		}
	}
	return{ min, max };
}

template<typename T>
class RandomGenerator
{
public:
	virtual T GetNextRand() const = 0;
	virtual void PrintResult(MinMaxPair<T> const& pair) const = 0;
};

class IntGenerator : public RandomGenerator<int>
{
public:
	int GetNextRand() const override
	{
		return rand() % 31 - 15;
	}

	void PrintResult(MinMaxPair<int> const& pair) const override
	{
		printf("Min element: %d, Max element %d\n", pair.min, pair.max);
	}

};

template <typename T>
std::vector<std::vector<T>> GetRandomMatrix(size_t const n, size_t const m, RandomGenerator<T> const& generator)
{
	std::vector<std::vector<T>> matrix(n);
	for (size_t i = 0; i < n; ++i)
	{
		matrix[i].reserve(m);
		for (size_t j = 0; j < m; ++j)
		{
			matrix[i].push_back(generator.GetNextRand());
		}
	}

	return matrix;
}

template <typename T>
MinMaxPair<T> GetMinMaxInMatrixMultithread(std::vector<std::vector<T>> const& mat, size_t const countOfThreads = 1)
{
	std::vector<MinMaxPair<T>> rowPairs(mat.size());

	if (countOfThreads == 1)
	{
		for (size_t i = 0; i < mat.size(); ++i)
		{
			rowPairs[i] = GetMinMaxInRow<T>(mat[i]);
		}
	}
	else
	{
		std::vector<std::future<MinMaxPair<T>>> futures(countOfThreads);
		size_t const countOfRowsForOneThread =  static_cast<size_t>(std::round(mat.size() * 1.0f / countOfThreads));
		size_t const preLastNum = countOfThreads - 1;
		for (size_t i = 0; i < preLastNum; ++i)
		{
			futures[i] = std::async(std::launch::async, GetMinMaxForRows<T>, mat, countOfRowsForOneThread * i, countOfRowsForOneThread);
		}
		futures[preLastNum] = std::async(std::launch::async, GetMinMaxForRows<T>, mat, countOfRowsForOneThread * preLastNum, mat.size() - preLastNum * countOfRowsForOneThread);

		for (size_t i = 0; i < countOfThreads; ++i)
		{
			rowPairs[i] = futures[i].get();
		}
	}

	return GetMinMaxInPair<T>(rowPairs);
}

int main() {
	IntGenerator gen;
	auto matrix = GetRandomMatrix<int>(SIZE_N, SIZE_M, gen);
	
	printf("One thread: \n");
	auto start = std::chrono::steady_clock::now();
	auto minMax = GetMinMaxInMatrixMultithread(matrix);
	auto end = std::chrono::steady_clock::now();
	gen.PrintResult(minMax);
	printf("Elapsed time: %llu\n", std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

	printf("Four threads: \n");
	start = std::chrono::steady_clock::now();
	minMax = GetMinMaxInMatrixMultithread(matrix, TOTAL_THREAD_COUNT);
	end = std::chrono::steady_clock::now();
	gen.PrintResult(minMax);
	printf("Elapsed time: %llu\n", std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

	getchar();
	return 0;
}

