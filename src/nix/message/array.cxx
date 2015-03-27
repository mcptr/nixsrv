#include "array.hxx"


namespace nix {
namespace core {


Array& Array::append_null()
{
	this->data_.appendNull();
	return *this;
}

Array& Array::append(int v)
{
	this->data_.append(v);
	return *this;
}

Array& Array::append(long long v)
{
	this->data_.append(v);
	return *this;
}

Array& Array::append(double v)
{
	this->data_.append(v);
	return *this;
}

Array& Array::append(const std::string& v)
{
	this->data_.append(v);
	return *this;
}

Array& Array::append(Element& v)
{
	this->data_.append(v.get_obj());
	return *this;
}


} // core
} // nix
