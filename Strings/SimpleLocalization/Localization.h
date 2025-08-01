#ifndef SIMPLE_LOCALIZATION_H
#define SIMPLE_LOCALIZATION_H

#include <string.h>
#include <unordered_map>
#include <map>
#include <vector>
#include <list>
#include <initializer_list>

#if __has_include("../Unicode/TinyUtf8Wrapper.h")
#	include "../Unicode/TinyUtf8Wrapper.h"
#endif

#if __has_include("../Unicode/IcuUnicodeStringWrapper.h")
#	include "../Unicode/IcuUnicodeStringWrapper.h"
#endif

#include "../MyString.h"


class Localization
{
public:

	using String = MyStringAnsi;	
	using StringView = MyStringView;
	using UnicodeStringWrapper = TinyUtf8Wrapper;

	class ILocalizationObserver
	{
	public:
		virtual ~ILocalizationObserver() {};
		virtual void OnLanguageChange(StringView lang, Localization* loc) = 0;
	};

	Localization(StringView lang, StringView defLang = "en", StringView defPath = "");
	~Localization();
	
	void AddObserver(ILocalizationObserver* observer);
	void RemoveObserver(ILocalizationObserver* observer);

	const std::map<StringView, StringView> & GetAllSupportedLanguages() const;

    void SetLang(StringView lang);
	void SetLang(StringView lang, bool forceReload);
    const String & GetLang() const;
    
	void ReplaceKeysByLang(Localization::StringView lang, std::initializer_list< Localization::String> keys);

	template <typename T>
	T LocalizeAs(const String& key, bool* exist = nullptr);

	UnicodeStringWrapper Localize(const String & key, bool * exist = nullptr);
	UnicodeStringWrapper Localize(const String & key, const String & group, bool * exist = nullptr);
	UnicodeStringWrapper Localize(const String & key, const std::vector<UnicodeStringWrapper> & params, bool * exist = nullptr);
	UnicodeStringWrapper Localize(const String & key, const String & group, const std::vector<UnicodeStringWrapper> & params, bool * exist = nullptr);
	UnicodeStringWrapper Localize(const String& key, const String& group, const std::unordered_map<String, UnicodeStringWrapper>& params, bool* exist = nullptr);

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

	std::map<StringView, StringView> supportedLanguages;
    String lang;
    
	std::unordered_map<String, LocalString> strs; //ordinary translation [key] = value
	std::unordered_map<String, std::unordered_map<String, LocalString>> groups; //special translation [group][key] = value
	
	std::list<ILocalizationObserver *> observers;

	void GenerateSupportedLanguagesList();

	void LoadLocalization(const String & langID,
		std::unordered_map<String, LocalString>& strs,
		std::unordered_map<String, std::unordered_map<String, LocalString>>& groups) const;
	LocalString ProcessSingleInput(const char * rawData) const;

	String LoadFile(const String & path) const;
	void LoadLangInfo(const String & data, String & code, String & label);
	
	const Localization::LocalString& GetLocalStringInfo(const Localization::String& key, const Localization::String& group, bool& found);

	UnicodeStringWrapper LocalizeWithReplace(const LocalString & input, const std::vector<UnicodeStringWrapper> & params);
};


template <typename T>
T Localization::LocalizeAs(const String& key, bool* exist)
{
	auto tmp = this->Localize(key, exist);
	return tmp.getAs<T>();
}


#endif
