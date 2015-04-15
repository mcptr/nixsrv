#include <cassert>

#include "nix/common.hxx"
#include "message.hxx"


namespace nix {


Message::Message()
	: root_(Json::objectValue)
{
}

Message::Message(Json::Value& root)
	: root_(root)
{
}

Message::Message(const std::string& json_string)
{
	parse(json_string);
}

void Message::parse(const std::string& json_string)
{
	Json::Reader reader;
	
	if(!reader.parse(json_string, root_, false)) {
		LOG(DEBUG) << "Cannot parse message: " << json_string;
		throw std::runtime_error(reader.getFormattedErrorMessages());
	}
}

void Message::clear()
{
	root_.clear();
}

Json::Value Message::get_raw_value(const std::string& k)
{
	Json::Value dest;
	bool found = find(k, dest);
	return (found ? dest : Json::nullValue);
}

std::string Message::get_serialized(
	const std::string& k,
	const std::string& value)
{
	Json::Value dest;
	bool found = find(k, dest);
	
	if(found) {
		Json::FastWriter writer;
		return writer.write(dest);
	}

	return value;
}

std::string Message::get(const std::string& k, const std::string& default_value) const
{
	Json::Value dest;
	bool found = find(k, dest);
	return (found && dest.isConvertibleTo(Json::stringValue)
			? dest.asString() : default_value);
}

std::string Message::get(const std::string& k, const char* default_value) const
{
	Json::Value dest;
	bool found = find(k, dest);
	return (found && dest.isConvertibleTo(Json::stringValue)
			? dest.asString() : default_value);
}

int Message::get(const std::string& k, int default_value) const
{
	Json::Value dest;
	bool found = find(k, dest);
	return (found && dest.isNumeric()
			? dest.asInt() : default_value);
}

double Message::get(const std::string& k, double default_value) const
{
	Json::Value dest;
	bool found = find(k, dest);
	return (found && dest.isDouble()
			? dest.asDouble() : default_value);
}

long long Message::get(const std::string& k, long long default_value) const
{
	Json::Value dest;
	bool found = find(k, dest);
	return (found && dest.isIntegral()
			? dest.asLargestInt() : default_value);
}

bool Message::get(const std::string& k, bool default_value) const
{
	Json::Value dest;
	bool found = find(k, dest);
	return (found && dest.isConvertibleTo(Json::booleanValue)
			? dest.asBool() : default_value);
}

Message::Array_t Message::get(const std::string& k,
							  Message::Array_t default_value) const
{
	if(is_array(k)) {
		Json::Value dest;
		find(k, dest);
		Message::Array_t out(dest);
		return out;
	}
	return Message::Array_t();
}

void Message::set_object(const std::string& k)
{
	set(k, Object_t().get_value());
}

void Message::set_array(const std::string& k)
{
	set(k, Array_t().get_value());
}

void Message::set_null(const std::string& k)
{
	set(k, Null_t().get_value());
}

void Message::set_deserialized(const std::string& k,
							   const std::string& value)
{
	Json::Value deserialized;
	Json::Reader reader;
	if(!reader.parse(value, deserialized, false)) {
		LOG(DEBUG) << "Cannot parse value: " << value;
		throw std::runtime_error(reader.getFormattedErrorMessages());
	}

	set(k, deserialized);
}


void Message::set_status_code(nix::StatusCode_t status)
{
	root_["@status_code"] = status;
}

void Message::set_status_msg(const std::string& msg)
{
	root_["@status"] = msg;
}

void Message::set_status(nix::StatusCode_t status,
						 const std::string& msg)
{
	set_status_code(status);
	set_status_msg(msg);
}

void Message::append_null(const std::string& k)
{
	append(k, Null_t().get_value());
}

void Message::remove(const std::string& k)
{
	std::vector<std::string> keys;
	nix::util::string::split(k, keys, ".");

	Json::Value* ptr = &root_;
	for(auto it = keys.begin(); it != keys.end() - 1; it++) {
		if(!ptr->isMember(*it)) {
			(*ptr)[*it] = Json::objectValue;
		}
		ptr = &((*ptr)[*it]);
	}

	std::string last = keys.back();
	ptr->removeMember(last);
}

bool Message::is_null(const std::string& k) const
{
	Json::Value dest;
	bool found = find(k, dest);
	return (!found || dest.isNull());
}

bool Message::is_array(const std::string& k) const
{
	Json::Value dest;
	bool found = find(k, dest);
	return (found && dest.isArray());
}

bool Message::is_object(const std::string& k) const
{
	Json::Value dest;
	bool found = find(k, dest);
	return (found && dest.isObject());
}

std::string Message::to_string(const std::string& k, bool pretty) const
{
	if(k.length()) {
		Json::Value dest;
		bool found = find(k, dest);
		
		if(found && pretty) {
			return dest.toStyledString();
		}
		Json::FastWriter writer;
		return writer.write((found ? dest : Null_t().get_value()));
	}

	if(pretty) {
		return root_.toStyledString();
	}

	Json::FastWriter writer;
	return writer.write(root_);
}

bool Message::exists(const std::string& k) const
{
	Json::Value dummy;
	return find(k, dummy);
}

bool Message::find(const std::string& k, Json::Value& dest) const
{
	std::vector<std::string> keys;
	nix::util::string::split(k, keys, ".");
	if(root_.isArray()) {
		return false;
	}
	const Json::Value* ptr = &root_;
	for(auto& it : keys) {
		if(ptr->isArray() || !ptr->isMember(it)) {
			return false;
		}
		ptr = &((*ptr)[it]);
	}
	dest = *ptr;
	return true;
}


} // nix
