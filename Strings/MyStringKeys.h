#ifndef MY_STRING_KEYS_H
#define MY_STRING_KEYS_H

#include "./MyString.h"
#include "./MyStringView.h"

namespace mystrlib
{

	/// <summary>
	/// This is special key that can be used in maps and sets
	/// to allow finds with view instead of string
	/// 
	/// "Heterogenous key"
	/// </summary>
	struct StringViewComparableKey
	{
		//if str is set, view is created from str		
		//otherwise only view is used

		MyStringView view;
		MyString str;

		StringViewComparableKey() = default;

		explicit StringViewComparableKey(const MyStringView& v) : view(v), str()
		{
		}

		StringViewComparableKey(const char* s) : str(s)
		{
			view = MyStringView(str);
		}

		StringViewComparableKey(const MyString& s) : str(s)
		{
			view = MyStringView(str);
		}

		bool operator() (StringViewComparableKey const& t1, StringViewComparableKey const& t2) const
		{
			return t1.view == t2.view;
		};

		size_t operator()(const StringViewComparableKey& x) const
		{
			//we must call str hash if str is present
			//because view will compute hash only if it is not created from MyString
			return (x.str.length() != 0) ? x.str.GetHashCode() : x.view.GetHashCode();
		}
	};
}

#endif
