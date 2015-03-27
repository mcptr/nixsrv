#ifndef NIX_MESSAGE_ELEMENT_HXX
#define NIX_MESSAGE_ELEMENT_HXX

#include <string>
#include <mongo/bson/bson.h>
#include <mongo/db/json.h>


namespace nix {


template <class ContentBuilderT, class ContentT>
class Element
{
public:
	Element() = default;
	virtual ~Element() = default;

	virtual ContentT get_obj()
	{
		return this->data_.obj();
	}

	virtual std::string to_string()
	{
		return this->data_.obj().jsonString();
	}

protected:
	ContentBuilderT data_;
};


} // nix

#endif
