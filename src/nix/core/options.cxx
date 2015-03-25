#include <iostream>
#include <iomanip>

#include "options.hxx"

namespace nix {
namespace core {

void Options::dump_variables_map() const
{
	for (const auto& it : vm_) {
		std::cout << ": "  << std::setw(48) << std::left << it.first;
		std::cout.width(0);
		auto& raw = it.second.value();
		std::string v;
		if (auto v = boost::any_cast<int>(&raw)) {
			std::cout << *v;
		}
		else if (auto v = boost::any_cast<std::string>(&raw)) {
			std::cout << *v;
		}
		else if (auto v = boost::any_cast<bool>(&raw)) {
			std::cout << *v;
		}

		std::cout << std::endl;
	}
}

} // core
} // nix
