#include "object.hxx"


namespace nix {


Object& Object::set_null(const std::string& k)
{
	this->data_.appendNull(k);
	return *this;
}

Object& Object::set(const std::string& k, int v)
{
	this->data_.append(k, v);
	return *this;
}

Object& Object::set(const std::string& k, long long v)
{
	this->data_.append(k, v);
	return *this;
}

Object& Object::set(const std::string& k, double v)
{
	this->data_.append(k, v);
	return *this;
}

Object& Object::set(const std::string& k, const std::string& v)
{
	this->data_.append(k, v);
	return *this;
}

Object& Object::set(const std::string& k, Array& v)
{
	this->data_.appendArray(k, v.get_obj());
	return *this;
}

Object& Object::set(const std::string& k, Element& v)
{
	this->data_.append(k, v.get_obj());
	return *this;
}


} // nix
