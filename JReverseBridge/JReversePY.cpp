#include "boost/python.hpp"

char const* testpy()
{
	return "Python scripts are working";
}

using namespace boost::python;

BOOST_PYTHON_MODULE(JReversePY)
{
	def("testpy", testpy);
}
