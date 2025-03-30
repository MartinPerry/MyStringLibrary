#include "./MyStringUtils.h"

#include <locale.h>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cfloat>
#include <cmath>

#include <unordered_set>
#include <deque>

#include "./MyStringAnsi.h"


/// <summary>
/// Simple conversion of double value to string
/// with given number of fractional places
/// (Does not support "e" notation, output number is always XX.YY)
/// </summary>
/// <param name="val"></param>
/// <param name="fractPlaces"></param>
/// <returns></returns>
MyStringAnsi MyStringUtils::ToStringSimple(double val, int fractPlaces)
{
	bool negative = val < 0;

	if (negative) val *= -1;
	uint64_t intPart = static_cast<uint64_t>(val);

	MyStringAnsi res = (negative) ? "-" : "";
	res += intPart;

	if (fractPlaces > 0)
	{
		if (fractPlaces <= 7)
		{
			uint64_t fractPartReverse = MyStringUtils::GetFractPartAsBuffer(val, intPart, fractPlaces);
			if (fractPartReverse)
			{
				res += '.';
				char * buf = reinterpret_cast<char *>(&fractPartReverse);
				res.Append(buf, fractPlaces);
			}
		}
		else
		{
			uint64_t fractPartReverse = MyStringUtils::GetFractPartReversed(val, intPart, fractPlaces);
			if (fractPartReverse)
			{
				res += '.';

				while (fractPartReverse)
				{
					int cc = (fractPartReverse % 10);
					fractPartReverse /= 10;
					res += (char)(cc + '0');
				}
			}
		}
	}

	return res;
}

/// <summary>
/// Get fractional part of number in reversed order
/// Eg: for 45.789 => 987
/// </summary>
/// <param name="val">number</param>
/// <param name="intPart">integer part of number</param>
/// <param name="fractPlaces">how many places we want</param>
/// <returns></returns>
uint64_t MyStringUtils::GetFractPartReversed(double val, uint64_t intPart, int fractPlaces) noexcept
{
	double decimalMult = std::pow(10, fractPlaces);

	double fractPart = val - intPart;

	//reversed number is without leading zeroes
	//eg: 0.0157 => will reverse to 751
	//but we need one leading zero
	uint64_t fractPartReverse = MyStringUtils::ReversDigits((uint64_t)(fractPart * decimalMult));
	if (fractPartReverse == 0)
	{
		return fractPartReverse;
	}

	//calculate number of leading zeroes
	int fractLeadingZeros = 0;
	while (fractPart < 1)
	{
		fractLeadingZeros++;
		fractPart *= 10;
	}
	fractLeadingZeros--;

	//move reversed part 
	//751 => 7510
	while (fractLeadingZeros > 0)
	{
		fractPartReverse *= 10;
		fractLeadingZeros--;
	}

	return fractPartReverse;
}

/// <summary>
/// Get fractional part directly as a buffer (stored inside uint64_t)
/// This can support maximal 7 fractPlaces
/// </summary>
/// <param name="val"></param>
/// <param name="intPart"></param>
/// <param name="fractPlaces"></param>
/// <returns></returns>
uint64_t MyStringUtils::GetFractPartAsBuffer(double val, uint64_t intPart, int fractPlaces) noexcept
{
	double decimalMult = std::pow(10, fractPlaces);

	double fractPart = val - intPart;
	uint64_t fractInt = (uint64_t)(fractPart * decimalMult);

	uint64_t res = 0x3030303030303030; //fill buffer with '0'
	uint8_t * buf = reinterpret_cast<uint8_t *>(&res);

	buf[fractPlaces] = 0; //set terminating NULL to last fractPlace
	
	while (fractInt > 0)
	{
		fractPlaces--;
		buf[fractPlaces] = (uint8_t)(fractInt % 10 + '0');
		fractInt = fractInt / 10;
	}

	return res;
}

/// <summary>
/// Reverse digits in input number
/// Input: 123
/// Output: 321
/// </summary>
/// <param name="num"></param>
/// <returns></returns>
uint64_t MyStringUtils::ReversDigits(uint64_t num) noexcept
{
	if (num < 10) return num;

	uint64_t rev_num = 0;
	while (num > 0)
	{
		rev_num = rev_num * 10 + num % 10;
		num = num / 10;
	}
	return rev_num;
}

/// <summary>
/// Simple load for CSV
/// CSV is splitted with delimeter, but this method
/// does not handle delimeter inside strings ""
/// </summary>
/// <param name="fileName"></param>
/// <param name="delim"></param>
/// <returns></returns>
std::vector<std::vector<MyStringAnsi>> MyStringUtils::LoadCsv(const char* fileName, char delim)
{
	std::vector<std::vector<MyStringAnsi>> res;

	MyStringAnsi tmp = MyStringAnsi::LoadFromFile(fileName);

	auto lines = tmp.Split({ '\n', '\r' });

	for (auto & line : lines)
	{
		res.emplace_back(line.Split(delim));
	}

	return res;
}


//====================================================================
// Searching
//====================================================================

/// <summary>
/// Perfrom Boyer-Moore searching. Last function can be passed in
/// in "last". If "last" is NULL, last function is calculated
/// and filled to "last" array
/// !Important!"last" array must be freed outside this method
/// </summary>
/// <param name="haystack">where to find</param>
/// <param name="needle">text to find</param>
/// <param name="last">pointer to array of last function (in / out)</param>
/// <param name="start">start position of searching (default: 0)</param>
/// <returns>position of occurence needle in haystack</returns>
size_t MyStringUtils::SearchBoyerMoore(MyStringView haystack, MyStringView needle, size_t*& last, size_t start)
{
	const char* str = haystack.c_str();
	size_t needleLen = needle.length();


	if (needleLen == 0)
	{
		return MyStringUtils::npos;
	}

	size_t strLength = haystack.length();

	if (last == nullptr)
	{
		last = new size_t[static_cast<int>(std::numeric_limits<uint8_t>::max()) + 1];
		size_t val = MyStringUtils::npos;
		std::fill(last, last + std::numeric_limits<uint8_t>::max(), val);
		for (size_t i = 0; i < strLength; i++)
		{
			last[static_cast<uint8_t>(str[i])] = i;
		}
	}

	size_t index = needleLen - 1;
	size_t cmpIndex = index;
	index += start;
	while (index < strLength)
	{
		if (str[index] == needle[cmpIndex])
		{
			if (cmpIndex == 0)
			{
				return index;
			}

			index--;
			cmpIndex--;
		}
		else
		{
			size_t offset = last[static_cast<int>(str[index])];
			index = index + needleLen - ((cmpIndex < offset + 1) ? cmpIndex : offset + 1);
			cmpIndex = needleLen - 1;
		}
	}


	return MyStringUtils::npos;

}

size_t* MyStringUtils::BuildBoyerMooreHorspoolLookup(MyStringView needle)
{
	size_t needleLen = needle.length();

	size_t* lookUp = new size_t[static_cast<int>(std::numeric_limits<uint8_t>::max()) + 1];
	std::fill(lookUp, lookUp + std::numeric_limits<uint8_t>::max() + 1, needleLen);

	for (int i = 0; i < needleLen - 1; i++)
	{
		lookUp[static_cast<uint8_t>(needle[i])] = needleLen - 1 - i;
	}

	return lookUp;
}


bool MyStringUtils::IsSame(const char* str1, const char* str2, size_t len)
{
	size_t i = len - 1;

	while (str1[i] == str2[i])
	{
		if (i == 0)
		{
			return true;
		}
		i--;
	}
	return false;
}

/// <summary>
/// Perfrom Boyer-Moore-Horspool searching. Lookup function can be passed in
/// in "lookUp". If "lookUp" is NULL, lookUp function is calculated
/// and filled to "lookUp" array
/// !Important!"lookUp" array must be freed outside this method
/// </summary>
/// <param name="haystack">where to find</param>
/// <param name="needle">text to find</param>
/// <param name="last">pointer to array of lookUp function (in / out)</param>
/// <param name="start">start position of searching (default: 0)</param>
/// <returns>position of occurence needle in haystack</returns>
size_t MyStringUtils::SearchBoyerMooreHorspool(MyStringView haystack, MyStringView needle, size_t*& lookUp, size_t start)
{
	size_t haystackLen = haystack.length();
	size_t needleLen = needle.length();

	if (needleLen == 0)
	{
		return MyStringUtils::npos;
	}


	if (lookUp == nullptr)
	{
		lookUp = MyStringUtils::BuildBoyerMooreHorspoolLookup(needle);
	}
	
	size_t skip = start;
	while (haystackLen - skip >= needleLen)
	{
		if (IsSame(haystack.c_str() + skip, needle.c_str(), needleLen))
		{
			//found at skip
			return skip;
		}

		auto c = static_cast<uint8_t>(haystack[skip + needleLen - 1]);
		skip += lookUp[c];
	}

	return MyStringUtils::npos;
}


/// <summary>
/// Calculate last function lookup
/// Pass prealocated buffer for lookup function
/// or nullptr.
/// If nullptr is passed, lookup function is allocated and must be 
/// freed outside this method
/// </summary>
/// <param name="needle"></param>
/// <param name="failFce"></param>
size_t* MyStringUtils::BuildKnuthMorisPratBuildFailLookup(MyStringView needle)
{
	size_t needleLen = needle.length();

	size_t* failFce = new size_t[needleLen];	
	
	//buil Fail fce
	failFce[0] = 0;
	size_t index = 1;
	size_t cmpIndex = 0;

	while (index < needleLen)
	{
		if (needle[index] == needle[cmpIndex])
		{
			failFce[index] = failFce[index - 1] + 1;
			cmpIndex++;
		}
		else
		{
			failFce[index] = 0;
			if ((failFce[index - 1] != 0) && (cmpIndex != 0))
			{
				cmpIndex = 0;
				index--;
			}
		}
		index++;
	}

	return failFce;
}

/// <summary>
/// Perfrom KMP (KnuthMorisPrat) searching. Last function can be passed in
/// in "last". If "last" is NULL, last function is calculated
/// and filled to "last" array
/// !Important!"last" array must be freed outside this method
/// </summary>
/// <param name="haystack">where to find</param>
/// <param name="needle">text to find</param>
/// <param name="last">pointer to array of last function (in / out)</param>
/// <param name="start">start position of searching (default: 0)</param>
/// <returns>position of occurence needle in haystack</returns>
size_t MyStringUtils::SearchKnuthMorisPrat(MyStringView haystack, MyStringView needle, size_t*& last, size_t start)
{
	size_t needleLen = needle.length();

	if (needleLen == 0)
	{
		return MyStringUtils::npos;
	}

	size_t index = 1;
	size_t cmpIndex = 0;	
	size_t strLen = haystack.length();
	const char* str = haystack.c_str();

	if (last == nullptr)
	{
		last = MyStringUtils::BuildKnuthMorisPratBuildFailLookup(needle);
	}

	index = start;
	cmpIndex = 0;
	while (index < strLen)
	{
		if (str[index] == needle[cmpIndex])
		{
			index++;
			cmpIndex++;
			if (cmpIndex == needleLen)
			{

				return (index - cmpIndex);
			}
		}
		else
		{
			if (cmpIndex == 0)
			{
				index++;
			}
			else
			{
				cmpIndex = last[cmpIndex - 1];
			}
		}
	}


	return MyStringUtils::npos;
}

/// <summary>
/// Perfrom BF searching. Suitable for small strings
/// </summary>
/// <param name="haystack">where to find</param>
/// <param name="needle">text to find</param>
/// <param name="start">start position of searching (default: 0)</param>
/// <returns>position of occurence needle in haystack</returns>
size_t MyStringUtils::SearchBruteForce(MyStringView haystack, MyStringView needle, size_t start)
{
	size_t needleLen = needle.length();
	size_t i = start;
	size_t j = 0;
	size_t lastPos = MyStringUtils::npos;
	size_t strLen = haystack.length();
	const char* str = haystack.c_str();

	while (i < strLen)
	{
		j = 0;
		while (j < needleLen)
		{
			if (str[i] == needle[j])
			{
				i++;
				j++;
				lastPos = i;
			}
			else
			{
				i++;
				lastPos = MyStringUtils::npos;
				break;
			}
		}

		if (lastPos != MyStringUtils::npos)
		{
			return (lastPos - needleLen);
		}
	}

	return MyStringUtils::npos;
}

//===========================================================================================
//===========================================================================================
//===========================================================================================


AhoCorasick::AhoCorasick() :
	root(new TrieNode()),
	failCreated(false)
{
}

AhoCorasick::~AhoCorasick()
{
	this->Release();
	delete root;
}

void AhoCorasick::Release()
{
	std::unordered_set<TrieNode*> visited;

	std::deque<TrieNode*> q;
	q.emplace_back(root);
	visited.emplace(root);

	while (!q.empty())
	{
		TrieNode* currentNode = q.front();
		q.pop_front();

		for (const auto& [_, child] : currentNode->children)
		{
			q.emplace_back(child);
			visited.emplace(child);
		}
	}

	for (auto v : visited)
	{
		delete v;
	}

	root = new TrieNode();
	failCreated = false;

	
	nodePatterns.clear();
}


void AhoCorasick::AddPattern(const std::string& pattern)
{
	this->AddPattern(pattern.c_str(), pattern.length());
}

void AhoCorasick::AddPattern(const char* pattern, size_t patternLength)
{
	TrieNode* node = root;
	for (size_t i = 0; i < patternLength; i++)
	{
		node = node->try_emplace_child(pattern[i]);
	}

	//node->output.push_back(pattern);
	node->hasOutput = true;

	auto it = nodePatterns.try_emplace(node);
	it.first->second.emplace_back(pattern);

	
	failCreated = false;
}

void AhoCorasick::BuildFailTransitions()
{
	if (failCreated)
	{
		return;
	}

	std::deque<TrieNode*> queue;
	for (const auto& [_, node] : root->children)
	{
		queue.emplace_back(node);
		node->fail = root;
	}

	while (!queue.empty())
	{
		TrieNode* currentNode = queue.front();
		queue.pop_front();

		for (const auto& [c, child] : currentNode->children)
		{
			queue.emplace_back(child);

			TrieNode* failNode = currentNode->fail;

			while ((failNode != root) && (failNode->has_child(c) == false))
			{
				failNode = failNode->fail;
			}

			if (auto childNode = failNode->find_child(c))
			{
				child->fail = childNode;
			}
			else
			{
				child->fail = root;
			}


			auto failPatterns = nodePatterns.find(child->fail);
			if (failPatterns != nodePatterns.end())
			{
				child->hasOutput = (failPatterns->second.size() > 0);

				auto jt = nodePatterns.try_emplace(child);
				jt.first->second.insert(
					jt.first->second.end(),
					failPatterns->second.begin(),
					failPatterns->second.end()
				);
			}

			/*
			child->output.insert(
				child->output.end(),
				std::make_move_iterator(child->fail->output.begin()),
				std::make_move_iterator(child->fail->output.end())
			);
			*/
		}
	}

	this->failCreated = true;
}

bool AhoCorasick::ContainsPatterns(const std::string& haystack)
{
	this->BuildFailTransitions();

	TrieNode* node = root;

	size_t i = 0;
	while (i < haystack.length())
	{
		char c = haystack[i];

		while ((node != root) && (node->has_child(c) == false))
		{
			node = node->fail;
		}

		auto tmp = node->find_child(c);
		if (tmp != nullptr)
		{
			node = tmp;			
		}
		

		if (node->hasOutput)
		{
			return true;
			/*
			auto it = nodePatterns.find(node);
			if (it != nodePatterns.end())
			{
				return true;
			}
			*/
		}

		i++;
	}

	return false;
}

void AhoCorasick::SearchPatterns(const std::string& haystack)
{
	this->BuildFailTransitions();

	TrieNode* node = root;
	//std::vector<std::string> patterns_found;

	size_t i = 0;
	while (i < haystack.length())
	{
		char c = haystack[i];

		while ((node != root) && (node->has_child(c) == false))
		{
			node = node->fail;
		}

		auto tmp = node->find_child(c);
		if (tmp != nullptr)
		{
			node = tmp;
		}

		if (node->hasOutput)
		{
			auto it = nodePatterns.find(node);
			if (it != nodePatterns.end())
			{
				//todo
				//printf("Pattern(s) found at index %d:\n", i);

				for (const auto& s : it->second)
				{
					//printf("=> %s\n", s.c_str());
				}
			}
		}

		i++;

		/*
		patterns_found.insert(patterns_found.end(), node->output.begin(), node->output.end());

		if (!patterns_found.empty())
		{
			printf("=> Pattern(s) found at index %d:", index);

			for (const auto& s : patterns_found)
			{
				printf("=> %s\n", s.c_str());
			}
		}
		*/
	}
}