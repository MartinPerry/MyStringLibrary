#ifndef MY_STRING_VIEW_H
#define MY_STRING_VIEW_H

namespace mystrlib
{
	class String;
	class SmallString;
}

#include <type_traits>
#include <unordered_map>
#include <cstdint>
#include <limits>
#include <string>
#include <cstring>
#include <vector>

#include "./hashing/MurmurHash3_constexpr.inl"
#include "./MyStringLiteral.h"

//========================================================================

namespace mystrlib
{
	
	//========================================================================

	/// <summary>
	/// Simple string view class
	/// </summary>
	class StringView
	{
	public:

		static const size_t npos = static_cast<size_t>(-1);

		constexpr StringView() noexcept;
		constexpr StringView(StringLiteral l) noexcept;
		StringView(const char* str, size_t len = 0) noexcept;
		StringView(const String& str) noexcept;
		StringView(const String* str) noexcept;
		StringView(const SmallString& str) noexcept;
		StringView(const SmallString* str) noexcept;
		StringView(const StringView& v) noexcept;

		StringView(const std::string& str) noexcept;
		StringView(const std::string_view& str) noexcept;
		StringView(const std::vector<char>& v) noexcept;


		~StringView() = default;

		uint32_t GetHashCode() const noexcept;
		const char* c_str() const noexcept;
		size_t length() const noexcept;
		bool empty() const noexcept;

		char GetLastChar() const;
		char GetFirstChar() const;

		void Trim();

		void RemoveFromStart(size_t count = 1);
		void RemoveFromEnd(size_t count = 1);
		StringView SubString(int start) const;
		StringView SubString(int start, size_t length) const;
		StringView substr(int start, size_t length) const;

		bool StartWith(StringView needle) const noexcept;
		bool EndWith(StringView needle) const noexcept;
		size_t Find(const char c) const noexcept;
		size_t Find(StringView needle) const noexcept;
		size_t Find(StringView needle, size_t offset) const noexcept;
		std::vector<size_t> FindAll(StringView needle) const;

		StringView& operator = (const char* str) noexcept;

		template <typename T>
		RET_VAL_STR(char, (std::is_integral<T>::value))
			operator [](const T index) const;

		/// <summary>
		/// Used for comparison os views that are substring 
		/// non-null terminated
		/// For example:
		/// std::unordered_map<MyStringView, ...,
		/// std::hash<MyStringView>, MyStringView>
		/// </summary>
		/// <param name="t1"></param>
		/// <param name="t2"></param>
		/// <returns></returns>
		bool operator() (StringView const& t1, StringView const& t2) const
		{
			if (t1.length() != t2.length()) return false;
			return (memcmp(t1.c_str(), t2.c_str(), t1.length()) == 0);
		};


	private:
		struct stringHash
		{
			bool isPtr;
			union
			{
				mutable uint32_t value;
				uint32_t* valuePtr;
			};

			constexpr stringHash(uint32_t val) :
				isPtr(false),
				value(val)
			{
			}

			constexpr stringHash(uint32_t* val) :
				isPtr(true),
				valuePtr(val)
			{
			}
		};

		const char* str;
		size_t len;
		stringHash hash;
	};



	constexpr StringView::StringView() noexcept :
		str(nullptr),
		len(0),
		hash(std::numeric_limits<uint32_t>::max())
	{
	}

	constexpr StringView::StringView(StringLiteral l) noexcept :
		str(l),
		len(l.length()),
		hash(MurmurHash3_32CExpr(str, static_cast<uint32_t>(len), MURMUR_HASH_DEF_SEED))
	{
	}



	template <typename T>
	RET_VAL_STR(char, (std::is_integral<T>::value))
		StringView::operator [](const T index) const
	{
		return str[index];
	}

}


#endif
