#ifndef MY_STRING_VIEW_H
#define MY_STRING_VIEW_H

namespace mystrlib
{
	class MyString;
	class MySmallString;
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
	class MyStringView
	{
	public:

		static const size_t npos = static_cast<size_t>(-1);

		constexpr MyStringView() noexcept;
		constexpr MyStringView(StringLiteral l) noexcept;
		MyStringView(const char* str, size_t len = 0) noexcept;
		MyStringView(const MyString& str) noexcept;
		MyStringView(const MyString* str) noexcept;
		MyStringView(const MySmallString& str) noexcept;
		MyStringView(const MySmallString* str) noexcept;
		MyStringView(const MyStringView& v) noexcept;

		MyStringView(const std::string& str) noexcept;
		MyStringView(const std::string_view& str) noexcept;
		MyStringView(const std::vector<char>& v) noexcept;


		~MyStringView() = default;

		uint32_t GetHashCode() const noexcept;
		const char* c_str() const noexcept;
		size_t length() const noexcept;
		bool empty() const noexcept;

		char GetLastChar() const;
		char GetFirstChar() const;

		void Trim();

		void RemoveFromStart(size_t count = 1);
		void RemoveFromEnd(size_t count = 1);
		MyStringView SubString(int start) const;
		MyStringView SubString(int start, size_t length) const;
		MyStringView substr(int start, size_t length) const;

		bool StartWith(MyStringView needle) const noexcept;
		bool EndWith(MyStringView needle) const noexcept;
		size_t Find(const char c) const noexcept;
		size_t Find(MyStringView needle) const noexcept;
		size_t Find(MyStringView needle, size_t offset) const noexcept;
		std::vector<size_t> FindAll(MyStringView needle) const;

		MyStringView& operator = (const char* str) noexcept;

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
		bool operator() (MyStringView const& t1, MyStringView const& t2) const
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



	constexpr MyStringView::MyStringView() noexcept :
		str(nullptr),
		len(0),
		hash(std::numeric_limits<uint32_t>::max())
	{
	}

	constexpr MyStringView::MyStringView(StringLiteral l) noexcept :
		str(l),
		len(l.length()),
		hash(MurmurHash3_32CExpr(str, static_cast<uint32_t>(len), MURMUR_HASH_DEF_SEED))
	{
	}



	template <typename T>
	RET_VAL_STR(char, (std::is_integral<T>::value))
		MyStringView::operator [](const T index) const
	{
		return str[index];
	}

}


#endif
