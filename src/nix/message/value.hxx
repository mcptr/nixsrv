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

	friend std::ostream& operator<<(std::ostream& os, const Value& v)
	{
		v.to_ostream(os);
		return os;
	}

protected:
	Json::Value value_;
	virtual void to_ostream(std::ostream& os) const;
};

class Object : public Value
{
public:
	explicit Object(const Json::Value& v = Json::objectValue);

	friend std::ostream& operator<<(std::ostream& os, const Object& v)
	{
		v.to_ostream(os);
		return os;
	}
};


class Array : public Value
{
public:
	//Array();
	explicit Array(const Json::Value& v = Json::arrayValue);

	friend std::ostream& operator<<(std::ostream& os, const Array& v)
	{
		v.to_ostream(os);
		return os;
	}
};


class Null : public Value
{
public:
	Null();

	friend std::ostream& operator<<(std::ostream& os, const Null& v)
	{
		v.to_ostream(os);
		return os;
	}
};


} // value
} // nix


#endif
