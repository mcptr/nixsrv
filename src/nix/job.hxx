#ifndef NIX_JOB_HXX
#define NIX_JOB_HXX

#include <string>

namespace nix {


class Job
{
public:
	explicit Job(const std::string& action,
				 const std::string& serialized_parameters = std::string());

	inline const std::string& get_id() const
	{
		return id_;
	}

	inline int ctime() const
	{
		return ctime_;
	}

	inline const std::string& get_parameters() const
	{
		return parameters_;
	}

	void set_progress(double progress);
	double get_progress() const;
	const std::string& get_action() const;

protected:
	const std::string action_;
	const std::string parameters_;
	std::string id_;
	int ctime_;
	double progress_ = 0.0;
};


} // nix

#endif
