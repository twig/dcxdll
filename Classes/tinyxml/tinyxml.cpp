/*
www.sourceforge.net/projects/tinyxml
Original code (2.0 and earlier )copyright (c) 2000-2006 Lee Thomason (www.grinninglizard.com)

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

#include "defines.h"
#include <ctype.h>

#ifdef TIXML_USE_STL
#include <sstream>
#include <iostream>
#endif

#include "tinyxml.h"

FILE* TiXmlFOpen(const char* filename, const char* mode) noexcept;

// Microsoft compiler security
FILE* TiXmlFOpen(const char* filename, const char* mode) noexcept
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
	FILE* fp{ nullptr };
	const errno_t err = fopen_s(&fp, filename, mode);
	if (!err && fp)
		return fp;
	return nullptr;
#else
	return fopen(filename, mode);
#endif
}

void TiXmlBase::EncodeString(const TIXML_STRING& str, TIXML_STRING* outString)
{
	if (!outString)
		return;

	size_t i = 0;

	while (i < str.length())
	{
		const unsigned char c = gsl::narrow_cast<unsigned char>(gsl::at(str, i));
		//const unsigned char c = gsl::narrow_cast<unsigned char>(str[i]);

		if (c == '&'
			&& i < (str.length() - 2)
			&& gsl::at(str, i + 1) == '#'
			&& gsl::at(str, i + 2) == 'x')
		{
			// Hexadecimal character reference.
			// Pass through unchanged.
			// &#xA9;	-- copyright symbol, for example.
			//
			// The -1 is a bug fix from Rob Laveaux. It keeps
			// an overflow from happening if there is no ';'.
			// There are actually 2 ways to exit this loop -
			// while fails (error case) and break (semicolon found).
			// However, there is no mechanism (currently) for
			// this function to return an error.
			while (i < (str.length() - 1))
			{
				outString->append(str.c_str() + i, 1);
				++i;
				if (gsl::at(str, i) == ';')
					break;
			}
		}
		else if (c == '&')
		{
			outString->append(entity[0].str, entity[0].strLength);
			++i;
		}
		else if (c == '<')
		{
			outString->append(entity[1].str, entity[1].strLength);
			++i;
		}
		else if (c == '>')
		{
			outString->append(entity[2].str, entity[2].strLength);
			++i;
		}
		else if (c == '\"')
		{
			outString->append(entity[3].str, entity[3].strLength);
			++i;
		}
		else if (c == '\'')
		{
			outString->append(entity[4].str, entity[4].strLength);
			++i;
		}
		else if (c < 32)
		{
			// Easy pass at non-alpha/numeric/symbol
			// Below 32 is symbolic.
			char buf[32]{};

			{
#if defined(TIXML_SNPRINTF)
				const auto tmp = gsl::narrow_cast<unsigned int>(c & 0xff);
				TIXML_SNPRINTF(buf, sizeof(buf), "&#x%02X;", tmp);
				buf[31] = 0;	// Ook: make sure string is zero terminated. snprintf() can leave it non zero terminated...
#else
				const auto tmp = gsl::narrow_cast<unsigned int>(c & 0xff);
				sprintf(&buf[0], "&#x%02X;", tmp);
#endif		
			}
			//*ME:	warning C4267: convert 'size_t' to 'int'
			//*ME:	Int-Cast to make compiler happy ...
			outString->append(&buf[0], strlen(&buf[0]));
			++i;
		}
		else
		{
			//char realc = (char) c;
			//outString->append( &realc, 1 );
			*outString += gsl::narrow_cast<char>(c);	// somewhat more efficient function call.
			//*outString += static_cast<char>(c);	// somewhat more efficient function call.
			++i;
		}
	}
}


TiXmlNode::TiXmlNode(NodeType _type) noexcept
	: TiXmlBase()
{
	parent = nullptr;
	type = _type;
	firstChild = nullptr;
	lastChild = nullptr;
	prev = nullptr;
	next = nullptr;
}


TiXmlNode::~TiXmlNode() noexcept
{
	this->Clear();
}


void TiXmlNode::CopyTo(TiXmlNode* target) const
{
	target->SetValue(value.c_str());
	target->userData = userData;
	target->location = location;
}


void TiXmlNode::Clear() noexcept
{
	for (TiXmlNode* node = firstChild, *temp{ nullptr }; node; node = temp)
	{
		temp = node->next;
		delete node;
	}
	firstChild = nullptr;
	lastChild = nullptr;
}


TiXmlNode* TiXmlNode::LinkEndChild(TiXmlNode* const node)
{
	assert(node != nullptr);
	assert(node->parent == nullptr || node->parent == this);
	assert(node->GetDocument() == nullptr || node->GetDocument() == this->GetDocument());

	if (node->Type() == TiXmlNode::NodeType::TINYXML_DOCUMENT)
	{
		delete node;
		if (const auto document = GetDocument(); document)
			document->SetError(tixmlErrors::TIXML_ERROR_DOCUMENT_TOP_ONLY, nullptr, nullptr, TiXmlEncoding::TIXML_ENCODING_UNKNOWN);
		return nullptr;
	}

	node->parent = this;

	node->prev = lastChild;
	node->next = nullptr;

	if (lastChild)
		lastChild->next = node;
	else
		firstChild = node;			// it was an empty list.

	lastChild = node;
	return node;
}


TiXmlNode* TiXmlNode::InsertEndChild(const TiXmlNode& addThis)
{
	if (addThis.Type() == TiXmlNode::NodeType::TINYXML_DOCUMENT)
	{
		if (GetDocument()) GetDocument()->SetError(tixmlErrors::TIXML_ERROR_DOCUMENT_TOP_ONLY, nullptr, nullptr, TiXmlEncoding::TIXML_ENCODING_UNKNOWN);
		return nullptr;
	}
	const auto node = addThis.Clone();
	if (!node)
		return nullptr;

	return LinkEndChild(node);
}


TiXmlNode* TiXmlNode::InsertBeforeChild(TiXmlNode* beforeThis, const TiXmlNode& addThis)
{
	if (!beforeThis || beforeThis->parent != this)
		return nullptr;

	if (addThis.Type() == TiXmlNode::NodeType::TINYXML_DOCUMENT)
	{
		if (const auto document = GetDocument(); document)
			document->SetError(tixmlErrors::TIXML_ERROR_DOCUMENT_TOP_ONLY, nullptr, nullptr, TiXmlEncoding::TIXML_ENCODING_UNKNOWN);
		return nullptr;
	}

	const auto node = addThis.Clone();
	if (!node)
		return nullptr;
	node->parent = this;

	node->next = beforeThis;
	node->prev = beforeThis->prev;
	if (beforeThis->prev)
	{
		beforeThis->prev->next = node;
	}
	else
	{
		assert(firstChild == beforeThis);
		firstChild = node;
	}
	beforeThis->prev = node;
	return node;
}


TiXmlNode* TiXmlNode::InsertAfterChild(TiXmlNode* afterThis, const TiXmlNode& addThis)
{
	if (!afterThis || afterThis->parent != this)
		return nullptr;

	if (addThis.Type() == TiXmlNode::NodeType::TINYXML_DOCUMENT)
	{
		if (const auto document = GetDocument(); document)
			document->SetError(tixmlErrors::TIXML_ERROR_DOCUMENT_TOP_ONLY, nullptr, nullptr, TiXmlEncoding::TIXML_ENCODING_UNKNOWN);
		return nullptr;
	}

	const auto node = addThis.Clone();
	if (!node)
		return nullptr;
	node->parent = this;

	node->prev = afterThis;
	node->next = afterThis->next;
	if (afterThis->next)
	{
		afterThis->next->prev = node;
	}
	else
	{
		assert(lastChild == afterThis);
		lastChild = node;
	}
	afterThis->next = node;
	return node;
}


TiXmlNode* TiXmlNode::ReplaceChild(TiXmlNode* replaceThis, const TiXmlNode& withThis)
{
	if (!replaceThis)
		return nullptr;

	if (replaceThis->parent != this)
		return nullptr;

	if (withThis.ToDocument())
	{
		// A document can never be a child.	Thanks to Noam.
		if (const auto document = GetDocument(); document)
			document->SetError(tixmlErrors::TIXML_ERROR_DOCUMENT_TOP_ONLY, nullptr, nullptr, TiXmlEncoding::TIXML_ENCODING_UNKNOWN);
		return nullptr;
	}

	const auto node = withThis.Clone();
	if (!node)
		return nullptr;

	node->next = replaceThis->next;
	node->prev = replaceThis->prev;

	if (replaceThis->next)
		replaceThis->next->prev = node;
	else
		lastChild = node;

	if (replaceThis->prev)
		replaceThis->prev->next = node;
	else
		firstChild = node;

	delete replaceThis;
	node->parent = this;
	return node;
}


bool TiXmlNode::RemoveChild(TiXmlNode* removeThis) noexcept
{
	if (!removeThis)
		return false;

	if (removeThis->parent != this)
	{
		assert(0);
		return false;
	}

	if (removeThis->next)
		removeThis->next->prev = removeThis->prev;
	else
		lastChild = removeThis->prev;

	if (removeThis->prev)
		removeThis->prev->next = removeThis->next;
	else
		firstChild = removeThis->next;

	delete removeThis;
	return true;
}

const TiXmlNode* TiXmlNode::FirstChild(const char* _value) const noexcept
{
	if (!_value)
		return firstChild;

	for (const TiXmlNode* node = firstChild; node; node = node->next)
	{
		if (_ts_strcmp(node->Value(), _value) == 0)
			return node;
	}
	return nullptr;
}


const TiXmlNode* TiXmlNode::LastChild(const char* _value) const noexcept
{
	if (!_value)
		return lastChild;

	for (const TiXmlNode* node = lastChild; node; node = node->prev)
	{
		if (_ts_strcmp(node->Value(), _value) == 0)
			return node;
	}
	return nullptr;
}


const TiXmlNode* TiXmlNode::IterateChildren(const TiXmlNode* previous) const noexcept
{
	if (!previous)
	{
		return FirstChild();
	}
	else
	{
		assert(previous->parent == this);
		return previous->NextSibling();
	}
}


const TiXmlNode* TiXmlNode::IterateChildren(const char* val, const TiXmlNode* previous) const noexcept
{
	if (!previous)
	{
		return FirstChild(val);
	}
	else
	{
		assert(previous->parent == this);
		return previous->NextSibling(val);
	}
}


const TiXmlNode* TiXmlNode::NextSibling(const char* _value) const noexcept
{
	for (const TiXmlNode* node = next; node; node = node->next)
	{
		if (_ts_strcmp(node->Value(), _value) == 0)
			return node;
	}
	return nullptr;
}


const TiXmlNode* TiXmlNode::PreviousSibling(const char* _value) const noexcept
{
	for (const TiXmlNode* node = prev; node; node = node->prev)
	{
		if (_ts_strcmp(node->Value(), _value) == 0)
			return node;
	}
	return nullptr;
}


void TiXmlElement::RemoveAttribute(const char* name) noexcept
{
#ifdef TIXML_USE_STL
	TIXML_STRING str(name);
	const auto node = attributeSet.Find(str);
#else
	const auto node = attributeSet.Find(name);
#endif
	if (node)
	{
		attributeSet.Remove(node);
		delete node;
	}
}

const TiXmlElement* TiXmlNode::FirstChildElement() const noexcept
{
	for (const TiXmlNode* node = FirstChild(); node; node = node->NextSibling())
	{
		if (node->ToElement())
			return node->ToElement();
	}
	return nullptr;
}


const TiXmlElement* TiXmlNode::FirstChildElement(const char* _value) const noexcept
{
	for (const TiXmlNode* node = FirstChild(_value); node; node = node->NextSibling(_value))
	{
		if (node->ToElement())
			return node->ToElement();
	}
	return nullptr;
}


const TiXmlElement* TiXmlNode::NextSiblingElement() const noexcept
{
	for (const TiXmlNode* node = NextSibling(); node; node = node->NextSibling())
	{
		if (node->ToElement())
			return node->ToElement();
	}
	return nullptr;
}


const TiXmlElement* TiXmlNode::NextSiblingElement(const char* _value) const noexcept
{
	for (const TiXmlNode* node = NextSibling(_value); node; node = node->NextSibling(_value))
	{
		if (node->ToElement())
			return node->ToElement();
	}
	return nullptr;
}


const TiXmlDocument* TiXmlNode::GetDocument() const noexcept
{
	for (const TiXmlNode* node = this; node; node = node->parent)
	{
		if (node->ToDocument())
			return node->ToDocument();
	}
	return nullptr;
}


TiXmlElement::TiXmlElement(const char* _value)
	: TiXmlNode(TiXmlNode::NodeType::TINYXML_ELEMENT)
{
	firstChild = lastChild = nullptr;
	value = _value;
}


#ifdef TIXML_USE_STL
TiXmlElement::TiXmlElement(const std::string& _value)
	: TiXmlNode(TiXmlNode::TINYXML_ELEMENT)
{
	firstChild = lastChild = 0;
	value = _value;
}
#endif


TiXmlElement::TiXmlElement(const TiXmlElement& copy)
	: TiXmlNode(TiXmlNode::NodeType::TINYXML_ELEMENT)
{
	firstChild = lastChild = nullptr;
	copy.CopyTo(this);
}

//warning C26434 : Function 'TiXmlElement::operator=' hides a non - virtual function 'TiXmlNode::operator=' (c.128: http://go.microsoft.com/fwlink/?linkid=853923).
//TiXmlElement& TiXmlElement::operator=(const TiXmlElement& base)
//{
//	ClearThis();
//	base.CopyTo(this);
//	return *this;
//}


TiXmlElement::~TiXmlElement() noexcept
{
	ClearThis();
}


void TiXmlElement::ClearThis() noexcept
{
	Clear();
	while (attributeSet.First())
	{
		auto node = attributeSet.First();
		attributeSet.Remove(node);
		delete node;
	}
}


const char* TiXmlElement::Attribute(const char* name) const noexcept
{
	if (const TiXmlAttribute* const node = attributeSet.Find(name); node)
		return node->Value();
	return nullptr;
}


#ifdef TIXML_USE_STL
const std::string* TiXmlElement::Attribute(const std::string& name) const
{
	if (const TiXmlAttribute* const attrib = attributeSet.Find(name); attrib)
		return &attrib->ValueStr();
	return nullptr;
}
#endif


const char* TiXmlElement::Attribute(const char* name, int* i) const noexcept
{
	const char* result{ nullptr };

	if (const TiXmlAttribute* const attrib = attributeSet.Find(name); attrib)
	{
		result = attrib->Value();
		if (i)
			attrib->QueryIntValue(i);
	}
	return result;
}


#ifdef TIXML_USE_STL
const std::string* TiXmlElement::Attribute(const std::string& name, int* i) const
{
	const std::string* result{ nullptr };

	if (const TiXmlAttribute* const attrib = attributeSet.Find(name); attrib != nullptr)
	{
		result = &attrib->ValueStr();
		if (i)
			attrib->QueryIntValue(i);
	}
	return result;
}
#endif


const char* TiXmlElement::Attribute(const char* name, double* d) const noexcept
{
	const char* result{ nullptr };

	if (const TiXmlAttribute* const attrib = attributeSet.Find(name); attrib)
	{
		result = attrib->Value();
		if (d)
			attrib->QueryDoubleValue(d);
	}
	return result;
}


#ifdef TIXML_USE_STL
const std::string* TiXmlElement::Attribute(const std::string& name, double* d) const
{
	const std::string* result{ nullptr };

	if (const TiXmlAttribute* const attrib = attributeSet.Find(name); attrib)
	{
		result = &attrib->ValueStr();
		if (d)
			attrib->QueryDoubleValue(d);
	}
	return result;
}
#endif


TiXmlReturns TiXmlElement::QueryIntAttribute(const char* name, int* const ival) const noexcept
{
	if (const TiXmlAttribute* const attrib = attributeSet.Find(name); attrib)
		return attrib->QueryIntValue(ival);
	return TiXmlReturns::TIXML_NO_ATTRIBUTE;
}

std::pair<TiXmlReturns, int> TiXmlElement::QueryIntAttribute(const char* name) const noexcept
{
	int iVal{};
	const auto status = QueryIntAttribute(name, &iVal);
	return{ status,iVal };
}


#ifdef TIXML_USE_STL
TiXmlReturns TiXmlElement::QueryIntAttribute(const std::string& name, int* const ival) const
{
	if (const TiXmlAttribute* const attrib = attributeSet.Find(name); attrib)
		return attrib->QueryIntValue(ival);
	return TiXmlReturns::TIXML_NO_ATTRIBUTE;
}
#endif


TiXmlReturns TiXmlElement::QueryDoubleAttribute(const char* name, double* const dval) const noexcept
{
	if (const TiXmlAttribute* const attrib = attributeSet.Find(name); attrib)
		return attrib->QueryDoubleValue(dval);
	return TiXmlReturns::TIXML_NO_ATTRIBUTE;
}


#ifdef TIXML_USE_STL
TiXmlReturns TiXmlElement::QueryDoubleAttribute(const std::string& name, double* const dval) const
{
	if (const TiXmlAttribute* const attrib = attributeSet.Find(name); attrib)
		return attrib->QueryDoubleValue(dval);
	return TIXML_NO_ATTRIBUTE;
}
#endif


void TiXmlElement::SetAttribute(const char* name, int val)
{
	if (const auto attrib = attributeSet.FindOrCreate(name); attrib)
		attrib->SetIntValue(val);
}


#ifdef TIXML_USE_STL
void TiXmlElement::SetAttribute(const std::string& name, int val)
{
	if (const auto attrib = attributeSet.FindOrCreate(name); attrib)
		attrib->SetIntValue(val);
}
#endif


void TiXmlElement::SetDoubleAttribute(const char* name, double val)
{
	if (const auto attrib = attributeSet.FindOrCreate(name); attrib)
		attrib->SetDoubleValue(val);
}


#ifdef TIXML_USE_STL
void TiXmlElement::SetDoubleAttribute(const std::string& name, double val)
{
	if (const auto attrib = attributeSet.FindOrCreate(name); attrib)
		attrib->SetDoubleValue(val);
}
#endif 


void TiXmlElement::SetAttribute(const char* cname, const char* cvalue)
{
	if (const auto attrib = attributeSet.FindOrCreate(cname); attrib)
		attrib->SetValue(cvalue);
}


#ifdef TIXML_USE_STL
void TiXmlElement::SetAttribute(const std::string& _name, const std::string& _value)
{
	if (const auto attrib = attributeSet.FindOrCreate(_name); attrib)
		attrib->SetValue(_value);
}
#endif


void TiXmlElement::Print(FILE* cfile, int depth) const
{
	assert(cfile);
	for (auto i = 0; i < depth; ++i)
	{
		fprintf(cfile, "    ");
	}

	fprintf(cfile, "<%s", value.c_str());

	for (const TiXmlAttribute* attrib = attributeSet.First(); attrib; attrib = attrib->Next())
	{
		fprintf(cfile, " ");
		attrib->Print(cfile, depth);
	}

	// There are 3 different formatting approaches:
	// 1) An m_pElement without children is printed as a <foo /> node
	// 2) An m_pElement with only a text child is printed as <foo> text </foo>
	// 3) An m_pElement with children is printed on multiple lines.
	if (!firstChild)
	{
		fprintf(cfile, " />");
	}
	else if (firstChild == lastChild && firstChild->ToText())
	{
		fprintf(cfile, ">");
		firstChild->Print(cfile, depth + 1);
		fprintf(cfile, "</%s>", value.c_str());
	}
	else
	{
		fprintf(cfile, ">");

		for (const TiXmlNode* node = firstChild; node; node = node->NextSibling())
		{
			if (!node->ToText())
			{
				fprintf(cfile, "\n");
			}
			node->Print(cfile, depth + 1);
		}
		fprintf(cfile, "\n");
		for (auto i = 0; i < depth; ++i)
		{
			fprintf(cfile, "    ");
		}
		fprintf(cfile, "</%s>", value.c_str());
	}
}


void TiXmlElement::CopyTo(TiXmlElement* target) const
{
	// superclass:
	TiXmlNode::CopyTo(target);

	// Element class: 
	// Clone the attributes, then clone the children.
	for (const TiXmlAttribute* attribute = attributeSet.First(); attribute; attribute = attribute->Next())
	{
		target->SetAttribute(attribute->Name(), attribute->Value());
	}

	for (const TiXmlNode* node = firstChild; node; node = node->NextSibling())
	{
		target->LinkEndChild(node->Clone());
	}
}

bool TiXmlElement::Accept(TiXmlVisitor* visitor) const
{
	if (visitor->VisitEnter(*this, attributeSet.First()))
	{
		for (const TiXmlNode* node = FirstChild(); node; node = node->NextSibling())
		{
			if (!node->Accept(visitor))
				break;
		}
	}
	return visitor->VisitExit(*this);
}


TiXmlNode* TiXmlElement::Clone() const
{
	try {
		//auto clone = new TiXmlElement(Value());

		//CopyTo(clone);
		//return clone;

		auto clone = std::make_unique<TiXmlElement>(Value());
		CopyTo(clone.get());
		return clone.release();
	}
	catch (const std::bad_alloc)
	{
		return nullptr;
	}
}


const char* TiXmlElement::GetText() const noexcept
{
	if (const TiXmlNode* const child = this->FirstChild(); child)
	{
		if (const TiXmlText* const childText = child->ToText(); childText)
			return childText->Value();
	}
	return nullptr;
}


TiXmlDocument::TiXmlDocument() noexcept
	: TiXmlNode(TiXmlNode::NodeType::TINYXML_DOCUMENT)
{
}

TiXmlDocument::TiXmlDocument(const char* documentName)
	: TiXmlDocument()
{
	value = documentName;
}


#ifdef TIXML_USE_STL
TiXmlDocument::TiXmlDocument(const std::string& documentName)
	: TiXmlDocument()
{
	value = documentName;
}
#endif


TiXmlDocument::TiXmlDocument(const TiXmlDocument& copy)
	: TiXmlDocument()
{
	copy.CopyTo(this);
}

//warning C26434 : Function 'TiXmlDocument::operator=' hides a non - virtual function 'TiXmlNode::operator=' (c.128: http://go.microsoft.com/fwlink/?linkid=853923).
TiXmlDocument& TiXmlDocument::operator=(const TiXmlDocument& copy)
{
	Clear();
	copy.CopyTo(this);
	return *this;
}

bool TiXmlDocument::LoadFile(TiXmlEncoding encoding)
{
	return LoadFile(Value(), encoding);
}


bool TiXmlDocument::SaveFile() const
{
	return SaveFile(Value());
}

bool TiXmlDocument::LoadFile(const char* _filename, TiXmlEncoding encoding)
{
	TIXML_STRING filename(_filename);
	value = filename;

	// reading in binary mode so that tinyxml can normalize the EOL
	if (const auto file = TiXmlFOpen(value.c_str(), "rb"); file != nullptr)
	{
		const bool result = LoadFile(file, encoding);
		fclose(file);
		return result;
	}
	else
	{
		SetError(tixmlErrors::TIXML_ERROR_OPENING_FILE, nullptr, nullptr, TiXmlEncoding::TIXML_ENCODING_UNKNOWN);
		return false;
	}
}

bool TiXmlDocument::LoadFile(FILE* file, TiXmlEncoding encoding)
{
	if (!file)
	{
		SetError(tixmlErrors::TIXML_ERROR_OPENING_FILE, nullptr, nullptr, TiXmlEncoding::TIXML_ENCODING_UNKNOWN);
		return false;
	}

	// Delete the existing data:
	Clear();
	location.Clear();

	// Get the file size, so we can pre-allocate the string. HUGE speed impact.
	fseek(file, 0, SEEK_END);
	const auto length = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Strange case, but good to handle up front.
	if (length <= 0)
	{
		SetError(tixmlErrors::TIXML_ERROR_DOCUMENT_EMPTY, nullptr, nullptr, TiXmlEncoding::TIXML_ENCODING_UNKNOWN);
		return false;
	}

	// Subtle bug here. TinyXml did use fgets. But from the XML spec:
	// 2.11 End-of-Line Handling
	// <snip>
	// <quote>
	// ...the XML processor MUST behave as if it normalized all line breaks in external 
	// parsed entities (including the document entity) on input, before parsing, by translating 
	// both the two-character sequence #xD #xA and any #xD that is not followed by #xA to 
	// a single #xA character.
	// </quote>
	//
	// It is not clear fgets does that, and certainly isn't clear it works cross platform. 
	// Generally, you expect fgets to translate from the convention of the OS to the c/unix
	// convention, and not work generally.

	/*
	while( fgets( buf, sizeof(buf), file ) )
	{
		data += buf;
	}
	*/

	//char* buf = new char[ length+1 ];
	auto buf = std::make_unique<char[]>(static_cast<size_t>(length) + 1U);
	buf[0] = 0;

	if (fread(buf.get(), static_cast<size_t>(length), 1U, file) != 1)
	{
		SetError(tixmlErrors::TIXML_ERROR_OPENING_FILE, nullptr, nullptr, TiXmlEncoding::TIXML_ENCODING_UNKNOWN);
		return false;
	}

	// Process the buffer in place to normalize new lines. (See comment above.)
	// Copies from the 'p' to 'q' pointer, where p can advance faster if
	// a newline-carriage return is hit.
	//
	// Wikipedia:
	// Systems based on ASCII or a compatible character set use either LF  (Line feed, '\n', 0x0A, 10 in decimal) or 
	// CR (Carriage return, '\r', 0x0D, 13 in decimal) individually, or CR followed by LF (CR+LF, 0x0D 0x0A)...
	//		* LF:    Multics, Unix and Unix-like systems (GNU/Linux, AIX, Xenix, Mac OS X, FreeBSD, etc.), BeOS, Amiga, RISC OS, and others
	//		* CR+LF: DEC RT-11 and most other early non-Unix, non-IBM OSes, CP/M, MP/M, DOS, OS/2, Microsoft Windows, Symbian OS
	//		* CR:    Commodore 8-bit machines, Apple II family, Mac OS up to version 9 and OS-9

	const char* p = buf.get();	// the read head
	char* q = buf.get();			// the write head
	constexpr char CR{ 0x0d };
	constexpr char LF{ 0x0a };

	buf[static_cast<size_t>(length)] = 0;
	while (*p)
	{
		assert(p < (buf.get() + length));
		assert(q <= (buf.get() + length));
		assert(q <= p);

		if (*p == CR)
		{
			*q++ = LF;
			++p;
			if (*p == LF)
			{		// check for CR+LF (and skip LF)
				++p;
			}
		}
		else {
			*q++ = *p++;
		}
	}
	assert(q <= (buf.get() + length));
	*q = 0;

	Parse(buf.get(), nullptr, encoding);

	return !Error();
}


bool TiXmlDocument::SaveFile(const char* filename) const
{
	// The old c stuff lives on...
	if (const auto fp = TiXmlFOpen(filename, "w"); fp)
	{
		//const bool result = SaveFile(fp);
		//fclose(fp);
		//return result;

		Auto(fclose(fp));
		return SaveFile(fp);
	}
	return false;
}


bool TiXmlDocument::SaveFile(FILE* fp) const
{
	if (useMicrosoftBOM)
	{
		constexpr unsigned char TIXML_UTF_LEAD_0 = 0xefU;
		constexpr unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
		constexpr unsigned char TIXML_UTF_LEAD_2 = 0xbfU;

		fputc(TIXML_UTF_LEAD_0, fp);
		fputc(TIXML_UTF_LEAD_1, fp);
		fputc(TIXML_UTF_LEAD_2, fp);
	}
	Print(fp, 0);
	return (ferror(fp) == 0);
}


void TiXmlDocument::CopyTo(TiXmlDocument* target) const
{
	TiXmlNode::CopyTo(target);

	target->error = error;
	target->errorId = errorId;
	target->errorDesc = errorDesc;
	target->tabsize = tabsize;
	target->errorLocation = errorLocation;
	target->useMicrosoftBOM = useMicrosoftBOM;

	for (const TiXmlNode* node = firstChild; node; node = node->NextSibling())
	{
		target->LinkEndChild(node->Clone());
	}
}


TiXmlNode* TiXmlDocument::Clone() const
{
	try {
		//auto clone = new TiXmlDocument();

		//CopyTo(clone);
		//return clone;

		auto clone = std::make_unique<TiXmlDocument>();

		CopyTo(clone.get());
		return clone.release();
	}
	catch (const std::bad_alloc)
	{
		return nullptr;
	}
}


void TiXmlDocument::Print(FILE* cfile, int depth) const
{
	assert(cfile);
	for (const TiXmlNode* node = FirstChild(); node; node = node->NextSibling())
	{
		node->Print(cfile, depth);
		fprintf(cfile, "\n");
	}
}


bool TiXmlDocument::Accept(TiXmlVisitor* visitor) const
{
	if (visitor->VisitEnter(*this))
	{
		for (const TiXmlNode* node = FirstChild(); node; node = node->NextSibling())
		{
			if (!node->Accept(visitor))
				break;
		}
	}
	return visitor->VisitExit(*this);
}


const TiXmlAttribute* TiXmlAttribute::Next() const noexcept
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if (next->value.empty() && next->name.empty())
		return nullptr;
	return next;
}

const TiXmlAttribute* TiXmlAttribute::Previous() const noexcept
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if (prev->value.empty() && prev->name.empty())
		return nullptr;
	return prev;
}

void TiXmlAttribute::Print(FILE* cfile, int /*depth*/, TIXML_STRING* str) const
{
	TIXML_STRING n, v;

	EncodeString(name, &n);
	EncodeString(value, &v);

	if (value.find('\"') == TIXML_STRING::npos)
	{
		if (cfile)
		{
			fprintf(cfile, "%s=\"%s\"", n.c_str(), v.c_str());
		}
		if (str)
		{
			(*str) += n; (*str) += "=\""; (*str) += v; (*str) += "\"";
		}
	}
	else {
		if (cfile)
		{
			fprintf(cfile, "%s='%s'", n.c_str(), v.c_str());
		}
		if (str)
		{
			(*str) += n; (*str) += "='"; (*str) += v; (*str) += "'";
		}
	}
}


TiXmlReturns TiXmlAttribute::QueryIntValue(int* const ival) const noexcept
{
	if (TIXML_SSCANF(value.c_str(), "%d", ival) == 1)
		return TiXmlReturns::TIXML_SUCCESS;
	return TiXmlReturns::TIXML_WRONG_TYPE;
}

TiXmlReturns TiXmlAttribute::QueryDoubleValue(double* const dval) const noexcept
{
	if (TIXML_SSCANF(value.c_str(), "%lf", dval) == 1)
		return TiXmlReturns::TIXML_SUCCESS;
	return TiXmlReturns::TIXML_WRONG_TYPE;
}

void TiXmlAttribute::SetIntValue(const int _value)
{
	char buf[64]{};
#if defined(TIXML_SNPRINTF)		
	TIXML_SNPRINTF(buf, sizeof(buf), "%d", _value);
#else
	sprintf(buf, "%d", _value);
#endif
	SetValue(&buf[0]);
}

void TiXmlAttribute::SetDoubleValue(const double _value)
{
	char buf[256]{};
#if defined(TIXML_SNPRINTF)
	TIXML_SNPRINTF(buf, sizeof(buf), "%g", _value);
#else
	sprintf(buf, "%g", _value);
#endif
	SetValue(&buf[0]);
}

int TiXmlAttribute::IntValue() const noexcept
{
	return _ts_atoi(value.c_str());
}

double  TiXmlAttribute::DoubleValue() const noexcept
{
	return _ts_atof(value.c_str());
}


TiXmlComment::TiXmlComment(const TiXmlComment& copy)
	: TiXmlNode(TiXmlNode::NodeType::TINYXML_COMMENT)
{
	copy.CopyTo(this);
}

//warning C26434 : Function 'TiXmlComment::operator=' hides a non - virtual function 'TiXmlNode::operator=' (c.128: http://go.microsoft.com/fwlink/?linkid=853923).
//TiXmlComment& TiXmlComment::operator=(const TiXmlComment& base)
//{
//	Clear();
//	base.CopyTo(this);
//	return *this;
//}


void TiXmlComment::Print(FILE* cfile, int depth) const noexcept
{
	assert(cfile);
	for (int i = 0; i < depth; ++i)
	{
		fprintf(cfile, "    ");
	}
	fprintf(cfile, "<!--%s-->", value.c_str());
}

//warning C26434 : Function 'TiXmlComment::CopyTo' hides a non - virtual function 'TiXmlNode::CopyTo' (c.128: http://go.microsoft.com/fwlink/?linkid=853923).
//void TiXmlComment::CopyTo( TiXmlComment* target ) const
//{
//	TiXmlNode::CopyTo( target );
//}


bool TiXmlComment::Accept(TiXmlVisitor* visitor) const
{
	return visitor->Visit(*this);
}


TiXmlNode* TiXmlComment::Clone() const
{
	try {
		//auto clone = new TiXmlComment();
		//
		//CopyTo(clone);
		//return clone;

		auto clone = std::make_unique<TiXmlComment>();

		CopyTo(clone.get());
		return clone.release();
	}
	catch (const std::bad_alloc)
	{
		return nullptr;
	}
}


void TiXmlText::Print(FILE* cfile, int depth) const
{
	assert(cfile);
	if (cdata)
	{
		fprintf(cfile, "\n");
		for (int i = 0; i < depth; ++i)
		{
			fprintf(cfile, "    ");
		}
		fprintf(cfile, "<![CDATA[%s]]>\n", value.c_str());	// unformatted output
	}
	else
	{
		TIXML_STRING buffer;
		EncodeString(value, &buffer);
		fprintf(cfile, "%s", buffer.c_str());
	}
}


void TiXmlText::CopyTo(TiXmlText* target) const
{
	TiXmlNode::CopyTo(target);
	target->cdata = cdata;
}


bool TiXmlText::Accept(TiXmlVisitor* visitor) const
{
	return visitor->Visit(*this);
}


TiXmlNode* TiXmlText::Clone() const
{
	try {
		//const auto clone = new TiXmlText();
		//
		//CopyTo(clone);
		//return clone;

		auto clone = std::make_unique<TiXmlText>();

		CopyTo(clone.get());
		return clone.release();
	}
	catch (const std::bad_alloc)
	{
		return nullptr;
	}
}


TiXmlDeclaration::TiXmlDeclaration(const char* _version, const char* _encoding, const char* _standalone)
	: TiXmlNode(TiXmlNode::NodeType::TINYXML_DECLARATION)
	, version(_version), encoding(_encoding), standalone(_standalone)
{
}


#ifdef TIXML_USE_STL
TiXmlDeclaration::TiXmlDeclaration(const std::string& _version, const std::string& _encoding, const std::string& _standalone)
	: TiXmlNode(TiXmlNode::TINYXML_DECLARATION)
	, version(_version), encoding(_encoding), standalone(_standalone)
{
}
#endif


TiXmlDeclaration::TiXmlDeclaration(const TiXmlDeclaration& copy)
	: TiXmlNode(TiXmlNode::NodeType::TINYXML_DECLARATION)
{
	copy.CopyTo(this);
}

//warning C26434 : Function 'TiXmlDeclaration::operator=' hides a non - virtual function 'TiXmlNode::operator=' (c.128: http://go.microsoft.com/fwlink/?linkid=853923).
//TiXmlDeclaration& TiXmlDeclaration::operator=(const TiXmlDeclaration& copy)
//{
//	Clear();
//	copy.CopyTo(this);
//	return *this;
//}

void TiXmlDeclaration::Print(FILE* cfile, int /*depth*/, TIXML_STRING* str) const
{
	if (cfile)
		fprintf(cfile, "<?xml ");
	if (str)
		(*str) += "<?xml ";

	if (!version.empty())
	{
		if (cfile)
			fprintf(cfile, "version=\"%s\" ", version.c_str());
		if (str)
		{
			(*str) += "version=\"";
			(*str) += version;
			(*str) += "\" ";
		}
	}
	if (!encoding.empty())
	{
		if (cfile)
			fprintf(cfile, "encoding=\"%s\" ", encoding.c_str());
		if (str)
		{
			(*str) += "encoding=\"";
			(*str) += encoding;
			(*str) += "\" ";
		}
	}
	if (!standalone.empty())
	{
		if (cfile)
			fprintf(cfile, "standalone=\"%s\" ", standalone.c_str());
		if (str)
		{
			(*str) += "standalone=\"";
			(*str) += standalone;
			(*str) += "\" ";
		}
	}
	if (cfile)
		fprintf(cfile, "?>");
	if (str)
		(*str) += "?>";
}

void TiXmlDeclaration::CopyTo(TiXmlDeclaration* target) const
{
	TiXmlNode::CopyTo(target);

	target->version = version;
	target->encoding = encoding;
	target->standalone = standalone;
}


bool TiXmlDeclaration::Accept(TiXmlVisitor* visitor) const
{
	return visitor->Visit(*this);
}


TiXmlNode* TiXmlDeclaration::Clone() const
{
	try {
		//auto clone = new TiXmlDeclaration();
		//
		//CopyTo(clone);
		//return clone;

		auto clone = std::make_unique<TiXmlDeclaration>();

		CopyTo(clone.get());
		return clone.release();
	}
	catch (const std::bad_alloc)
	{
		return nullptr;
	}
}


void TiXmlUnknown::Print(FILE* cfile, int depth) const noexcept
{
	for (int i = 0; i < depth; ++i)
		fprintf(cfile, "    ");
	fprintf(cfile, "<%s>", value.c_str());
}

//void TiXmlUnknown::CopyTo( TiXmlUnknown* target ) const
//{
//	TiXmlNode::CopyTo( target );
//}

bool TiXmlUnknown::Accept(TiXmlVisitor* visitor) const
{
	return visitor->Visit(*this);
}


TiXmlNode* TiXmlUnknown::Clone() const
{
	try {
		//auto clone = new TiXmlUnknown();
		//
		//CopyTo(clone);
		//return clone;

		auto clone = std::make_unique<TiXmlUnknown>();

		CopyTo(clone.get());
		return clone.release();
	}
	catch (const std::bad_alloc)
	{
		return nullptr;
	}
}


TiXmlAttributeSet::TiXmlAttributeSet() noexcept
	: sentinel()
{
	sentinel.next = &sentinel;
	sentinel.prev = &sentinel;
}


TiXmlAttributeSet::~TiXmlAttributeSet() noexcept
{
	assert(sentinel.next == &sentinel);
	assert(sentinel.prev == &sentinel);
}


void TiXmlAttributeSet::Add(TiXmlAttribute* addMe) noexcept
{
	assert(addMe != nullptr);

#ifdef TIXML_USE_STL
	assert(!Find(TIXML_STRING(addMe->Name())));	// Shouldn't be multiply adding to the set.
#else
	assert(!Find(addMe->Name()));	// Shouldn't be multiply adding to the set.
#endif

	addMe->next = &sentinel;
	addMe->prev = sentinel.prev;

	sentinel.prev->next = addMe;
	sentinel.prev = addMe;
}

void TiXmlAttributeSet::Remove(const TiXmlAttribute* const removeMe) noexcept
{
	for (auto node = sentinel.next; node != &sentinel; node = node->next)
	{
		if (node == removeMe)
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
			node->next = nullptr;
			node->prev = nullptr;
			return;
		}
	}
	assert(0);		// we tried to remove a non-linked attribute.
}


#ifdef TIXML_USE_STL
TiXmlAttribute* TiXmlAttributeSet::Find(const std::string& name) const
{
	for (auto node = sentinel.next; node != &sentinel; node = node->next)
	{
		if (node->name == name)
			return node;
	}
	return nullptr;
}

TiXmlAttribute* TiXmlAttributeSet::FindOrCreate(const std::string& _name)
{
	auto attrib = Find(_name);
	if (!attrib)
	{
		attrib = new TiXmlAttribute();
		Add(attrib);
		attrib->SetName(_name);
	}
	return attrib;
}
#endif


TiXmlAttribute* TiXmlAttributeSet::Find(const char* name) const noexcept
{
	if (!name)
		return nullptr;

	for (auto node = sentinel.next; node != &sentinel; node = node->next)
	{
		if (_ts_strcmp(node->name.c_str(), name) == 0)
			return node;
	}
	return nullptr;
}


TiXmlAttribute* TiXmlAttributeSet::FindOrCreate(const char* _name)
{
	auto attrib = Find(_name);
	if (!attrib)
	{
		attrib = new TiXmlAttribute();
		Add(attrib);
		attrib->SetName(_name);
	}
	return attrib;
}


#ifdef TIXML_USE_STL	
std::istream& operator>> (std::istream& in, TiXmlNode& base)
{
	TIXML_STRING tag;
	tag.reserve(8 * 1000);
	base.StreamIn(&in, &tag);

	base.Parse(tag.c_str(), 0, TIXML_DEFAULT_ENCODING);
	return in;
}
#endif


#ifdef TIXML_USE_STL	
std::ostream& operator<< (std::ostream& out, const TiXmlNode& base)
{
	TiXmlPrinter printer;
	printer.SetStreamPrinting();
	base.Accept(&printer);
	out << printer.Str();

	return out;
}


std::string& operator<< (std::string& out, const TiXmlNode& base)
{
	TiXmlPrinter printer;
	printer.SetStreamPrinting();
	base.Accept(&printer);
	out.append(printer.Str());

	return out;
}
#endif


TiXmlHandle TiXmlHandle::FirstChild() const noexcept
{
	if (node)
	{
		if (const auto child = node->FirstChild(); child)
			return{ child };
	}
	return{};
}


TiXmlHandle TiXmlHandle::FirstChild(const char* value) const noexcept
{
	if (node)
	{
		if (const auto child = node->FirstChild(value); child)
			return{ child };
	}
	return{};
}


TiXmlHandle TiXmlHandle::FirstChildElement() const noexcept
{
	if (node)
	{
		if (const auto child = node->FirstChildElement(); child)
			return{ child };
	}
	return{};
}


TiXmlHandle TiXmlHandle::FirstChildElement(const char* value) const noexcept
{
	if (node)
	{
		if (const auto child = node->FirstChildElement(value); child)
			return{ child };
	}
	return{};
}


TiXmlHandle TiXmlHandle::Child(int count) const noexcept
{
	if (node)
	{
		auto child = node->FirstChild();
		for (auto i = 0; child && i < count; child = child->NextSibling(), ++i)
		{
			// nothing
		}
		if (child)
			return{ child };
	}
	return{};
}


TiXmlHandle TiXmlHandle::Child(const char* value, int count) const noexcept
{
	if (node)
	{
		auto child = node->FirstChild(value);
		for (auto i = 0; child && i < count; child = child->NextSibling(value), ++i)
		{
			// nothing
		}
		if (child)
			return{ child };
	}
	return{};
}


TiXmlHandle TiXmlHandle::ChildElement(int count) const noexcept
{
	if (node)
	{
		auto child = node->FirstChildElement();
		for (auto i = 0; child && i < count; child = child->NextSiblingElement(), ++i)
		{
			// nothing
		}
		if (child)
			return{ child };
	}
	return{};
}


TiXmlHandle TiXmlHandle::ChildElement(const char* value, int count) const noexcept
{
	if (node)
	{
		auto child = node->FirstChildElement(value);
		for (auto i = 0; child && i < count; child = child->NextSiblingElement(value), ++i)
		{
			// nothing
		}
		if (child)
			return{ child };
	}
	return{};
}


bool TiXmlPrinter::VisitEnter(const TiXmlDocument&) noexcept
{
	return true;
}

bool TiXmlPrinter::VisitExit(const TiXmlDocument&) noexcept
{
	return true;
}

bool TiXmlPrinter::VisitEnter(const TiXmlElement& element, const TiXmlAttribute* firstAttribute)
{
	DoIndent();
	buffer += "<";
	buffer += element.Value();

	for (const auto* attrib = firstAttribute; attrib; attrib = attrib->Next())
	{
		buffer += " ";
		attrib->Print(nullptr, 0, &buffer);
	}

	if (!element.FirstChild())
	{
		buffer += " />";
		DoLineBreak();
	}
	else
	{
		buffer += ">";
		if (element.FirstChild()->ToText()
			&& element.LastChild() == element.FirstChild()
			&& element.FirstChild()->ToText()->CDATA() == false)
		{
			simpleTextPrint = true;
			// no DoLineBreak()!
		}
		else
		{
			DoLineBreak();
		}
	}
	++depth;
	return true;
}


bool TiXmlPrinter::VisitExit(const TiXmlElement& element)
{
	--depth;
	if (!element.FirstChild())
	{
		// nothing.
	}
	else
	{
		if (simpleTextPrint)
		{
			simpleTextPrint = false;
		}
		else
		{
			DoIndent();
		}
		buffer += "</";
		buffer += element.Value();
		buffer += ">";
		DoLineBreak();
	}
	return true;
}


bool TiXmlPrinter::Visit(const TiXmlText& text)
{
	if (text.CDATA())
	{
		DoIndent();
		buffer += "<![CDATA[";
		buffer += text.Value();
		buffer += "]]>";
		DoLineBreak();
	}
	else if (simpleTextPrint)
	{
		TIXML_STRING str;
		TiXmlBase::EncodeString(text.ValueTStr(), &str);
		buffer += str;
	}
	else
	{
		DoIndent();
		TIXML_STRING str;
		TiXmlBase::EncodeString(text.ValueTStr(), &str);
		buffer += str;
		DoLineBreak();
	}
	return true;
}


bool TiXmlPrinter::Visit(const TiXmlDeclaration& declaration)
{
	DoIndent();
	declaration.Print(nullptr, 0, &buffer);
	DoLineBreak();
	return true;
}


bool TiXmlPrinter::Visit(const TiXmlComment& comment)
{
	DoIndent();
	buffer += "<!--";
	buffer += comment.Value();
	buffer += "-->";
	DoLineBreak();
	return true;
}


bool TiXmlPrinter::Visit(const TiXmlUnknown& unknown)
{
	DoIndent();
	buffer += "<";
	buffer += unknown.Value();
	buffer += ">";
	DoLineBreak();
	return true;
}

