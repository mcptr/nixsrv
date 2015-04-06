#include "value.hxx"


namespace nix {
namespace value {


Value::Value(const Json::Value& v)
	: value_(v)
{
}

Json::Value Value::get_value() const
{
	return this->value_;
}

std::string Value::to_string(bool pretty) const
{
	if(pretty) {
		return this->value_.toStyledString();
	}
	Json::FastWriter writer;
	return writer.write(value_);
}

Value::operator Json::Value() const
{
	return value_;
}



Object::Object()
	: Value(Json::objectValue)
{
}


Array::Array()
	: Value(Json::arrayValue)
{
}


Null::Null()
	: Value(Json::nullValue)
{
}


} // value
} // nix
