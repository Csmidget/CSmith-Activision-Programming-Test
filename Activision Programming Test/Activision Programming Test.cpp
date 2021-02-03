#include <iostream>
#include <fstream>

// Some assumptions made:
// 1. Only alphabetical characters a-z are supposed to appear on padlocks.
// 2. All external files are in ASCII format.
// 3. Words in the dictionary are no longer than 255 characters.

// Solution Explanation:
// As only alphabetical characters are used, there are 26 possible values for each letter
// (After ensuring it is lower case). My solution represents each wheel as 26 bools,
// one for each letter in the alphabet. These are set to true if the wheel contains the letter.
//
// Each character of a word is then tested against the corresponding wheel with a 
// quick check against that characters index in the array. As soon as a mismatch is found,
// it moves on.
//
// If a word is shorter than the number of wheels it will shift along the lock and test again
// until the last character of the word is aligned with the last wheel of the lock.
//
// I believe this solution has complexity ~O(n*log(m)) where n is the word count and m is the wheel count.
// The number of letters on each wheel has no significant effect.

#define ALPHABETLENGTH 26
#define MAXWORDLENGTH  255

typedef unsigned int uint;

// This const expression converts letters from a lower case value
// to an index from 0-25, indicating it's position in the alphabet.
constexpr int alphabetindex(char letter) { return letter - 'a'; }

// This function could just return a bool and exit as soon as it finds a match
// but to ensure any duplicate matches are found it continues until all possible
// positions have been tested.
int TestWord(bool** wheels, const char* word, const uint wordLength, const uint wheelCount)
{
	uint count{};

	// How many more wheels there are than characters in the word.
	uint space{ wheelCount - wordLength };

	// After each test, shift along the lock one wheel and test again.
    for (size_t i = 0; i <= space; i++)
    {
		bool match{true};

        for (size_t j = 0; j < wordLength; j++)
        {
            if (!wheels[i + j][alphabetindex(word[j])])
            {
                match = false;
                break;
            }
        }

        if (match)
            ++count;
    }

    return count;
}

// A lot of exception handling in this to catch the most common mistakes and prevent the application
// from hard crashing.
void ReadWheelFile(bool**& wheels, char*& letters, uint& wheelCount, uint& lettersPerWheel)
{
	std::ifstream wheelFile{ "wheels.txt" };
	if (!wheelFile.good())
		throw std::exception("ERROR: Unable to open wheel file. Ensure wheels.txt is located in the same directory as the executable.");

	// Read in wheel count and initialize wheel grid.
	wheelFile >> wheelCount;

	if (wheelCount == 0)
		throw std::exception("ERROR: Invalid value for wheel count in wheels.txt. Expecting number greater than 0.");

	wheels = new bool* [wheelCount];
	for (int i = 0; i < wheelCount; ++i)
		wheels[i] = new bool[ALPHABETLENGTH] { false };


	// Read in letters per wheel and populate wheel grid with data.
	wheelFile >> lettersPerWheel;

	if (lettersPerWheel == 0)
		throw std::exception("ERROR: Invalid value for letters per wheel in wheels.txt. Expecting number greater than 0.");

	// Leave one space for terminating character.
	letters = new char[lettersPerWheel + 1]{'\0'};

	// Move to the first wheel. ( '>>' does not move us to the next line after reading)
	wheelFile.getline(letters, 1);

	for (size_t i = 0; i < wheelCount; i++)
	{
		wheelFile.getline(letters,lettersPerWheel + 1);

		if (wheelFile.fail())
			throw std::exception("ERROR: Wheel contained too many letters.");

		for (size_t j = 0; j < lettersPerWheel; j++)
		{
			if (!isalpha(letters[j]))
			{
				letters[j] == '\0' ? 
					throw std::exception("ERROR: Wheel contained insufficient letters.") :
					throw std::exception("ERROR: Non-alphabetical character found in combination. Ensure only characters a-z or A-Z are used.");
			}

			// Convert to lower case, and then into an index in the wheel array.
			wheels[i][alphabetindex(tolower(letters[j]))] = true;
		}
	}
}

// Reads in the dictionary file and tests each word against the lock.
int ProcessDictionary(bool**& wheels,const uint wheelCount,const uint lettersPerWheel)
{
	// Leave one space for terminating character.
	char word[MAXWORDLENGTH + 1] {'\0'};
	uint count{};
	uint matches{};

	std::ifstream dictionaryFile{ "dictionary.txt" };

	if (!dictionaryFile.good())
		throw std::exception("ERROR: Unable to open dictionary file. Ensure dictionary.txt is located in the same directory as the executable.");

	// Read each word in from the dictionary and test it against the lock.
	while (!dictionaryFile.eof() && dictionaryFile.good())
	{
		dictionaryFile.getline(word,MAXWORDLENGTH + 1);

		if (dictionaryFile.fail())
			throw std::exception("ERROR: Word in dictionary exceeded maximum length!");

		uint length{ (uint)strlen(word) };

		// Convert the word to lower case, and check that only
		// alphabetical characters are used while we're at it.
		bool validWord{ true };
		for (int i = 0; i < length; ++i)
		{
			validWord = validWord && isalpha(word[i]);
			word[i] = tolower(word[i]);
		}

		if (!validWord)
			continue;

		// If the word is longer than the number of wheels
		// it is impossible for it to be a combination.
		if (length > wheelCount)
			continue;

		matches = TestWord(wheels, word, length, wheelCount);
		if (matches > 0)
		{
			std::cout << word << std::endl;
			count += matches;
		}
	}

	std::cout << "Found " << count << " words." << std::endl;
}

int main()
{
	// Declaring these here so that they can be
	// cleaned up if an exception occurs.
	bool** wheels = nullptr;
	char* letters = nullptr;
	uint wheelCount{};
	uint lettersPerWheel{};

    try
    {
		ReadWheelFile(wheels, letters, wheelCount, lettersPerWheel);
		ProcessDictionary(wheels, wheelCount, lettersPerWheel);
    }
	catch(std::exception e)
	{
		std::cout << e.what() << std::endl;
	}

	std::cout << "Hit enter to exit." << std::endl;
	getchar();

	// Clean up any used heap memory.
	delete[] letters;

	for (auto i = 0; i < wheelCount; ++i)
	{
		delete[] wheels[i];
	}
	delete[] wheels;
}
