#ifndef TEST_UTIL_HXX
#define TEST_UTIL_HXX

#include <functional>
#include <iostream>
#include <string>
#include <map>


class TestCase
{
public:
	typedef std::function<void(TestCase&)> TestFunction_t;

	TestCase() = delete;
	TestCase(const std::string& unit_name)
		: unit_name_(unit_name)
	{
	}

	virtual ~TestCase() = default;

	template <class T>
	bool equals(const T& result,
				const T& expected,
				const std::string& name = "equals")
	{
		bool ok = (result == expected);
		if(!ok) {
			print_diff(result, expected, name);
		}
		store_result(ok, name);
		return ok;
	}

	template <class T>
	bool equals(const std::string& result,
				const std::string& expected,
				const std::string& name = "equals")
	{
		int different = (result.compare(expected));
		if(different) {
			print_diff(result, expected, name);
		}
		store_result(!different, name);
		return !different;
	}

	size_t failures_count() const
	{
		size_t failures = 0;
		for(auto& it : results_) {
			if(!it.second) {
				failures++;
			}
		}
		return failures;
	}

protected:
	template<class T>
	void  print_diff(const T& result,
					 const T& expected,
					 const std::string& name)
	{
		std::cout << "\n\t" << (name.empty() ? "..." : name)
				  << "\n\tRESULT: " << result
				  << "\n\tEXPECT: " << expected
				  << std::endl;
	}

	void store_result(bool passed, const std::string& name = std::string())
	{
		static int result_id = 0;
		result_id++;
		std::string test_name(name.empty() ? unit_name_ + "test" : name);
		test_name.append("_" + std::to_string(result_id));
		results_[test_name] = passed;
	}

	std::map<std::string, int> results_;
	std::string unit_name_;
};



class UnitTest
{
public:
	void test_case(const std::string& name,
				   TestCase::TestFunction_t code)
	{
		cases_[name] = code;
	}

	int run()
	{
		int i = 0;
		for(auto& it : cases_) {
			try	{
				i++;
				std::cout << i << " - " << it.first << " - ";

				TestCase tcase(it.first);
				it.second(tcase);

				size_t failures = tcase.failures_count();
				if(failures) {
					failed_.push_back(it.first);
				}
				
				std::cout << (failures ? "FAIL" : "PASS") << std::endl;
			}
			catch(std::exception& e) {
				std::cout << "FAIL: " << it.first << "\n"
						  << "\t" << e.what() << std::endl;
			}
		}

		return failed_.size();
	}

protected:
	std::map<std::string, TestCase::TestFunction_t> cases_;
	std::vector<std::string> failed_;
};

#endif
