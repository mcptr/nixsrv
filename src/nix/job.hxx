#ifndef NIX_JOB_HXX
#define NIX_JOB_HXX

#include <string>


namespace nix {


class Job
{
public:
	Job();
	explicit Job(const std::string& id);

	inline const std::string& get_id() const
	{
		return id_;
	}

protected:
	const std::string id_;
};


} // nix

#endif
