#ifndef FACE_RECOG_PYTHON_INTEROP_H
#define FACE_RECOG_PYTHON_INTEROP_H

#include <boost/python.hpp>

namespace bp = boost::python;

PyObject* callPyUsingModule(std::string funcName, std::string filePath, PyObject* arg);
bp::object callPyFuncFromFile(std::string funcName, std::string filePath, std::vector<bp::object>& args);

#endif/*FACE_RECOG_PYTHON_INTEROP_H*/
