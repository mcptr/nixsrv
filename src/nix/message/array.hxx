#ifndef NIX_MESSAGE_ARRAY_HXX
#define NIX_MESSAGE_ARRAY_HXX

#include <string>
#include "element.hxx"


namespace nix {


class Array;
class Object;


class Array : public Element<mongo::BSONArrayBuilder, mongo::BSONObj>
{
public:
	Array() = default;
	virtual ~Array() = default;

	virtual Array& append_null();
	virtual Array& append(int v);
	virtual Array& append(long long v);
	virtual Array& append(double v);
	virtual Array& append(const std::string& v);
	virtual Array& append(Element& v);
};


} // nix

#endif
