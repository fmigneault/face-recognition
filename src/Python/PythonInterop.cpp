#ifdef FACE_RECOG_HAS_PYTHON

#include "FaceRecog.h"

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>

// Python
#include <Python.h>
#include <Python/PythonInterop.h>
#include <Python/PyCvBoostConverter.h>
#include <numpy/ndarrayobject.h>
#include <abstract.h>

// Boost
#include <boost/python.hpp>
#include <boost/filesystem.hpp>
#include <boost/exception/all.hpp>
namespace bp = boost::python;

// STD
#include <iostream>

//All these function needs to be wrapped around a Py_init and Py_Finit to work
//This needs to be done on the caller layer

PyObject* callPyUsingModule(std::string funcName, std::string filePath, PyObject* arg)
{
    #if PY_MAJOR_VERSION >= 3
    PyObject* myModuleString = PyBytes_FromString(filePath.c_str());
    #else
    PyObject* myModuleString = PyString_FromString(filePath.c_str());
    #endif
    PyObject* myModule = PyImport_Import(myModuleString);

    PyObject* myFunction = PyObject_GetAttrString(myModule, funcName.c_str());
    PyObject* returnVal = PyObject_CallFunction(myFunction, "O", arg);
    Py_DECREF(myModuleString);
    Py_DECREF(myModule);
    Py_DECREF(myFunction);
    return returnVal;

}

bp::object callPyFuncFromFile(std::string funcName, std::string filePath, std::vector<bp::object>& args)
{
    //Only handle 1 arg for now
    bp::object main = bp::import("__main__");
    bp::object global(main.attr("__dict__"));
    if (!boost::filesystem::exists(filePath))
    {
        std::cout << "No file found" << std::endl;
        return bp::object();
    }
    try
    {
        bp::object pyFile = bp::exec_file(filePath.c_str(), global, global);
    }
    catch (boost::exception &e)
    {
        std::cerr << boost::diagnostic_information(e);
    }
    bp::object pyFunc = global[funcName];
    if (!pyFunc.is_none())
    {
        bp::object ret = pyFunc(args[0]);
        return ret;
    }
    else
        return bp::object();
}

#endif/*FACE_RECOG_HAS_PYTHON*/
