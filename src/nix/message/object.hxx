#ifndef NIX_MESSAGE_OBJECT_HXX
#define NIX_MESSAGE_OBJECT_HXX

#include <string>

#include "element.hxx"
// careful - circular deps ahead
#include "array.hxx"


namespace nix {


class Array;

class Object : public Element<mongo::BSONObjBuilder, mongo::BSONObj>
{
public:
	Object() = default;
	virtual ~Object() = default;

	virtual Object& set_null(const std::string& k);
	virtual Object& set(const std::string& k, int v);
	virtual Object& set(const std::string& k, long long v);
	virtual Object& set(const std::string& k, double v);
	virtual Object& set(const std::string& k, const std::string& v);
	virtual Object& set(const std::string& k, Array& v);
 	virtual Object& set(const std::string& k, Element& v);
	
};


} // nix

#endif
