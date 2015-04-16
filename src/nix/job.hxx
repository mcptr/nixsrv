#ifndef NIX_JOB_HXX
#define NIX_JOB_HXX

#include <string>
#include "nix/message.hxx"


namespace nix {


class Job
{
public:
	Job() = default;
	explicit Job(const nix::Message& msg);
	virtual ~Job() = default;

	virtual inline const std::string& get_id() const final
	{
		return id_; 
	}

	virtual inline const std::string& get_module() const final
	{
		return module_;
	}

	virtual inline const std::string& get_action() const final
	{
		return action_;
	}

	virtual inline const std::string& get_api_key() const final
	{
		return api_key_;
	}

	virtual inline const std::string& get_origin_node() const final
	{
		return origin_node_;
	}

	virtual inline int get_ctime() const final
	{
		return ctime_;
	}

	virtual inline double get_progress() const final
	{
		return progress_;
	}

	virtual inline Message& parameters() final
	{
		return parameters_;
	}

	virtual void set_origin_node(const std::string& nodename);
	virtual std::string to_string() const;
	virtual void deserialize(const Message& msg);

	operator std::string() const
	{
		return to_string();
	}
protected:
	std::string id_ = "";
	std::string origin_node_ = "";
	std::string module_ = "";
	std::string action_ = "";
	std::string api_key_ = "";
	double progress_ = 0.0;
	int ctime_ = 0;
	bool completed_ = false;
	Message parameters_;

};


} // nix

#endif
