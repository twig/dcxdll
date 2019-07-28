/*
www.sourceforge.net/projects/tinyxml
Original file by Yves Berquin.

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

/*
 * THIS FILE WAS ALTERED BY Tyge Lovset, 7. April 2005.
 *
 * - completely rewritten. compact, clean, and fast implementation.
 * - sizeof(TiXmlString) = pointer size (4 bytes on 32-bit systems)
 * - fixed reserve() to work as per specification.
 * - fixed buggy compares operator==(), operator<(), and operator>()
 * - fixed operator+=() to take a const ref argument, following spec.
 * - added "copy" constructor with length, and most compare operators.
 * - added swap(), clear(), size(), capacity(), operator+().
 */

#ifndef TIXML_USE_STL

#ifndef TIXML_STRING_INCLUDED
#define TIXML_STRING_INCLUDED

#include <assert.h>
#include <string.h>

 /*	The support for explicit isn't that universal, and it isn't really
	 required - it is used to check that the TiXmlString class isn't incorrectly
	 used. Be nice to old compilers and macro it here:
 */
#if defined(_MSC_VER) && (_MSC_VER >= 1200 )
 // Microsoft visual studio, version 6 and higher.
#define TIXML_EXPLICIT explicit
#elif defined(__GNUC__) && (__GNUC__ >= 3 )
 // GCC version 3 and higher.s
#define TIXML_EXPLICIT explicit
#else
#define TIXML_EXPLICIT
#endif


 /*
	TiXmlString is an emulation of a subset of the std::string template.
	Its purpose is to allow compiling TinyXML on compilers with no or poor STL support.
	Only the member functions relevant to the TinyXML project have been implemented.
	The buffer allocation is made by a simplistic power of 2 like mechanism : if we increase
	a string and there's no more room, we allocate a buffer twice as big as we need.
 */
class TiXmlString
{
public:
	// The size type used
	using size_type = std::size_t;

	// Error value for find primitive
	//static const size_type npos; // = -1;
	static constexpr size_type npos{ std::numeric_limits<size_type>::max() };

	// TiXmlString empty constructor
	constexpr TiXmlString() noexcept = default;

	TiXmlString(const TiXmlString& copy)
		: TiXmlString(copy.data(), copy.length())
	{
	}

	TiXmlString(TiXmlString&& copy) noexcept
	{
		swap(copy);
	}

	// TiXmlString constructor, based on a string
	TIXML_EXPLICIT TiXmlString(const char* copy)
		: TiXmlString(copy, gsl::narrow_cast<size_type>(_ts_strlen(copy)))
	{
	}

	// TiXmlString constructor, based on a string
	TIXML_EXPLICIT TiXmlString(const char* str, size_type len)
	{
		init(len);
		memcpy(start(), str, len);
	}

	// TiXmlString destructor
	~TiXmlString() noexcept
	{
		quit();
	}

	// = operator
	TiXmlString& operator = (const char* copy)
	{
		return assign(copy, gsl::narrow_cast<size_type>(_ts_strlen(copy)));
	}

	// = operator
	TiXmlString& operator = (const TiXmlString& copy)
	{
		return assign(copy.start(), copy.length());
	}

	// = operator
	TiXmlString& operator = (TiXmlString&& copy) noexcept
	{
		if (this == &copy)	// self assignment check.
			return *this;

		swap(copy);

		return *this;
	}

	// += operator. Maps to append
	TiXmlString& operator += (const char* suffix)
	{
		return append(suffix, gsl::narrow_cast<size_type>(_ts_strlen(suffix)));
	}

	// += operator. Maps to append
	TiXmlString& operator += (char single)
	{
		return append(&single, 1);
	}

	// += operator. Maps to append
	TiXmlString& operator += (const TiXmlString& suffix)
	{
		return append(suffix.data(), suffix.length());
	}


	// Convert a TiXmlString into a null-terminated char *
	const char* c_str() const noexcept { return &rep_->str[0]; }

	// Convert a TiXmlString into a char * (need not be null terminated).
	const char* data() const noexcept { return &rep_->str[0]; }

	// Return the length of a TiXmlString
	const size_type& length() const noexcept { return rep_->size; }

	// Alias for length()
	const size_type& size() const noexcept { return rep_->size; }

	// Checks if a TiXmlString is empty
	bool empty() const noexcept { return rep_->size == 0; }

	// Return capacity of string
	const size_type& capacity() const noexcept { return rep_->capacity; }

	// single char extraction
	const char& at(size_type index) const noexcept
	{
		assert(index < length());
		return rep_->str[index];
	}

	// [] operator
	char& operator [] (size_type index) const noexcept
	{
		assert(index < length());
		return rep_->str[index];
	}

	// find a char in a string. Return TiXmlString::npos if not found
	size_type find(char lookup) const noexcept
	{
		return find(lookup, 0);
	}

	// find a char in a string from an offset. Return TiXmlString::npos if not found
	size_type find(char tofind, size_type offset) const noexcept
	{
		if (offset >= length())
			return npos;

		for (const char* p = c_str() + offset; *p != '\0'; ++p)
		{
			if (*p == tofind)
				return gsl::narrow_cast<size_type>(p - c_str());
		}
		return npos;
	}

	void clear()
	{
		//Lee:
		//The original was just too strange, though correct:
		//	TiXmlString().swap(*this);
		//Instead use the quit & re-init:
		quit();
		init(0, 0);
	}

	/*	Function to reserve a big amount of data when we know we'll need it. Be aware that this
		function DOES NOT clear the content of the TiXmlString if any exists.
	*/
	void reserve(size_type cap);

	TiXmlString& assign(const char* str, size_type len);

	TiXmlString& append(const char* str, size_type len);

	void swap(TiXmlString& other) noexcept
	{
		//Rep *const r = rep_;
		//rep_ = other.rep_;
		//other.rep_ = r;

		auto r = rep_;
		auto otherr = other.rep_;

		if (rep_ == &nullrep_)
			r = &other.nullrep_;

		if (other.rep_ == &other.nullrep_)
			otherr = &nullrep_;

		rep_ = otherr;
		other.rep_ = r;
	}

	struct iter
	{
		char operator * () const noexcept { return *n; }
		iter& operator ++() noexcept { ++n; return *this; }
		friend
			bool operator != (iter const& lhs, iter const& rhs) noexcept
		{
			return lhs.n != rhs.n;
		}
	
		char* n{ nullptr };
	};
	
	iter begin() const noexcept { return{ start() }; }
	iter end() const noexcept { return{ finish() }; }

private:

	void init(size_type sz)
	{
		init(sz, sz);
	}
	void set_size(size_type sz) noexcept
	{
		rep_->size = sz;
		rep_->str[sz] = '\0';
	}
	char* start() const noexcept { return &rep_->str[0]; }
	char* finish() const noexcept { return &rep_->str[0] + rep_->size; }

	struct Rep
	{
		size_type size{}, capacity{};
		char str[1]{};
	};

	void init(size_type sz, size_type cap)
	{
		if (cap)
		{
			// Lee: the original form:
			//	rep_ = static_cast<Rep*>(operator new(sizeof(Rep) + cap));
			// doesn't work in some cases of new being overloaded. Switching
			// to the normal allocation, although use an 'int' for systems
			// that are overly picky about structure alignment.
			const size_type bytesNeeded = sizeof(Rep) + cap;
			const size_type intsNeeded = (bytesNeeded + sizeof(int) - 1) / sizeof(int);
			rep_ = reinterpret_cast<Rep*>(new int[intsNeeded]);

			rep_->size = sz;
			rep_->str[sz] = '\0';
			rep_->capacity = cap;
		}
		else
		{
			rep_ = &nullrep_;
		}
	}

	void quit() noexcept
	{
		if (rep_ != &nullrep_)
		{
			// The rep_ is really an array of ints. (see the allocator, above).
			// Cast it back before delete, so the compiler won't incorrectly call destructors.
			delete[](reinterpret_cast<int*>(rep_));
		}
	}

	Rep* rep_{ &nullrep_ };
	static Rep nullrep_;
};


inline bool operator == (const TiXmlString& a, const TiXmlString& b) noexcept
{
	return    (a.length() == b.length())				// optimization on some platforms
		&& (_ts_strcmp(a.c_str(), b.c_str()) == 0);	// actual compare
}
inline bool operator < (const TiXmlString& a, const TiXmlString& b) noexcept
{
	return _ts_strcmp(a.c_str(), b.c_str()) < 0;
}

inline bool operator != (const TiXmlString& a, const TiXmlString& b) noexcept { return !(a == b); }
inline bool operator >  (const TiXmlString& a, const TiXmlString& b) noexcept { return b < a; }
inline bool operator <= (const TiXmlString& a, const TiXmlString& b) noexcept { return !(b < a); }
inline bool operator >= (const TiXmlString& a, const TiXmlString& b) noexcept { return !(a < b); }

inline bool operator == (const TiXmlString& a, const char* b) noexcept { return _ts_strcmp(a.c_str(), b) == 0; }
inline bool operator == (const char* a, const TiXmlString& b) noexcept { return b == a; }
inline bool operator != (const TiXmlString& a, const char* b) noexcept { return !(a == b); }
inline bool operator != (const char* a, const TiXmlString& b) noexcept { return !(b == a); }

TiXmlString operator + (const TiXmlString& a, const TiXmlString& b);
TiXmlString operator + (const TiXmlString& a, const char* b);
TiXmlString operator + (const char* a, const TiXmlString& b);


/*
   TiXmlOutStream is an emulation of std::ostream. It is based on TiXmlString.
   Only the operators that we need for TinyXML have been developed.
*/
class TiXmlOutStream
	: public TiXmlString
{
public:

	// TiXmlOutStream << operator.
	TiXmlOutStream& operator << (const TiXmlString& in)
	{
		*this += in;
		return *this;
	}

	// TiXmlOutStream << operator.
	TiXmlOutStream& operator << (const char* in)
	{
		*this += in;
		return *this;
	}

};

#endif	// TIXML_STRING_INCLUDED
#endif	// TIXML_USE_STL
