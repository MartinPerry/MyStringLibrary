#include "./Localization.h"

//#define USE_VFS

#ifdef _MSC_VER
#	ifndef my_fopen 
#		define my_fopen(a, b, c) fopen_s(a, b, c)	
#	endif
#else
#	ifndef my_fopen 
#		define my_fopen(a, b, c) (*a = fopen(b, c))
#	endif
#endif

#ifndef NOMINMAX
#   define NOMINMAX
#endif

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <algorithm>


#include "./cJSON_JS.h"

#if defined(_WIN32) && defined(_DEBUG)
#   define GENERATE_CODE
#endif

#ifdef GENERATE_CODE
#	ifdef _WIN32
#		include "./win_dirent.h"
#	else 
#		include <sys/stat.h>
#	   include <dirent.h>
#	endif
#endif


#ifdef USE_VFS
#   include "../Utils/VFS/VFS.h"
#endif

#if __has_include("generated_localization.h")
#	include "generated_localization.h"
#endif

//Limitation: translated string cannot contain {}
//These symbols are reserved for "translation replacement"
//eg: 
//active_label = Currently is active {active_id}
//active_id = something
//this will end in "Currently is active something"




Localization::Localization(Localization::StringView lang,
	Localization::StringView defLang,
	Localization::StringView defPath) :
	DEFAULT_LANGUAGE(defLang),
	DEFAULT_PATH(defPath),
	lang("")
{	
	this->GenerateSupportedLanguagesList();
    this->SetLang(lang);
}



Localization::~Localization()
{
}

void Localization::AddObserver(ILocalizationObserver* observer)
{
	this->observers.push_back(observer);
}

void Localization::RemoveObserver(ILocalizationObserver* observer)
{
	this->observers.remove(observer);
}

const std::map<Localization::StringView, Localization::StringView> & Localization::GetAllSupportedLanguages() const
{
#ifdef GENERATED_LANG_LIST_H
	return SUPPORTED_LANG_LIST;
#elif defined(GENERATE_CODE)
	//here we return address of local variable
	//but it is OK, because we use GENERATE_CODE mode to generate correct header
	return {};
#else
#	error List of languages not found
#endif
}

void Localization::GenerateSupportedLanguagesList()
{
#if defined(GENERATE_CODE) && !defined(GENERATED_LANG_LIST_H)

	std::map<String, String> langList;

	DIR *dir;
	struct dirent * ent;

	dir = opendir(DEFAULT_PATH.c_str());
	if (dir == NULL)
	{
		return;
	}


	Localization::String newDirName;
	Localization::String fullPath;
	Localization::String key;
	Localization::String data;

	Localization::String code;
	Localization::String label;
	

	/* print all the files and directories within directory */
	while ((ent = readdir(dir)) != nullptr)
	{
		if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0))
		{
			continue;
		}

		switch (ent->d_type)
		{
		case DT_REG:

			//file
			fullPath = dir->patt;
			fullPath = fullPath.substr(0, fullPath.length() - 1);
			fullPath += ent->d_name;

			data = LoadFile(fullPath);
			LoadLangInfo(data, code, label);
			if (code != "")
			{
				if (label == "")
				{
					label = code;
				}
				langList[code] = label;
			}
			break;

		case DT_DIR:

			break;

		default:
			//printf ("%s:\n", ent->d_name);
			break;
		}
	}


	Localization::String generatedCodeCpp = "#ifndef GENERATED_LANG_LIST_H\n";
	generatedCodeCpp += "#define GENERATED_LANG_LIST_H\n\n";
	generatedCodeCpp += "#include \"./Localization.h\"\n\n";
	generatedCodeCpp += "static const std::map<Localization::StringView, Localization::StringView> SUPPORTED_LANG_LIST = { \n";

	for (auto & langInfo : langList)
	{
			generatedCodeCpp += "{\"";
			generatedCodeCpp += langInfo.first;
			generatedCodeCpp += "\", \"";
			generatedCodeCpp += langInfo.second;
			generatedCodeCpp += "\"},\n";	
	}

	generatedCodeCpp[generatedCodeCpp.length() - 2] = ' ';

	generatedCodeCpp += "};\n\n";

	generatedCodeCpp += "#endif";

	FILE *f = fopen("generated_localization.h", "wb");
	if (f != nullptr)
	{
		fwrite(generatedCodeCpp.c_str(), sizeof(char), generatedCodeCpp.length(), f);
		fclose(f);
	}


	closedir(dir);
#endif
}

void Localization::LoadLangInfo(const Localization::String & data, Localization::String & code, Localization::String & label)
{
	code = "";
	label = "";
	if (cJSON *root = cJSON_Parse(data.c_str()))
	{
		int keysCount = cJSON_GetArraySize(root);
		for (int i = 0; i < keysCount; i++)
		{
			cJSON *item = cJSON_GetArrayItem(root, i);

			if (strcmp(item->string, "code") == 0)
			{
				code = item->valuestring;
				if (label != "")
				{
					break;
				}
			}
			if (strcmp(item->string, "label") == 0)
			{
				label = item->valuestring;
				if (code != "")
				{
					break;
				}
			}
						
		}

		cJSON_Delete(root);
	}
}

const Localization::String & Localization::GetLang() const
{
    return this->lang;
}

void Localization::SetLang(Localization::StringView lang)
{
	if (this->lang == lang)
	{
		return;
	}

    this->strs.clear();
    this->groups.clear();
    
    this->lang = lang;
    
    this->LoadLocalization(DEFAULT_LANGUAGE, this->strs, this->groups);
    
    if (this->lang != DEFAULT_LANGUAGE)
    {
        this->LoadLocalization(this->lang, this->strs, this->groups);
    }

	for (auto tmp : this->observers)
	{
		tmp->OnLanguageChange(lang, this);
	}
}

void Localization::ReplaceKeysByLang(Localization::StringView lang, 
	std::initializer_list< Localization::String> keys)
{
	if (this->lang == lang)
	{
		return;
	}

	std::unordered_map<String, LocalString> strsOther;
	std::unordered_map<String, std::unordered_map<String, LocalString>> groupsOther;

	this->LoadLocalization(lang, strsOther, groupsOther);

	for (const auto& key : keys)
	{
		if (auto it = strsOther.find(key); it != strsOther.end())
		{
			this->strs.insert_or_assign(key, it->second);
		}
	}

}

void Localization::LoadLocalization(const Localization::String & langID,
	std::unordered_map<String, LocalString>& strs,
	std::unordered_map<String, std::unordered_map<String, LocalString>>& groups) const
{    
	Localization::String path = DEFAULT_PATH;
	path += "gen_stringtable-";
	path += langID;
	path += ".js";

	Localization::String str = this->LoadFile(path);

	if (cJSON *root = cJSON_Parse(str.c_str()))
	{
		int keysCount = cJSON_GetArraySize(root);
		for (int i = 0; i < keysCount; i++)
		{
			cJSON *item = cJSON_GetArrayItem(root, i);
			
			if (item->valuestring == nullptr)
			{
				std::unordered_map<Localization::String, LocalString> tmpInner;
				
				auto it = groups.find(item->string);
                if (it != groups.end())
                {
                    tmpInner = it->second;
                }

				int innerKeysCount = cJSON_GetArraySize(item);
				for (int j = 0; j < innerKeysCount; j++)
				{
					cJSON *inner = cJSON_GetArrayItem(item, j);

					Localization::LocalString str = this->ProcessSingleInput(inner->valuestring);

					if (inner->string == nullptr)
					{
						//tmpInner.emplace(std::to_string(j), std::move(str));
						//tmpInner[std::to_string(j)] = str;
						tmpInner.insert_or_assign(std::to_string(j), std::move(str));
					}
					else
					{
						//tmpInner.emplace(inner->string, std::move(str));
						//tmpInner[inner->string] = str;
						tmpInner.insert_or_assign(inner->string, std::move(str));
					}
				}

				groups[item->string] = tmpInner;
			}
			else
			{
				//strs.emplace(item->string, this->ProcessSingleInput(item->valuestring));
				//strs[item->string] = this->ProcessSingleInput(item->valuestring);
				strs.insert_or_assign(item->string, this->ProcessSingleInput(item->valuestring));
			}

			
			//printf("%s = %s\n", item->string, item->valuestring);
		}

		cJSON_Delete(root);
	}	
}

Localization::LocalString Localization::ProcessSingleInput(const char * rawData) const
{
	const String EMPTY = String();

	LocalString str;
	str.str = rawData;

	size_t startIndexRaw = 0;
	size_t endIndexRaw = 0;

	size_t startIndex = str.str.find('{');
	while (startIndex != Localization::UnicodeStringWrapper::npos)
	{

		//startIndexRaw = str.str.raw_find('{', startIndexRaw);
		//endIndexRaw = str.str.raw_find('}', endIndexRaw);
		startIndexRaw = str.str.find('{', startIndexRaw);
		endIndexRaw = str.str.find('}', endIndexRaw);

		str.replaceOffsetsRawStart.push_back(startIndexRaw);
		str.replaceOffsetsRawEnd.push_back(endIndexRaw);


		size_t endIndex = str.str.find('}', startIndex);
		//str.replaceNames.push_back(str.str.substr(startIndex + 1, endIndex - startIndex - 1));

		Localization::UnicodeStringWrapper name = str.str.substr(startIndex + 1, endIndex - startIndex - 1);
		
		size_t groupIndex = name.find(':');
		if (groupIndex != Localization::UnicodeStringWrapper::npos)
		{
			str.replaceNameGroups.push_back(name.substr(0, groupIndex).getAs<Localization::String>());
			str.replaceNames.push_back(name.substr(groupIndex + 1).getAs<Localization::String>());
		}
		else
		{
			str.replaceNameGroups.push_back(EMPTY);
			str.replaceNames.push_back(name.getAs<Localization::String>());
		}
			
		startIndex = str.str.find('{', startIndex + 1);
		
		startIndexRaw++; //offset by 1 - next search will start one character after "{"
		endIndexRaw++; //offset by 1 - next search will start one character after "}"
	}

	return str;
}

Localization::String Localization::LoadFile(const Localization::String & path) const
{
#ifdef USE_VFS
	Localization::String str =  VFS::GetInstance()->GetFileString(path.c_str());
#else
	FILE * f = nullptr;
	my_fopen(&f, path.c_str(), "rb");

	if (f == nullptr)
	{		
		return "";
	}
	fseek(f, 0L, SEEK_END);
	long tmpSize = ftell(f);
	fseek(f, 0L, SEEK_SET);
	
	char * origData = new char[tmpSize + 1];
	fread(origData, sizeof(char), tmpSize, f);
	origData[tmpSize] = 0;
	fclose(f);
	
	Localization::String str = origData;
	delete[] origData;

#endif

	size_t foundPos = str.find("{");
	if (foundPos == Localization::String::npos)
	{
		return str;
	}

	return str.substr(foundPos);		
}

Localization::UnicodeStringWrapper Localization::Localize(const Localization::String& key, bool* exist)
{
	return this->Localize(key, "", exist);
}

Localization::UnicodeStringWrapper Localization::Localize(const Localization::String& key,
	const Localization::String& group, bool* exist)
{
	bool found;
	const LocalString& str = GetLocalStringInfo(key, group, found);
	if (found == false)
	{
		if (exist != nullptr) *exist = false;
		return key.c_str();
	}

	if (exist != nullptr) *exist = true;

	if (str.replaceNames.size() != 0)
	{
		std::vector<Localization::UnicodeStringWrapper> t;
		for (size_t i = 0; i < str.replaceNames.size(); i++)
		{
			auto val = this->Localize(str.replaceNames[i], str.replaceNameGroups[i]);
			t.push_back(std::move(val));
		}

		return this->LocalizeWithReplace(str, t);
	}
	else
	{

		return str.str;
	}
}

/// <summary>
/// localize string with replacement of "unknown" variables
/// eg:
/// key = "Hello {name} from {country} {xy}
/// country = "prison"
///
/// Call with 
/// Localize("key", {"Peter", "123"}) => Hello Peter from prison 123
/// -> replacement is used only for "variables", that are not found in other keys of translation file
/// in this case, country is key in translation file
/// </summary>
/// <param name="key"></param>
/// <param name="params"></param>
/// <param name="exist"></param>
Localization::UnicodeStringWrapper Localization::Localize(const Localization::String& key,
	const std::vector<Localization::UnicodeStringWrapper>& params, bool* exist)
{
	return this->Localize(key, "", params, exist);
}

/// <summary>
/// localize string with replacement of "unknown" variables
/// eg:
/// key = "Hello {name} from {country} {xy}
/// country = "prison"
///
/// Call with 
/// Localize("key", "", {"Peter", "123"}) => Hello Peter from prison 123
/// -> replacement is used only for "variables", that are not found in other keys of translation file
/// in this case, country is key in translation file
/// </summary>
/// <param name="key"></param>
/// <param name="group"></param>
/// <param name="params"></param>
/// <param name="exist"></param>
Localization::UnicodeStringWrapper Localization::Localize(const Localization::String& key,
	const Localization::String& group,
	const std::vector<Localization::UnicodeStringWrapper>& params, bool* exist)
{
	bool found;
	const LocalString& str = GetLocalStringInfo(key, group, found);
	if (found == false)
	{
		if (exist != nullptr) *exist = false;
		return key.c_str();
	}

	std::vector<Localization::UnicodeStringWrapper> t;

	int index = 0;
	for (const auto& elem : params)
	{
		bool found = false;
		Localization::UnicodeStringWrapper s = this->Localize(str.replaceNames[index],
			str.replaceNameGroups[index], &found);
		if (found)
		{
			t.push_back(std::move(s));
		}
		else
		{
			t.push_back(elem);
		}
		index++;
	}

	for (size_t i = index; i < str.replaceNames.size(); i++)
	{
		auto val = this->Localize(str.replaceNames[i], str.replaceNameGroups[i]);
		t.push_back(std::move(val));
	}


	if (exist != nullptr) *exist = true;
	return this->LocalizeWithReplace(str, t);
}
/// <summary>
/// localize string with replacement of "unknown" variables
/// eg:
/// key = "Hello {name} from {country} {xy}
///
/// Call with
/// Localize("key", "", {"name"=>"Peter", "country" => "prison", "xy" => "123"}) => Hello Peter from prison 123
/// -> replacement keys override keys from translation file
/// </summary>
/// <param name="key"></param>
/// <param name="group"></param>
/// <param name="params"></param>
/// <param name="exist"></param>
Localization::UnicodeStringWrapper Localization::Localize(const String& key, const String& group, const std::unordered_map<String, UnicodeStringWrapper>& params, bool* exist)
{
	bool found;
	const LocalString& str = GetLocalStringInfo(key, group, found);
	if (found == false)
	{
		if (exist != nullptr) *exist = false;
		return key.c_str();
	}

	std::vector<Localization::UnicodeStringWrapper> t;

	for (const auto& k : str.replaceNames)
	{
		for (const auto& [key, value] : params)
		{		
			if (k == key)
			{
				t.push_back(value);
				break;
			}
		}
	}


	if (exist != nullptr) *exist = true;
	return this->LocalizeWithReplace(str, t);
}


/// <summary>
/// Get localization info based on key and group
///
/// If no info is found, return first one - it is ok, since we dont use it
/// it is just to return something valid
/// </summary>
/// <param name="key"></param>
/// <param name="group"></param>
/// <param name="found"></param>
const Localization::LocalString& Localization::GetLocalStringInfo(const Localization::String& key, const Localization::String& group, bool& found)
{
	std::unordered_map<Localization::String, LocalString>::const_iterator it;

	if (group.length() == 0)
	{
		it = this->strs.find(key);

		if (it == this->strs.end())
		{
			found = false;
			return this->strs.begin()->second;
		}
	}
	else
	{
		auto jt = this->groups.find(group);
		if (jt == this->groups.end())
		{
			found = false;
			return this->strs.begin()->second;
		}
		it = jt->second.find(key);

		if (it == jt->second.end())
		{
			found = false;
			return this->strs.begin()->second;
		}
	}

	found = true;
	return it->second;
}


/// <summary>
/// Internal method - go over string and replace each {} with param at appropriate index
/// </summary>
/// <param name="input"></param>
/// <param name="params"></param>
Localization::UnicodeStringWrapper Localization::LocalizeWithReplace(const LocalString & input, 
	const std::vector<Localization::UnicodeStringWrapper> & params)
{
	size_t count = std::min(params.size(), input.replaceOffsetsRawStart.size());

	Localization::UnicodeStringWrapper tmp = input.str;

	size_t i = 0;
	size_t offset = 0;
	for (const auto & elem : params)
	{
		size_t start = input.replaceOffsetsRawStart[i] + offset;
		size_t end = (input.replaceOffsetsRawEnd[i] + 1) + offset;

		//size_t oldLengthRaw = tmp.size();
		//tmp.raw_replace(start, end - start, elem);
		//size_t newLengthRaw = tmp.size();

		size_t oldLengthRaw = tmp.length();
		tmp.replace(start, end - start, elem);
		size_t newLengthRaw = tmp.length();

		offset += (newLengthRaw - oldLengthRaw);

		i++;

		if (i > count)
		{
			break;
		}
	}

	return tmp;
}
