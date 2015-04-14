#include "value.hxx"


namespace nix {
namespace value {


Value::Value(const Json::Value& v)
	: value_(v)
{
}

void Value::to_ostream(std::ostream& os) const
{
	os << value_;
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



Object::Object(const Json::Value& v)
	: Value(v.isObject() ? v : Json::Value())
{
}


Array::Array(const Json::Value& v)
	: Value(v.isArray() ? v : Json::Value())
{
}


Null::Null()
	: Value(Json::nullValue)
{
}


} // value
} // nix
