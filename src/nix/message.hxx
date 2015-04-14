#ifndef NIX_MESSAGE_HXX
#define NIX_MESSAGE_HXX

#include <jsoncpp/reader.h>
#include <jsoncpp/writer.h>
#include <jsoncpp/value.h>
#include <memory>
#include <string>
#include <exception>

#include "nix/common.hxx"
#include "nix/util/string.hxx"
#include "message/value.hxx"

namespace nix {

using nix::value::Value;
using nix::value::Object;
using nix::value::Array;
using nix::value::Null;

class Message
{
public:
	typedef Object Object_t;
	typedef Array Array_t;
	typedef Null Null_t;

	Message();
	explicit Message(Json::Value& root);
	explicit Message(const std::string& json_string);
	virtual ~Message() = default;

	virtual void parse(const std::string& json_string) final;

	inline Json::Value& raw()
	{
		return root_;
	}

	inline const Json::Value& raw_const() const
	{
		return root_;
	}

	void clear();

	// value getters
	Json::Value get_raw_value(const std::string& k);
	std::string get_serialized(const std::string& k,
							   const std::string& value = std::string());


	virtual std::string get(const std::string& k, const char* default_value) const;
	virtual std::string get(const std::string& k, const std::string& default_value) const;
	virtual int get(const std::string& k, int default_value) const;
	virtual double get(const std::string& k, double default_value) const;
	virtual long long get(const std::string& k, long long default_value) const;
	virtual bool get(const std::string& k, bool default_value) const;
	virtual Array_t get(const std::string& k, Array_t default_value) const;

	// value setters
	void set_object(const std::string& k);
	void set_array(const std::string& k);
	void set_null(const std::string& k);
	void set_deserialized(const std::string& k,
						  const std::string& value = std::string());

	void append_null(const std::string& k);
	void remove(const std::string&k);

	// value check
	bool exists(const std::string& k) const;
	bool is_null(const std::string& k) const;
	bool is_array(const std::string& k) const;
	bool is_object(const std::string& k) const;

	void set_status_code(nix::StatusCode_t status);
	void set_status_msg(const std::string& msg);
	void set_status(nix::StatusCode_t status, const std::string& msg = std::string());

	template< class T>
	void set_meta(const std::string& k, const T& value)
	{
		root_["@" + k] = value;
	}

	template< class T>
	void set(const std::string& k, const T& value)
	{
		std::vector<std::string> keys;
		nix::util::string::split(k, keys, ".");

		Json::Value* ptr = &root_;
		for(auto it = keys.begin(); it != keys.end() - 1; it++) {
			if(!ptr->isMember(*it)) {
				(*ptr)[*it] = Json::objectValue;
			}
			else if(!ptr->isObject()) {
				throw std::runtime_error(
					"Not an object. Cannot set value for key: " + k);
			}
			ptr = &((*ptr)[*it]);
		}

		std::string last = keys.back();
		ptr->removeMember(last);
		(*ptr)[last] = value;
	}


	template< class T>
	void append(const std::string& k, const T& value)
	{
		std::vector<std::string> keys;
		nix::util::string::split(k, keys, ".");

		Json::Value* ptr = &root_;
		for(auto it = keys.begin(); it != keys.end(); it++) {
			if(!ptr->isMember(*it)) {
				(*ptr)[*it] = Json::objectValue;
			}
			else if(!ptr->isObject()) {
				throw std::runtime_error(
					"Not an object. Cannot traverse. " + k);
			}
			ptr = &((*ptr)[*it]);
		}

		if(ptr->empty()) {
			*ptr = Json::arrayValue;
		}

		if(ptr->isArray()) {
			ptr->append(value);
		}
		else {
			throw std::runtime_error("Not an array: " + k);
		}
	}

	std::string to_string(bool pretty = false) const;
	std::string to_string(const std::string& k, bool pretty = false) const;

	friend std::ostream& operator<<(std::ostream& os, const Message& msg)
	{
		return os << msg.to_string();
	}

	friend std::istream& operator>>(std::istream& is, Message& msg)
	{
		is >> msg.root_;
		return is;
	}

protected:
	Json::Value root_;

	bool find(const std::string& k, Json::Value& dest) const;
};


} // nix

#endif
