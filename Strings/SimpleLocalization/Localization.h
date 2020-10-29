#ifndef SIMPLE_LOCALIZATION_H
#define SIMPLE_LOCALIZATION_H

#include <string.h>
#include <unordered_map>
#include <map>
#include <vector>
#include <initializer_list>

#include "./TinyUtf8Wrapper.h"
#include "./IcuUnicodeStringWrapper.h"

#include "../MyString.h"

class Localization
{
public:

	using String = MyStringAnsi;	
	using UnicodeStringWrapper = TinyUtf8Wrapper;
	
	Localization(const String & lang, const String & defLang = "en", const String & defPath = "");
	~Localization();
	
	const std::map<String, String> & GetAllSupportedLanguages() const;

    void SetLang(const String & lang);
    const String & GetLang() const;
    
	UnicodeStringWrapper Localize(const String & key, bool * exist = nullptr);
	UnicodeStringWrapper Localize(const String & key, const String & group, bool * exist = nullptr);
	UnicodeStringWrapper Localize(const String & key, const std::vector<UnicodeStringWrapper> & params, bool * exist = nullptr);
	UnicodeStringWrapper Localize(const String & key, const String & group, const std::vector<UnicodeStringWrapper> & params, bool * exist = nullptr);

protected:

	/// <summary>
	/// internal helper string with allocated
	/// offsets to translation variables {xxx}
	/// !!! Translation variables can be only in ASCII !!!
	/// </summary>
	typedef struct LocalString
	{
		UnicodeStringWrapper str;
		std::vector<size_t> replaceOffsetsRawStart;
		std::vector<size_t> replaceOffsetsRawEnd;
		std::vector<String> replaceNames;
		std::vector<String> replaceNameGroups;

	} LocalString;

	const String DEFAULT_LANGUAGE;
	const String DEFAULT_PATH;

	std::map<String, String> supportedLanguages;
    String lang;
    
	std::unordered_map<String, LocalString> strs; //ordinary translation [key] = value
	std::unordered_map<String, std::unordered_map<String, LocalString>> groups; //special translation [group][key] = value

	void LoadAllSupportedLanguages();

	void LoadLocalization(const String & langID);
	LocalString ProcessSingleInput(const char * rawData);

	String LoadFile(const String & path);
	void LoadLangInfo(const String & data, String & code, String & label);

	UnicodeStringWrapper LocalizeWithReplace(const LocalString & input, const std::vector<UnicodeStringWrapper> & params);
};

#endif
