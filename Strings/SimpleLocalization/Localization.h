#ifndef SIMPLE_LOCALIZATION_H
#define SIMPLE_LOCALIZATION_H

#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <list>
#include <initializer_list>

#include "../MyStringLib.h"
#include "../Unicode/UnicodeUtils.h"

namespace mystrlib
{

	class Localization
	{
	public:

		using String = String;
		using StringView = StringView;
		using Utf8StringWrapper = StringUtf8;

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

		const std::map<StringView, StringView>& GetAllSupportedLanguages() const;

		void SetLang(StringView lang);
		void SetLang(StringView lang, bool forceReload);
		const String& GetLang() const;

		void ReplaceKeysByLang(Localization::StringView lang, std::initializer_list<Localization::String> keys);

		template <typename T>
		T LocalizeAs(const String& key, bool* exist = nullptr);

		Utf8StringWrapper Localize(const String& key, bool* exist = nullptr);
		Utf8StringWrapper Localize(const String& key, const String& group, bool* exist = nullptr);
		Utf8StringWrapper Localize(const String& key, const std::vector<Utf8StringWrapper>& params, bool* exist = nullptr);
		Utf8StringWrapper Localize(const String& key, const String& group, const std::vector<Utf8StringWrapper>& params, bool* exist = nullptr);
		Utf8StringWrapper Localize(const String& key, const String& group, const std::unordered_map<String, Utf8StringWrapper>& params, bool* exist = nullptr);

	protected:

		/// <summary>
		/// internal helper string with allocated
		/// offsets to translation variables {xxx}
		/// !!! Translation variables can be only in ASCII !!!
		/// </summary>
		struct LocalString
		{
			Utf8StringWrapper str;
			std::vector<size_t> replaceOffsetsRawStart;
			std::vector<size_t> replaceOffsetsRawEnd;
			std::vector<String> replaceNames;
			std::vector<String> replaceNameGroups;

		};

		const String DEFAULT_LANGUAGE;
		const String DEFAULT_PATH;

		std::map<StringView, StringView> supportedLanguages;
		String lang;

		std::unordered_map<String, LocalString> strs; //ordinary translation [key] = value
		std::unordered_map<String, std::unordered_map<String, LocalString>> groups; //special translation [group][key] = value

		std::list<ILocalizationObserver*> observers;

		void GenerateSupportedLanguagesList();

		void LoadLocalization(const String& langID,
			std::unordered_map<String, LocalString>& strs,
			std::unordered_map<String, std::unordered_map<String, LocalString>>& groups) const;
		LocalString ProcessSingleInput(const char* rawData) const;

		String LoadFile(const String& path) const;
		void LoadLangInfo(const String& data, String& code, String& label);

		const Localization::LocalString& GetLocalStringInfo(const Localization::String& key, const Localization::String& group, bool& found);

		Utf8StringWrapper LocalizeWithReplace(const LocalString& input, const std::vector<Utf8StringWrapper>& params);
	};


	template <typename T>
	T Localization::LocalizeAs(const String& key, bool* exist)
	{
		auto tmp = this->Localize(key, exist);
		return T(tmp);
	}

}

#endif
