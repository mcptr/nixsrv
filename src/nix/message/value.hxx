#ifndef NIX_MESSAGE_VALUE_HXX
#define NIX_MESSAGE_VALUE_HXX

#include <jsoncpp/reader.h>
#include <jsoncpp/writer.h>
#include <jsoncpp/value.h>
#include <string>


namespace nix {
namespace value {


class Value
{
public:
	Value() = delete;
	Value(const Json::Value& v);
	virtual ~Value() = default;

	virtual Json::Value get_value() const;
	virtual std::string to_string(bool pretty) const;
	operator Json::Value() const;
protected:
	Json::Value value_;
};

class Object : public Value
{
public:
	Object();
};


class Array : public Value
{
public:
	Array();
};


class Null : public Value
{
public:
	Null();
};


} // value
} // nix


#endif
