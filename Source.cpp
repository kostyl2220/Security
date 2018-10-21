#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>
#include <map>
#include <future>
#include <unordered_map>
#include <chrono>

////////////////////////////////////////////////// FIRST CIPHER
bool IsMeanableLine(std::string const& line)
{
	size_t countOfGaps = 0;
	size_t countOfLetters = 0;
	size_t const lineSize = line.size();
	for (size_t i = 0; i < lineSize; ++i)
	{
		if (line[i] == ' ')
		{
			++countOfGaps;
		}
		else if (line[i] >= 'a' && line[i] <= 'z'
			|| line[i] >= 'A' && line[i] <= 'Z')
		{
			++countOfLetters;
		}
	}
	float const letterAndGapPersentage = 85.0f;
	float const generalKoef = countOfGaps * 100.0f / lineSize +
		countOfLetters * 100.0f / lineSize;
	return generalKoef >= letterAndGapPersentage;
}

void FirstCipher(std::string const& cipher)
{
	size_t const cipherSize = cipher.size();

	for (size_t i = 1; i < 256; ++i)
	{
		std::string result;

		for (size_t j = 0; j < cipherSize; ++j)
		{
			char newChar = (char)((size_t)cipher[j] ^ i);
			result += newChar == '=' ? 'l' : newChar;
		}

		if (IsMeanableLine(result))
		{
			printf("First: %s\n", result.c_str());
			getchar();
			return;
		}
	}
}

////////////////////////////////////////////////// SECOND CIPHER

bool IsMeanableLine(std::string const& line, size_t const sizeOfLine, size_t const startI = 0, size_t const offset = 1, float const letterAndGapPersentage = 85.0f)
{
	size_t countOfGaps = 0;
	size_t countOfLetters = 0;
	size_t const lineSize = line.size();
	for (size_t i = startI; i < lineSize; i += offset)
	{
		if (line[i] == ' ')
		{
			++countOfGaps;
		}
		else if (line[i] >= 'a' && line[i] <= 'z'
			|| line[i] >= 'A' && line[i] <= 'Z')
		{
			++countOfLetters;
		}
	}
	float const generalKoef = countOfGaps * 100.0f / lineSize +
		countOfLetters * 100.0f / sizeOfLine;
	//float const wordsGapPersentage = countOfLetters / countOfGaps;
	return generalKoef >= letterAndGapPersentage;
}

std::string ParcialEnscryption(std::string const& cipher, char const key, size_t const startI, size_t const offset)
{
	size_t const cipherSize = cipher.size();
	size_t keyIndex = 0;
	std::string result;
	for (size_t i = 0; i < cipherSize; ++i)
	{
		result += ((i % offset == startI) ? (int)cipher[i] ^ (int)key : cipher[i]);
	}
	return result;
}

size_t CountOfLetter(std::string const& cipher, char const searchLetter, size_t const startEl, size_t const offset)
{
	size_t count = 0;
	for (size_t i = startEl; i < cipher.size(); i += offset)
	{
		if (cipher[i] == searchLetter)
		{
			++count;
		}
	}
	return count;
}

size_t CountValueForOne(std::string const& cipher, char const letter, size_t const startEl, size_t const offset)
{
	size_t const countOfLetter = CountOfLetter(cipher, letter, startEl, offset);
	return countOfLetter * (countOfLetter - 1);
}

std::vector<char> GetAllUniqueLetters(std::string const& line)
{
	std::set<char> newSet;
	for (auto& letter : line)
	{
		newSet.insert(letter);
	}
	std::vector<char> letters(newSet.begin(), newSet.end());
	return letters;
}

size_t GetLengthOfKey(std::string const& cipher)
{
	//koef was taken from site  https://habr.com/post/221485/
	float const keofOfKeyLenght = 1.06f;
	float maxKoef = 0;
	std::vector<char> keyValueArray = GetAllUniqueLetters(cipher);
	size_t const charValueArraySize = keyValueArray.size();
	size_t const cipherSize = cipher.size();
	for (size_t i = 2; i < cipherSize; ++i)
	{
		float currentKoef = 0;
		size_t const partLength = cipherSize / i;
		size_t const almostSquareSize = partLength * (partLength - 1);

		for (size_t l = 0; l < charValueArraySize; ++l)
		{
			currentKoef += CountValueForOne(cipher, keyValueArray[l], 0, i);
		}
		currentKoef /= almostSquareSize;

		if (maxKoef < currentKoef)
		{
			if (maxKoef != 0 && currentKoef * keofOfKeyLenght > maxKoef)
			{
				return i;
			}
			maxKoef = currentKoef;
		}
	}

	return -1;
}

bool GetEnscrypted(std::string const& cipher, std::string& result, size_t const keyIndex, size_t const keyLength)
{
	size_t const cipherSize = cipher.size() / keyLength;
	for (size_t i = 1; i < 256; ++i)
	{
		std::string partialRes = ParcialEnscryption(cipher, (char)i, keyIndex, keyLength);
		if (IsMeanableLine(partialRes, cipherSize, keyIndex, keyLength, 82.0f))
		{
			if (keyIndex == keyLength - 1)
			{
				result = partialRes;
				return true;
			}
			if (GetEnscrypted(partialRes, result, keyIndex + 1, keyLength))
			{
				return true;
			}
		}
	}
	return false;
}

std::string HexToString(std::string const& input)
{
	//copy paste from stackowerflow https://stackoverflow.com/questions/3381614/c-convert-string-to-hexadecimal-and-vice-versa
	static const char* const lut = "0123456789abcdef";
	size_t len = input.length();
	if (len & 1) throw std::invalid_argument("odd length");

	std::string output;
	output.reserve(len / 2);
	for (size_t i = 0; i < len; i += 2)
	{
		char a = input[i];
		const char* p = std::lower_bound(lut, lut + 16, a);
		if (*p != a) throw std::invalid_argument("not a hex digit");

		char b = input[i + 1];
		const char* q = std::lower_bound(lut, lut + 16, b);
		if (*q != b) throw std::invalid_argument("not a hex digit");

		output.push_back(((p - lut) << 4) | (q - lut));
	}
	return output;
}

void SecondCipher(std::string const& cipher)
{
	size_t keyLength = GetLengthOfKey(cipher);
	printf("Length of key %d\n", keyLength);
	std::string result;
	if (GetEnscrypted(cipher, result, 0, keyLength))
	{
		printf("Second: ");
		for (auto& letter : result)
		{
			printf("%c", letter);		
		}
		printf("\n");
	}
	getchar();
}

////////////////////////////////////////////////// THIRD CIPHER

size_t const keySize = 26;
size_t const maxWordSize = 32;
size_t const MAX_POPULATION_SIZE = 1000;
size_t const MIN_POPULATION_SIZE = 700;
size_t const MAX_N_GRAM_SIZE = 5;
size_t const COUNT_OF_CHILDREN = MAX_POPULATION_SIZE - MIN_POPULATION_SIZE;
float const EPSILON = 0.0001f;
float const MIN_WORD_SEARCH_SIZE = 0.8f;
float const WORD_LETTER_COEF = 0.02f;
size_t const COUNT_OF_WORDS_IN_VOCABLUARY = 1000;
size_t const COUNT_OF_PARENT_MUTATIONS = 6;
size_t const START_WORD_LEN = 2;
size_t const MAX_COUNT_OF_REPEATS = 100;

struct Parent
{
	size_t key[keySize];
	float fitnesValue;

	Parent(size_t const k[keySize]) : key(), fitnesValue(0)
	{
		std::copy(k, k + keySize, key);
	}

	Parent() {};
};

std::vector<std::string> GetVocabulary()
{
	std::vector<std::string> vocabulary;
	std::ifstream file("vocabluary.txt");
	
	while (file)
	{
		char str[maxWordSize];
		file.getline(str, maxWordSize);
		if (strlen(str) >= START_WORD_LEN)
		{
			vocabulary.push_back(str);
		}
	}
	file.close();
	return vocabulary;
}

struct WordFreq
{
	std::string word;
	double occurencies;
};

std::vector<WordFreq> GetVocabulary2()
{
	std::vector<WordFreq> vocabulary;
	std::ifstream file("voc2.txt");

	size_t currentWord = 0;
	long long sum = 0;
	while (file && currentWord < COUNT_OF_WORDS_IN_VOCABLUARY)
	{
		char str[maxWordSize];
		file.getline(str, maxWordSize);
		if (!strlen(str))
		{
			continue;
		}
		char* spacePos = strchr(str, ' ');
		size_t const occurencies = atoi(spacePos + 1);
		*spacePos = '\0';
		vocabulary.push_back({str, (double)occurencies});
		sum += occurencies;
		++currentWord;
	}
	for (auto& wordFreq : vocabulary)
	{
		wordFreq.occurencies /= sum;
	}

	file.close();
	return vocabulary;
}

bool IsMeanableString(std::string const& line)
{
	for (auto& letter : line)
	{
		if (letter < 'a' || letter > 'z')
		{
			return false;
		}
	}
	return true;
}

std::vector<std::map<std::string, double>> GetNGrams(size_t const maxAnagram)
{
	std::vector<WordFreq> voc = GetVocabulary2();
	std::vector<std::map<std::string, double>> nGrams;
	std::vector<std::map<std::string, size_t>> nGramCounts;
	nGrams.resize(maxAnagram - 1);
	nGramCounts.resize(maxAnagram - 1);
	for (WordFreq& wordFreq : voc)
	{
		for (size_t i = 2; i <= maxAnagram; ++i)
		{
			if (wordFreq.word.size() < i)
			{
				continue;
			}

			size_t const endIndex = wordFreq.word.size() - i + 1;
			for (size_t j = 0; j < endIndex; ++j)
			{
				std::string nGram = wordFreq.word.substr(j, i);
				if (!IsMeanableString(nGram))
				{
					continue;
				}

				if (nGrams[i - 2].find(nGram) == nGrams[i - 2].end())
				{
					nGrams[i - 2][nGram] = 0;
				}
				if (nGramCounts[i - 2].find(nGram) == nGramCounts[i - 2].end())
				{
					nGramCounts[i - 2][nGram] = 0;
				}
				nGrams[i - 2][nGram] += wordFreq.occurencies;
				nGramCounts[i - 2][nGram] += 1;
			}
		}
	}

	for (size_t i = 0; i < maxAnagram - 1; ++i)
	{
		for (auto& pair : nGrams[i])
		{
			pair.second /= nGramCounts[i][pair.first];
		}
	}

	return nGrams;
}

std::string Enscrypt(std::string const& cipher, size_t const key[keySize])
{
	std::string result;
	result.reserve(cipher.size());
	for (auto& letter : cipher)
	{
		size_t const letterId = (size_t)letter - (size_t)'a';
		result.push_back((char)('a' + key[letterId]));
	}
	return result;
}

Parent GetSingleCrossover(Parent const& parent1, Parent const& parent2)
{
	Parent child;
	bool filledKeys[keySize];
	std::fill(filledKeys, filledKeys + keySize, false);
	size_t countOfMismatch = 0;
	for (size_t i = 0; i < keySize; ++i)
	{
		size_t parentId = rand() % 2;
		Parent const& currentParent = (parentId % 2 == 0 ? parent1 : parent2);
		if (!filledKeys[currentParent.key[i]])
		{
			child.key[i] = currentParent.key[i];
			filledKeys[currentParent.key[i]] = true;
		}
		else
		{
			Parent const& otherParent = (parentId % 2 == 0 ? parent2 : parent1);
			if (!filledKeys[otherParent.key[i]])
			{
				child.key[i] = otherParent.key[i];
				filledKeys[otherParent.key[i]] = true;
			}
			else
			{
				++countOfMismatch;
				child.key[i] = -1;
			}
		}
	}
	if (countOfMismatch)
	{
		std::vector<size_t> mismathes;
		mismathes.reserve(countOfMismatch);
		for (size_t i = 0; i < keySize; ++i)
		{
			if (!filledKeys[i])
			{
				mismathes.push_back(i);
			}
		}
		std::random_shuffle(mismathes.begin(), mismathes.end());
		
		for (auto& keyValue : child.key)
		{
			if (keyValue == -1)
			{
				keyValue = mismathes[--countOfMismatch];
			}
		}
	}

	return child;
}

void InitPopulation(std::vector<Parent>& population)
{
	size_t key[keySize];
	for (size_t i = 0; i < keySize; ++i)
	{
		key[i] = i;
	}
	for (size_t i = 0; i < MAX_POPULATION_SIZE; ++i)
	{
		std::random_shuffle(&key[0], &key[keySize]);
		population.emplace_back(key);
	}
}

size_t GetCountOfOccurencies(std::string const& line, std::string const& search)
{
	size_t countOfOccurencies = 0;

	size_t pos = line.find(search, 0);
	while (pos != std::string::npos)
	{
		++countOfOccurencies;
		pos = line.find(search, pos + 1);
	}

	return countOfOccurencies;
}

double GetFitnesForNGram(std::string const& cipher, std::map<std::string, double> const& vocMap, double const nGramKoef)
{
	double fitnessSum = 0.0;
	for (auto& nGram : vocMap)
	{
		fitnessSum += GetCountOfOccurencies(cipher, nGram.first) * nGram.second * nGramKoef;
	}

	return fitnessSum;
}

float GetFitnessResult(std::string const& cipher, std::vector<std::map<std::string, double>> const& vocabulary, std::vector<std::string> const& wordVoc)
{
	double fitnessSum = 0.0;
	double const nGramKoef[] = { 1.0f, 1.2f, 1.5f, 1.8f };
	size_t const cipherLen = cipher.size();

	for (size_t i = 0; i < cipherLen - 1; ++i)
	{
		for (size_t len = 2; len < MAX_N_GRAM_SIZE && len + i <= cipherLen; ++len)
		{
			auto substr = cipher.substr(i, len);
			auto iter = vocabulary[len - 2].find(substr);
			if (iter != vocabulary[len - 2].end())
			{
				fitnessSum += iter->second * nGramKoef[len - 2];
			}
		}
	}
	if (fitnessSum > MIN_WORD_SEARCH_SIZE)
	{
		for (auto& word : wordVoc)
		{
			size_t const countOfOccurencies = GetCountOfOccurencies(cipher, word);
			if (countOfOccurencies)
			{
				fitnessSum += word.size() * WORD_LETTER_COEF;
			}
		}
	}

	return fitnessSum;
}

void ComputeFitnessForRange(std::string const& cipher, std::vector<Parent>::iterator begin, std::vector<Parent>::iterator const& end, std::vector<std::map<std::string, double>> const * const vocabulary, std::vector<std::string> const * const wordVoc)
{
	for (; begin != end; ++begin)
	{
		Parent& parent = *begin;
		parent.fitnesValue = GetFitnessResult(Enscrypt(cipher, parent.key), *vocabulary, *wordVoc);
	}
}

void ComputeFitnessForAll(std::string const& cipher, std::vector<Parent>& population, std::vector<std::map<std::string, double>> const& vocabulary, std::vector<std::string> const& wordVoc)
{
	size_t const THREAD_COUNT = 4;
	std::vector<std::thread> futures(THREAD_COUNT);
	size_t const sizeForOne = (COUNT_OF_CHILDREN - 1) / THREAD_COUNT + 1;
	std::vector<Parent>::iterator const startIter = population.begin() + MIN_POPULATION_SIZE;
	for (size_t i = 0; i < THREAD_COUNT; ++i)
	{
		std::vector<Parent>::iterator const endIter = (i == THREAD_COUNT - 1 ? population.end() : startIter + sizeForOne * (i + 1));
		futures[i] = std::thread(ComputeFitnessForRange, cipher, startIter + sizeForOne * i, endIter, &vocabulary, &wordVoc);
	}

	for (auto& thread : futures)
	{
		thread.join();
	}
}

void Selection(std::vector<Parent>& population)
{
	std::sort(population.begin(), population.end(), [](Parent a, Parent b) {return a.fitnesValue > b.fitnesValue; });
	population.erase(population.begin() + MIN_POPULATION_SIZE, population.end());
}

void MutateOne(Parent& parent)
{
	size_t const keyValue1 = rand() % keySize;
	size_t keyValue2 = rand() % keySize;
	while (keyValue1 == keyValue2)
	{
		keyValue2 = rand() % keySize;
	}

	std::swap(parent.key[keyValue1], parent.key[keyValue2]);
}

void Crossover(std::vector<Parent>& population)
{
	float const partsToGet[] = {0.0f, 0.01f, 0.02f, 0.1f, 0.4f, 1.0f};
	for (size_t i = 0; i < COUNT_OF_CHILDREN; ++i)
	{
		size_t const rangeToGet = rand() % 5;
		size_t const minIndex = partsToGet[rangeToGet] * MIN_POPULATION_SIZE;
		size_t const maxIndex = partsToGet[rangeToGet + 1] * MIN_POPULATION_SIZE;
		size_t const rangeSize = maxIndex - minIndex;
		size_t const indexToGet = rand() % rangeSize + minIndex;
		Parent child = population[rangeToGet == 0 ? 0 : indexToGet];
		for (size_t j = 0; j < rand() % COUNT_OF_PARENT_MUTATIONS; ++j)
		{
			MutateOne(child);
		}
		population.push_back(child);
	}
}

void Mutation(std::vector<Parent>& population)
{
	for (size_t i = MIN_POPULATION_SIZE; i < MAX_POPULATION_SIZE; ++i)
	{
		MutateOne(population[i]);
	}
}

bool Terminate(size_t const countOfReps)
{
	return countOfReps > MAX_COUNT_OF_REPEATS;
}

void PrintResult(std::string const& cipher, std::vector<Parent>& population)
{
	printf("Third: %s", Enscrypt(cipher, population.front().key).c_str());
	printf("Fitness value: %f", population.front().fitnesValue);
	getchar();
}

void ThirdCipher(std::string const& cipher)
{
	float lastBestFunc = 0.0f;
	size_t countOfRepeats = 0;

	std::string cipherCopy = cipher;
	std::transform(cipherCopy.begin(), cipherCopy.end(), cipherCopy.begin(), ::tolower);
	auto voc2 = GetNGrams(MAX_N_GRAM_SIZE);
	auto vocabulary = GetVocabulary();

	std::vector<Parent> population;
	InitPopulation(population);

	for (size_t i = 0; i < MAX_POPULATION_SIZE; ++i)
	{
		Parent& parent = population[i];
		parent.fitnesValue = GetFitnessResult(Enscrypt(cipherCopy, parent.key), voc2, vocabulary);
	}

	size_t numOfPopultion = 0;

	while (true)
	{
		++numOfPopultion;
		
		Selection(population);
		printf("Number of population %d, best fitness %f, count of repeats %d\n", numOfPopultion, population.front().fitnesValue, countOfRepeats);

		if (abs(population.front().fitnesValue - lastBestFunc) < EPSILON)
		{		
			++countOfRepeats;
		}
		else
		{
			lastBestFunc = population.front().fitnesValue;
			countOfRepeats = 0;
		}

		Crossover(population);
		Mutation(population);

		ComputeFitnessForAll(cipherCopy, population, voc2, vocabulary);

		if (Terminate(countOfRepeats))
		{
			PrintResult(cipherCopy, population);

			return;
		}
	}
}

bool IsLetter(char const letter)
{
	return letter >= 'a' && letter <= 'z'
		|| letter >= 'A' && letter <= 'Z' || letter == ' ' || letter == ',';
}

size_t const ACCEPTABLE_LETTER_NUMS = 13;

bool IsLetterEveryNth(std::vector<std::string> const& poem, size_t const letterNum)
{
	size_t countOfLetters = 0;
	for (auto& line : poem)
	{
		if (IsLetter(line[letterNum]))
		{
			++countOfLetters;
		}
	}
	return countOfLetters > ACCEPTABLE_LETTER_NUMS;
}

std::vector<std::string> GetEnscryptedNth(std::vector<std::string> const& poem, size_t const letterNum, size_t const key)
{
	std::vector<std::string> newPoem;
	for (auto& line : poem)
	{
		std::string newLine = line;
		newLine[letterNum] ^= key;
		newPoem.push_back(newLine);
	}

	return newPoem;
}


bool GetEnscryptedPoem(std::vector<std::string> const& poem, std::vector<std::string>& result, size_t const letterNum)
{
	for (size_t i = 1; i < 256; ++i)
	{
		auto partialRes = GetEnscryptedNth(poem, letterNum, (char)i);
		if (letterNum > 14 && poem[0].find("you can make") == -1)
		{
			continue;
		}

		if (IsLetterEveryNth(partialRes, letterNum) || letterNum == 18)
		{
			if (letterNum > 20)
			{
				char h = 'f';
			}
			if (letterNum == 30)
			{
				result = partialRes;
				return false;
			}
			if (GetEnscryptedPoem(partialRes, result, letterNum + 1))
			{
				return true;
			}
		}
	}
	return false;
}

void Additional()
{
	std::vector<std::string> poem =
	{   "ad924af7a9cdaf3a1bb0c3fe1a20a3f367d82b0f05f8e75643ba688ea2ce8ec88f4762fbe93b50bf5138c7b699",
		"a59a0eaeb4d1fc325ab797b31425e6bc66d36e5b18efe8060cb32edeaad68180db4979ede43856a24c7d",
		"a59a0eaeaad7fc3c56fe82fd1f6bb5a769c43a0f0cfae74f0df56fdae3db8d9d840875ecae2557bf563fcea2",
		"a59a0eaea8ddf93c08fe81e11e2ab2bb6d962f0f1af2f44243b46cc1b6d6c291995d65a9a5234aa204",
		"ad924af7a9cdaf3a1bb0c3f51439a5b628cf215a1fbdee4302a77a8ea2cc86c8984d65ffac6c58bf5b71dab8841136",
		"b09b4afda3caf93c5aa78ce6096bb2a67ad86e4302f3e10602b37acbb1829680935137e8bb2919b6503fccfdca5461",
		"a59a0eaeb5d7af3115b287b31425e6a460d3200f19f5e35406f567dde3cc8d9c9e4179eee92557f1463edc",
		"a18c09ebb6ccaf2d12bbc3c41227aaf37fde274c05bdf5471aa62edaac82968093452da9eb0456bd5b71c6bfcb56",

		"ad924af7a9cdaf3a1bb0c3e71a27adf37fdf3a474dfef44914b17d8ea2cc86c89d4d72f9e93556a44d71dfb8980034b3cea5c4d4",
		"ab864af9a7d4e4790db797fb5b00afbd6fc5acaff9f3e95443b961dda6829680930874e6a42156bf1f25c6a4891c6d",
		"ad924ae0a3d1fb311facc3f5142eb5f366d93c0f01f2f04f0db22ec8b1cb8786925b37eaa82219b94a23ddf1931b34fa",
		"ad924aefaad4af341fb0c3f0143ea8a728c1275b05bdff4916f92eccb6d6c286994672a9bd2356f15224cab9d1",
		"ad924af7a9cdaf3a1bb0c3f51227aaf37cde2b0f18f3e04911b267d8aacc85c89b4179fcbd29",
		"b39d1ee6e6cbe6210ea7c3e01e28a9bd6cc5690f1af2f4520bf561c8e3c68b9b824979eaac6c4ba4517d89f1ca",
		"bd9b1ffcb598e62a5aaa8bf65b0ea7a17cde6e4e03f9a64315b07cd7b7ca8b86910863e1a8381ea21f38c7f183006df6c2a5",
		"a59a0e6c462cf83113bd8bb31238e6be67c42bcded09ff4916f262c2e3c087c897085ae8a76019bc4671dabe8455"
	};

	for (auto& line : poem)
	{
		line = HexToString(line);
	}

	std::vector<std::string> result;
	GetEnscryptedPoem(poem, result, 0);
}

/////////////////////////// Fourth Cipher

size_t const MAX_COUNT_OF_REPEATS_WIDE = 100;

struct WideParent
{
	double fitnesValue;
	std::vector<std::vector<size_t>> keys;

	WideParent(std::vector<std::vector<size_t>> const key) : keys(key), fitnesValue(0) {}

	WideParent() {};
};

WideParent GetSingleCrossover(WideParent const& parent1, WideParent const& parent2)
{
	WideParent child;
	for (size_t j = 0; j < parent1.keys.size(); ++j)
	{
		bool filledKeys[keySize];
		std::fill(filledKeys, filledKeys + keySize, false);
		size_t countOfMismatch = 0;

		for (size_t i = 0; i < keySize; ++i)
		{
			size_t parentId = rand() % 2;
			WideParent const& currentParent = (parentId % 2 == 0 ? parent1 : parent2);
			if (!filledKeys[currentParent.keys[j][i]])
			{
				child.keys[j][i] = currentParent.keys[j][i];
				filledKeys[currentParent.keys[j][i]] = true;
			}
			else
			{
				WideParent const& otherParent = (parentId % 2 == 0 ? parent2 : parent1);
				if (!filledKeys[otherParent.keys[j][i]])
				{
					child.keys[j][i] = otherParent.keys[j][i];
					filledKeys[otherParent.keys[j][i]] = true;
				}
				else
				{
					++countOfMismatch;
					child.keys[j][i] = -1;
				}
			}
		}
		if (countOfMismatch)
		{
			std::vector<size_t> mismathes;
			mismathes.reserve(countOfMismatch);
			for (size_t i = 0; i < keySize; ++i)
			{
				if (!filledKeys[i])
				{
					mismathes.push_back(i);
				}
			}
			std::random_shuffle(mismathes.begin(), mismathes.end());

			for (auto& keyValue : child.keys[j])
			{
				if (keyValue == -1)
				{
					keyValue = mismathes[--countOfMismatch];
				}
			}
		}
	}

	return child;
}

void InitPopulation(std::vector<WideParent>& population, size_t const keysSize)
{
	std::vector<size_t> key(keySize);
	for (size_t i = 0; i < keySize; ++i)
	{
		key[i] = i;
	}
	for (size_t i = 0; i < MAX_POPULATION_SIZE; ++i)
	{
		std::vector<std::vector<size_t>> keys;
		for (size_t j = 0; j < keysSize; ++j)
		{
			std::random_shuffle(&key[0], &key[keySize]);
			keys.push_back(key);
		}
		population.emplace_back(keys);
	}
}


std::string EnscryptWide(std::string const& cipher, WideParent const& parent)
{
	std::string result;
	result.reserve(cipher.size());
	size_t const keysSize = parent.keys.size();

	for (size_t i = 0; i < cipher.size(); ++i)
	{
		char letter = cipher[i];
		size_t const keyId = i % keysSize;

		size_t const letterId = (size_t)letter - (size_t)'a';
		result.push_back((char)('a' + parent.keys[keyId][letterId]));
	}
	return result;
}


void ComputeFitnessForRangeWide(std::string const& cipher, std::vector<WideParent>::iterator begin, std::vector<WideParent>::iterator const& end, std::vector<std::map<std::string, double>> const * const vocabulary, std::vector<std::string> const * const wordVoc)
{
	for (; begin != end; ++begin)
	{
		WideParent& parent = *begin;
		parent.fitnesValue = GetFitnessResult(EnscryptWide(cipher, parent), *vocabulary, *wordVoc);
	}
}

void ComputeFitnessForAll(std::string const& cipher, std::vector<WideParent>& population, std::vector<std::map<std::string, double>> const& vocabulary, std::vector<std::string> const& wordVoc)
{
	size_t const THREAD_COUNT = 4;
	std::vector<std::thread> futures(THREAD_COUNT);
	size_t const sizeForOne = (COUNT_OF_CHILDREN - 1) / THREAD_COUNT + 1;
	std::vector<WideParent>::iterator const startIter = population.begin() + MIN_POPULATION_SIZE;
	for (size_t i = 0; i < THREAD_COUNT; ++i)
	{
		std::vector<WideParent>::iterator const endIter = (i == THREAD_COUNT - 1 ? population.end() : startIter + sizeForOne * (i + 1));
		futures[i] = std::thread(ComputeFitnessForRangeWide, cipher, startIter + sizeForOne * i, endIter, &vocabulary, &wordVoc);
	}

	for (auto& thread : futures)
	{
		thread.join();
	}
}

void Selection(std::vector<WideParent>& population)
{
	std::sort(population.begin(), population.end(), [](WideParent a, WideParent b) {return a.fitnesValue > b.fitnesValue; });
	population.erase(population.begin() + MIN_POPULATION_SIZE, population.end());
}

void MutateOne(WideParent& parent)
{
	for (size_t i = 0; i < parent.keys.size(); ++i)
	{
		size_t const keyValue1 = rand() % keySize;
		size_t keyValue2 = rand() % keySize;
		while (keyValue1 == keyValue2)
		{
			keyValue2 = rand() % keySize;
		}

		std::swap(parent.keys[i][keyValue1], parent.keys[i][keyValue2]);
	}
}

void Crossover(std::vector<WideParent>& population)
{
	float const partsToGet[] = { 0.0f, 0.01f, 0.02f, 0.1f, 0.4f, 1.0f };
	for (size_t i = 0; i < COUNT_OF_CHILDREN; ++i)
	{
		size_t const rangeToGet = rand() % 5;
		size_t const minIndex = partsToGet[rangeToGet] * MIN_POPULATION_SIZE;
		size_t const maxIndex = partsToGet[rangeToGet + 1] * MIN_POPULATION_SIZE;
		size_t const rangeSize = maxIndex - minIndex;
		size_t const indexToGet = rand() % rangeSize + minIndex;
		WideParent child = population[rangeToGet == 0 ? 0 : indexToGet];
		for (size_t j = 0; j < rand() % COUNT_OF_PARENT_MUTATIONS; ++j)
		{
			MutateOne(child);
		}
		population.push_back(child);
	}
}

void Mutation(std::vector<WideParent>& population)
{
	for (size_t i = MIN_POPULATION_SIZE; i < MAX_POPULATION_SIZE; ++i)
	{
		MutateOne(population[i]);
	}
}

bool TerminateWide(size_t const countOfReps)
{
	return countOfReps > MAX_COUNT_OF_REPEATS_WIDE;
}

void PrintResult(std::string const& cipher, std::vector<WideParent>& population)
{
	printf("Third: %s", EnscryptWide(cipher, population.front()).c_str());
	printf("Fitness value: %f", population.front().fitnesValue);
	getchar();
}

void FourthCipher(std::string const& cipher)
{
	float lastBestFunc = 0.0f;
	size_t countOfRepeats = 0;

	std::string cipherCopy = cipher;
	std::transform(cipherCopy.begin(), cipherCopy.end(), cipherCopy.begin(), ::tolower);
	auto voc2 = GetNGrams(MAX_N_GRAM_SIZE);
	auto vocabulary = GetVocabulary();

	size_t keysSize = GetLengthOfKey(cipher);

	std::vector<WideParent> population;
	InitPopulation(population, keysSize);

	for (size_t i = 0; i < MAX_POPULATION_SIZE; ++i)
	{
		WideParent& parent = population[i];
		parent.fitnesValue = GetFitnessResult(EnscryptWide(cipherCopy, parent), voc2, vocabulary);
	}

	size_t numOfPopultion = 0;

	while (true)
	{
		++numOfPopultion;

		Selection(population);
		printf("Number of population %d, best fitness %f, count of repeats %d\n", numOfPopultion, population.front().fitnesValue, countOfRepeats);

		if (abs(population.front().fitnesValue - lastBestFunc) < EPSILON)
		{
			++countOfRepeats;
		}
		else
		{
			lastBestFunc = population.front().fitnesValue;
			countOfRepeats = 0;
		}

		Crossover(population);
		Mutation(population);

		ComputeFitnessForAll(cipherCopy, population, voc2, vocabulary);

		if (Terminate(countOfRepeats))
		{
			PrintResult(cipherCopy, population);

			return;
		}
	}
}

int main()
{
	srand(time(NULL));
	FirstCipher("]|d3gaj3r3avcvrgz}t>xvj3K\A3pzc{va=3V=t=3zg3`{|f.w3grxv3r3`gaz}t31{v..|3d|a.w13r}w?3tzev}3g{v3xvj3z`31xvj1?3k|a3g{v3uza`g3.vggva31{13dzg{31x1?3g{v}3k|a31v13dzg{31v1?3g{v}31.13dzg{31j1?3r}w3g{v}3k|a3}vkg3p{ra31.13dzg{31x13rtrz}?3g{v}31|13dzg{31v13r}w3`|3|}=3J|f3~rj3f`v3z}wvk3|u3p|z}pzwv}pv?3[r~~z}t3wz`gr}pv?3Xr`z`xz3vkr~z}rgz|}?3`grgz`gzpr.3gv`g`3|a3d{rgveva3~vg{|w3j|f3uvv.3d|f.w3`{|d3g{v3qv`g3av`f.g=");
	SecondCipher(HexToString("1c41023f564b2a130824570e6b47046b521f3f5208201318245e0e6b40022643072e13183e51183f5a1f3e4702245d4b285a1b23561965133f2413192e571e28564b3f5b0e6b50042643072e4b023f4a4b24554b3f5b0238130425564b3c564b3c5a0727131e38564b245d0732131e3b430e39500a38564b27561f3f5619381f4b385c4b3f5b0e6b580e32401b2a500e6b5a186b5c05274a4b79054a6b67046b540e3f131f235a186b5c052e13192254033f130a3e470426521f22500a275f126b4a043e131c225f076b431924510a295f126b5d0e2e574b3f5c4b3e400e6b400426564b385c193f13042d130c2e5d0e3f5a086b52072c5c192247032613433c5b02285b4b3c5c1920560f6b47032e13092e401f6b5f0a38474b32560a391a476b40022646072a470e2f130a255d0e2a5f0225544b24414b2c410a2f5a0e25474b2f56182856053f1d4b185619225c1e385f1267131c395a1f2e13023f13192254033f13052444476b4a043e131c225f076b5d0e2e574b22474b3f5c4b2f56082243032e414b3f5b0e6b5d0e33474b245d0e6b52186b440e275f456b710e2a414b225d4b265a052f1f4b3f5b0e395689cbaa186b5d046b401b2a500e381d61"));
	ThirdCipher("EFFPQLEKVTVPCPYFLMVHQLUEWCNVWFYGHYTCETHQEKLPVMSAKSPVPAPVYWMVHQLUSPQLYWLASLFVWPQLMVHQLUPLRPSQLULQESPBLWPCSVRVWFLHLWFLWPUEWFYOTCMQYSLWOYWYETHQEKLPVMSAKSPVPAPVYWHEPPLUWSGYULEMQTLPPLUGUYOLWDTVSQETHQEKLPVPVSMTLEUPQEPCYAMEWWYOYULULTCYWPQLSEOLSVOHTLUYAPVWLYGDALSSVWDPQLNLCKCLRQEASPVILSLEUMQBQVMQCYAHUYKEKTCASLFPYFLMVHQLUHULIVYASHEUEDUEHQBVTTPQLVWFLRYGMYVWMVFLWMLSPVTTBYUNESESADDLSPVYWCYAMEWPUCPYFVIVFLPQLOLSSEDLVWHEUPSKCPQLWAOKLUYGMQEUEMPLUSVWENLCEWFEHHTCGULXALWMCEWETCSVSPYLEMQYGPQLOMEWCYAGVWFEBECPYASLQVDQLUYUFLUGULXALWMCSPEPVSPVMSBVPQPQVSPCHLYGMVHQLUPQLWLRPHEUEDUEHQMYWPEVWSSYOLHULPPCVWPLULSPVWDVWGYUOEPVYWEKYAPSYOLEFFVPVYWETULBEUF");
	FourthCipher("MULDCLZWKLZTWTXHJCXEDQLHLXIHBLLJDQWHDQUZSOBCKMEKWXRABUGADQZOBLQNNXMJSYXDWYZFNKINEUICNJNHSUTTNQZTQNGFCYYJCNEANNXHNKSADQZVDSEDCLRSWRZSAQMFMXOJSYCFSYETSQZSBOSNMSIVNCBCWRXVBGIBAGKFMLZTWTBBFUOJESIIBGEPBRNAKMYTYOSCWTZSBYSPCCZFCMBBXTIUBSXTNMENRNKADQRFCYQZVLKFUZYZNNJZMYKONUKFWAGFCYYJUCQNNYSCENBVKNTSWOSKCMRHFYYJKLXHECSKBZIAKJSHJNKWMAWFSNXHWRICYYYJESIIBYYTFHENNNLKCMRHBOZJZYINDHOFFYZSBKEPYNXHVUNKDTDJNZIHJQIGEHSVWHWZKEYZINZTMGIVJNQNAYNHAHIIBQXFQNZJMKLTLXSNNUMPAWSHJNCPVEZZSLKONCCZRLWBCUOHJNQTTKZQASSKNCBCQCXNENKABOTFCOXGDTNZKHWKJLLDBOCSDSEKNNQNINGFCNSCMSWVNCBCQNECATDABQXKWDFTSLZTWTXFUHSHNNQNATCAXOSQVUNGDMTZSYZFFNEPMKGFCLTZVYBWFNUCWZLVKLICNNJHDYZZMAENVUNKDTDWBQXNWDSEWSMNWSSUBTRSCLXJFRQFQYYJKLXHELQZTSEVJEQFGCMTSPZSBEQFTSEGRCZSEUXNAJKJEHETSYSYNEYPDQSNYUSNSUZGDASTNTBHDXZFQLZTMLLQFUOJESSVCUCJFQICTZBCNKNPNLXEBHK");
	//Additional();
}