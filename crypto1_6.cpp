// crypto1_6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
//#include <bitset>
#include <unordered_map>
#include <istream>
#include <fstream>
#include <vector>
//#include <algorithm>

#include <numeric>
#include <cmath>


float englishFreq[27] =
{
	0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228, 0.02015,
	0.06094, 0.06966, 0.00153, 0.00772, 0.04025, 0.02406, 0.06749,
	0.07507, 0.01929, 0.00095, 0.05987, 0.06327, 0.09056, 0.02758,
	0.00978, 0.02360, 0.00150, 0.01974, 0.00074, 0.1918182
};

int count = 0;
std::string b64ToHex(std::string str)
{
	std::string newStr = "";
	std::string ref = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+";

	//number of bytes per 12 bits
	int bytes = str.size() / 2;
	int padding = str.size() % 2;

	//padding must be either 0 or 1
	if (padding > 1)
	{
		return newStr;
	}

	//Number of characters to be encoded is 3

	int count = bytes * 2;

	unsigned long long h = 0;

	char x[10], y[10], z[10];
	int refNum;
	int i = 0;
	for (i = 0; i < count; i += 2)
	{
		//Get every 3 chars
		std::string a = str.substr(i, 1);
		std::string b = str.substr(i + 1, 1);


		int refNumOne = 0;
		int refNumTwo = 0;
		for (int i = 0; i < ref.length(); i++)
		{
			std::string c = ref.substr(i,1);
			if (a == c)
			{
				refNumOne = i;
			}
			if (b == c)
			{
				refNumTwo = i;
			}
		}
		h = refNumOne << 6; //shift left by 6 bits
		h |= refNumTwo; //

		sprintf(x, "%x", (int)(0xF & (h >> 8)));
		sprintf(y, "%x", (int)(0xF & (h >> 4)));
		sprintf(z, "%x", (int)(0xF & (h)));
		/*x = (char)(0xF & (h >> 8));
		y = (char)(0xF & (h >> 4));
		z = (char)(0xF & h);
		*/
		//HEX: 0x3F -> DEC: 63 -> b64: ?
		newStr += x; //first hex char; shift right by 8
		newStr += y; //ref[0xF & (h >> 4)]; //secondd hex char
		newStr += z; //ref[0xf & (h)];
		/*char xorChar[100];
			sprintf(xorChar, "%x", xorByte);
		*/
	}
	/*if (padding == 1)
	{
		refNum =
		h = strtoull(a, nullptr, 16) << 8;
		newStr += ref[0x3F & (h >> 6)];
		newStr += "=";
	}*/


	//std::cout << newStr;

	return newStr;


}

float getChi(std::string str)
{
	int ignored = 0;
	float count[27];
	for (int i = 0; i < 26; i++)
	{
		count[i] = 0;
	}
	for (int j = 0; j < str.length(); j++)
	{
		int c = (int)str[j];
		if (c >= 65 && c <= 90) {
			count[c - 65]++;
		}
		else if (c >= 97 && c <= 122)
		{
			count[c - 97]++;
		}
		if (c == 32) { //considering "space" the 27th letter of the alphabet
			count[26]++;
		}
		else if (c > 32 && c <= 126)
		{
			ignored++;
		}
		else if (c == 9 || c == 10 || c == 13)
		{
			ignored++;
		}
		else return 0;
	}

	float chi = 0; float len = str.length() - ignored;
	for (int k = 0; k < 26; k++)
	{
		float observed = count[k];
		float expected = len * englishFreq[k];
		float diff = observed - expected;
		chi += diff * diff / expected;
	}
	return chi;
}

int singlebyteXORASCII(std::string str)
{

	//Need to grab every hexadecimal to use to xor and give score
	std::string asciiStr;
	int key = 0;
	for (int i = 0; i < 256; i++)
	{
		std::string xorStr = "";

		//Convert char into decimal and xor
		for (int j = 0; j < str.length(); j += 2)
		{
			unsigned int h;
			unsigned int p;
			unsigned int xorByte;

			//char a[2] = { str[j], 0 };
			//biStr = str.substr(j, 2);

			h = (unsigned int) str[j];
			p = (unsigned int)i;


			xorByte = h ^ p;
			//std::cout << xorByte << std::endl;

			/*char xorChar[100];
			sprintf(xorChar, "%x", xorByte);
			*/

			char xorChar = (char)xorByte;
			xorStr += xorChar;
			//std::cout << xorStr << std::endl;

			//std::bitset<64> x(xorByte);
			//std::cout << x << '\n';
		}
		//std::cout << xorStr << std::endl;
		float chi = getChi(xorStr);
		int key = i;
		std::string plaintext = xorStr;
		int goodChars = 0;
		int c;
		for (int k = 0; k < plaintext.length(); k++) {
			c = (int) plaintext[k];
			if (c >= 65 && c <= 90) {
				goodChars++;
			}
			else if (c >= 97 && c <= 122)
			{
				goodChars++;
			}
			else if (c == 32) { //considering "space" the 27th letter of the alphabet
				goodChars++;
			}
		}

		float admissableNum = plaintext.length() * 0.85;

		std::unordered_map<std::string, int> m;
		if (chi > 0 && goodChars >= admissableNum)
		{
			//std::cout << "entered" << std::endl;
			m = {
					{plaintext, key}
			};
		}
		for (const auto& n : m) {
			//std::cout << "entered" << std::endl;
			//std::cout << "Plaintext: [" << n.first << "] Key:[" << n.second << "]\n";
			return n.second;

		}


		//std::cout << asciiStr << std::endl;
	}
	return 1000;
}

int repeatingXOR(std::string str, std::string key)
{

	//Need to grab every hexadecimal to use to xor and give score
	std::string asciiStr;
	std::string uniStr = "";
	std::string xorStr = "";
	std::string biXorStr = "";

	asciiStr = "";

	int keyLen = key.length();
	int leftOff = 0;
	unsigned int xorByte;
	unsigned int h;
	unsigned int p;

	//Convert char into decimal and xor
	for (int i = 0; i < str.length(); i += keyLen)
	{
		//char a[2] = { str[j], 0 };

		for (int j = 0; j < keyLen; j++)
		{
			if (i < str.length() - keyLen) {
				//uniStr = str.substr(i + j, 1);
				h = (unsigned int)str[i + j];
				//h = strtol(uniStr.c_str(), nullptr, 16);
				p = (unsigned int)key[j];

				xorByte = h ^ p;
				//char xorChar[100];
				//sprintf(xorChar, "%x", xorByte);


				//char xorChar = (char)xorByte;
				xorStr += (char)xorByte;
			}
			else {
				leftOff = i;
			}
		}

	}
	for (int k = 0; k < (str.length() - leftOff) - 1; k++) {
		//std::cout << i << " " << k << std::endl;
		//std::cout << (str.length() - i) << std::endl;
		h = (unsigned int)str[leftOff + k];
		p = (unsigned int)key[k];

		xorByte = h ^ p;
		xorStr += (char)xorByte;
	}

	std::cout << xorStr << std::endl;
	return 0;
}

unsigned countBits(unsigned int number)
{
	// log function in base 2
	// take only integer part
	return (int)log2(number) + 1;
}

unsigned int countSetBits(unsigned int n)
{
	unsigned int count = 0;
	while (n)
	{
		count += n & 1;
		n >>= 1;
	}
	return count;
}

int hammingDistance(std::string strOne, std::string strTwo)
{
	unsigned int tempOne;
	unsigned int tempTwo;
	unsigned int diffByte;
	unsigned int countDiff;
	unsigned int diff = 0;
	for (int i = 0; i < strOne.size(); i++)
	{
		tempOne = (unsigned int) strOne[i];
		tempTwo = (unsigned int) strTwo[i];
		diffByte = tempOne ^ tempTwo;
		countDiff = countSetBits(diffByte);
		diff += (countDiff);
	}
	//std::cout << diff << std::endl;
	return diff;
}

std::string breakX0R(std::string str)
{

	int keySize;
	std::string strFirst;
	std::string strSecond;
	int editDist;
	float normDist;
	std::vector<float> normVectors;
	int maxKeySize = (str.length() / 2) + 1;
	float avgNorm;
	int avgSum;
	float normVectorSum;
	std::unordered_map<float, int> m;
	std::vector<std::unordered_map<float, int>> minKeyMap;
	std::vector<int> keyVector;
	std::string key = "";
	char keyChar;

//	std::cout << str << std::endl;
	//std::cout << str.length() << std::endl;
	//std::cout << maxKeySize << std::endl;
	for (int i = 2; i < 41; i++) {
		//std::cout << i << std::endl;
		keySize = i;
		normVectors.clear();
		for (int j = 0; (j+keySize + keySize) < str.length(); j+=2*keySize)
		{
			//std::cout << j << std::endl;
			strFirst = str.substr(j, keySize);
			strSecond = str.substr(j+keySize, keySize);
			editDist = hammingDistance(strFirst, strSecond);
			//std::cout << editDist << std::endl;
			normDist = ((float)editDist / (float) keySize);
			normVectors.push_back(normDist);
		}
		normVectorSum = std::accumulate(normVectors.begin(), normVectors.end(), 0.0f);
		avgNorm = normVectorSum / (float)normVectors.size();
		m = {
				{avgNorm, keySize}
		};
		minKeyMap.push_back(m);
	}
	float minNorm = 100.0;
	int bestKey = 0;;
	for (int k = 0; k < minKeyMap.size(); k++) {
		for (const auto& n : minKeyMap[k])
		{
			if (n.first < minNorm) {
				minNorm = n.first;
				bestKey = n.second;
			}
		}
	}

	//std::cout << "Minimum Norm: [" << minNorm << "] Key:[" << bestKey << "]\n";

	keySize = bestKey;

	std::string skipBlock;
	std::string fullBlock;
	std::vector<std::string> keyedStrings;
	std::string keyedString = "";

	for (int j = 0; j < keySize; j++)
	{
		for (int k = j; k < str.length(); k += keySize) {
			skipBlock = str.substr(k, 1);
			keyedString += skipBlock;
		}
		keyedStrings.push_back(keyedString);
		keyedString = "";
	}
	//std::cout << keyedStrings.size() << std::endl;
	for (int l = 0; l < keyedStrings.size(); l++) {
		//std::cout << "\nBlock Number: " << l << std::endl;
		keyChar = (char) singlebyteXORASCII(keyedStrings[l]);
		key += keyChar;
	}

	//std::cout << key << std::endl;
	return key;
}

std::string hexToASCII(std::string str)
{
	std::string asciiStr;
	std::string biStr = "";

	asciiStr = "";

	//Convert char into decimal and xor
	for (int j = 0; j < str.length(); j += 2)
	{
		unsigned int h;
		unsigned int p;
		unsigned int xorByte;

		//char a[2] = { str[j], 0 };
		biStr = str.substr(j, 2);

		h = strtol(biStr.c_str(), nullptr, 16);

		char asciiChar = (char)h;
		asciiStr += asciiChar;
		//std::cout << xorStr << std::endl;

		//std::bitset<64> x(xorByte);
		//std::cout << x << '\n';
	}
	return asciiStr;
}


bool getFileContent(std::string fileName, std::vector<std::string>& vecOfStrs)
{

	// Open the File
	std::ifstream in(fileName.c_str());

	// Check if object is valid
	if (!in)
	{
		std::cerr << "Cannot open the File : " << fileName << std::endl;
		return false;
	}

	std::string str;
	// Read the next line from File untill it reaches the end.
	while (std::getline(in, str))
	{
		// Line contains string of length > 0 then save it in vector
		if (str.size() > 0)
			vecOfStrs.push_back(str);
	}
	//Close The File
	in.close();
	return true;
}

int main()
{

	std::vector<std::string> vecOfStr;
	std::vector<std::string> vecOfASCII;
	std::string fullASCIIString = "";

	// Get the contents of file in a vector
	bool result = getFileContent("crypto1_6.txt", vecOfStr);

	for (int i = 0; i < vecOfStr.size(); i++) {
		std::string b64Str = vecOfStr[i];
		std::string hexStr = b64ToHex(b64Str);
		std::string asciiStr = hexToASCII(hexStr);
		vecOfASCII.push_back(asciiStr);
	}

	for (std::string& line : vecOfASCII) {
		//std::cout << line << std::endl;
		//std::cout << line.length() << std::endl;
		fullASCIIString += line;
	}

	//std::cout << breakX0R(fullASCIIString);
	repeatingXOR(fullASCIIString, breakX0R(fullASCIIString));
}

