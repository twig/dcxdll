#pragma once
/*
	Auto release resources on error...

	By Marko Tintor (http://blog.memsql.com/c-error-handling-with-auto/)

	Minimal changes made so it compiles ok with vs2013
*/
//#include <stdexcept>	// for exceptions

#define TOKEN_PASTEx(x, y) x ## y
#define TOKEN_PASTE(x, y) TOKEN_PASTEx(x, y)

template <typename T>
class AutoOutOfScope
{
public:
	AutoOutOfScope(T& destructor) noexcept
		: m_destructor(destructor)
	{ }
	AutoOutOfScope(AutoOutOfScope &) = delete;	// Can't be copied!
	AutoOutOfScope() = delete;					// must have args!
	~AutoOutOfScope() { m_destructor(); }

	AutoOutOfScope &operator =(const AutoOutOfScope &) = delete;	// No assignments!

private:
	T& m_destructor;
};

#define Auto_INTERNAL(Destructor, counter) \
	const auto TOKEN_PASTE(auto_func_, counter) = [&]() noexcept { Destructor; }; \
AutoOutOfScope<decltype(TOKEN_PASTE(auto_func_, counter))> TOKEN_PASTE(auto_, counter)(TOKEN_PASTE(auto_func_, counter));

#define Auto(Destructor) Auto_INTERNAL(Destructor, __COUNTER__)
