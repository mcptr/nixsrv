#ifndef NIX_MESSAGE_HXX
#define NIX_MESSAGE_HXX

#include <iostream>
#include <string>
#include <functional>
#include <mongo/bson/bson.h>
#include <mongo/db/json.h>


namespace nix {


template<class T, class ElementT>
class Message
{
public:
	typedef T Object_t;
	typedef ElementT Element_t;
	typedef typename std::function<void(const Element_t&)> IterFunc_t;

	Message() = default;
	Message(const std::string& id, int version = 0);\
	virtual ~Message() = default;

	const std::string& get_id() const;
	int get_version() const;

	const std::string& get_error_msg() const;
	int get_error_code() const;

	void set_error_code(int code)
	{
		this->content_.append("@error_code", code);
	}

	void set_error_msg(const std::string& msg)
	{
		this->content_.append("@error", msg);
	}


	std::string to_string();
	const T& get_raw() const;

	// field getters
	int get_integer(const std::string& k, int dflt = 0L) const;
	long long get_long(const std::string& k, long long dflt = 0) const;
	std::string get_string(const std::string& k, const std::string& dflt = "") const;
	double get_double(const std::string& k, double dlft = 0) const;
	bool get_bool(const std::string& k, bool dflt = false) const;
	bool is_null(const std::string& k) const;
	bool exists(const std::string& k) const;

	void iter(const std::string& k, IterFunc_t f);

	virtual bool validate() const;
	
	Element_t get_object(const std::string& k, const Element_t& dflt = Element_t()) const
	{
		Element_t el = this->content_.getFieldDotted(k);
		if(el.eoo()) {
			return dflt;
		}
		
		return el;
	}

	Element_t get_array(const std::string& k, const Element_t& dflt = Element_t()) const
	{
		return this->get_object(k, dflt);
	}

	// unsafe
	template <class FieldT>
	const FieldT get_field(const std::string& k, const FieldT& dflt = FieldT()) const
	{
		Element_t el = this->content_.getFieldDotted(k);
		if(el.eoo()) {
			return dflt;
		}
		
		FieldT out;
		try {
			el.Val(out);
		}
		catch(std::exception &e) {
			
		}
		return out;
	}

protected:
	virtual std::string _to_string() = 0;

	std::string id_;
	int version_;
	T content_;
};



template<class T, class ElementT>
Message<T, ElementT>::Message(const std::string& id, int version)
	: id_(id.length() ? id : "Message"), version_(version)
{
}

template<class T, class ElementT>
const std::string& Message<T, ElementT>::get_id() const
{
	return this->id_;
}

template<class T, class ElementT>
int Message<T, ElementT>::get_version() const
{
	return this->version_;
}

template<class T, class ElementT>
std::string Message<T, ElementT>::to_string()
{
	return this->_to_string();
}

template<class T, class ElementT>
const T& Message<T, ElementT>::get_raw() const
{
	return this->content_;
}

template<class T, class ElementT>
void Message<T, ElementT>::iter(const std::string& k, IterFunc_t f)
{
	mongo::BSONObjIterator it(this->content_.getObjectField(k.c_str()));
	while(it.more()) {
		f(it.next());
	}
}

template<class T, class ElementT>
bool Message<T, ElementT>::validate() const
{
	//2DO: implement external json schema based validator 
	return true;
}

template<class T, class ElementT>
int Message<T, ElementT>::get_integer(const std::string& k, int dflt) const
{
	int v = dflt;
	Element_t el = this->content_.getFieldDotted(k);
	if(el.type() == mongo::BSONType::NumberInt) {
		v = el.numberInt();
	}
	return v;
}

template<class T, class ElementT>
long long Message<T, ElementT>::get_long(const std::string& k, long long dflt) const
{
	long long v = dflt;
	Element_t el = this->content_.getFieldDotted(k);
	if(el.type() == mongo::BSONType::NumberLong || el.type() == mongo::BSONType::NumberInt) {
		v = el.numberLong();
	}
	return v;
}

template<class T, class ElementT>
double Message<T, ElementT>::get_double(const std::string& k, double dflt) const
{
	double v = dflt;
	Element_t el = this->content_.getFieldDotted(k);
	if(el.type() == mongo::BSONType::NumberDouble) {
		v = el.numberDouble();
	}
	return v;
}

template<class T, class ElementT>
bool Message<T, ElementT>::get_bool(const std::string& k, bool dflt) const
{
	bool v(dflt);
	Element_t el = this->content_.getFieldDotted(k);
	if(el.type() == mongo::BSONType::Bool) {
		v = el.boolean();
	}
	else if(el.type() == mongo::BSONType::NumberInt) {
		v = (el.numberInt() != 0);
	}
	else if(el.type() == mongo::BSONType::NumberLong) {
	        v = (el.numberLong() != 0);
	}
	else if(el.type() == mongo::BSONType::NumberDouble) {
	        v = (el.numberDouble() != 0);
	}

	return v;
}

template<class T, class ElementT>
std::string Message<T, ElementT>::get_string(const std::string& k, const std::string& dflt) const
{
	std::string v(dflt);
	Element_t el = this->content_.getFieldDotted(k);
	if(el.type() == mongo::BSONType::String) {
		v = el.str();
	}
	return v;
}

template<class T, class ElementT>
bool Message<T, ElementT>::is_null(const std::string& k) const
{
	return this->content_.getFieldDotted(k).isNull();
	
}

template<class T, class ElementT>
bool Message<T, ElementT>::exists(const std::string& k) const
{
	return this->content_.getFieldDotted(k).ok();
}


} // nix

#endif
