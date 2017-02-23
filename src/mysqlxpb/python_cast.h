/*
 * MySQL Connector/Python - MySQL driver written in Python.
 * Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
 *
 * MySQL Connector/Python is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef MYSQLXPB_PYTHON_CAST_H
#define MYSQLXPB_PYTHON_CAST_H

#include "python.h"
#include "mysqlx/mysqlx.pb.h"
#include <stdexcept>
#include <string>


class BadPythonCast : std::runtime_error {
public:
  BadPythonCast(const char* message) :
    std::runtime_error(message) {
  }
};


#define THROW_BAD_PYTHON_CAST(TYPE_NAME) \
  { throw BadPythonCast("Bad Python cast: " #TYPE_NAME); }


template<typename T>
T common_int_cast(PyObject* obj)
{
  if (PyLong_CheckExact(obj))
    return static_cast<T>(PyLong_AsLong(obj));

#ifndef PY3
  if (PyInt_CheckExact(obj))
    return static_cast<T>(PyInt_AsLong(obj));
#endif

  THROW_BAD_PYTHON_CAST(long);
}


template<typename T>
T python_cast(PyObject* obj);


template<>
double python_cast<double>(PyObject* obj) {
  if (!PyFloat_CheckExact(obj))
    THROW_BAD_PYTHON_CAST(double);
  return PyFloat_AsDouble(obj);
}


template<>
float python_cast<float>(PyObject* obj) {
  if (!PyFloat_CheckExact(obj))
    THROW_BAD_PYTHON_CAST(float);
  return static_cast<float>(PyFloat_AsDouble(obj));
}


template<>
google::protobuf::int32 python_cast<google::protobuf::int32>(
    PyObject* obj) {
  return common_int_cast<google::protobuf::int32>(obj);
}


template<>
google::protobuf::int64 python_cast<google::protobuf::int64>(
    PyObject* obj) {
  return common_int_cast<google::protobuf::int64>(obj);
}


template<>
google::protobuf::uint32 python_cast<google::protobuf::uint32>(
    PyObject* obj) {
  return common_int_cast<google::protobuf::uint32>(obj);
}


template<>
google::protobuf::uint64 python_cast<google::protobuf::uint64>(
    PyObject* obj) {
  return common_int_cast<google::protobuf::uint64>(obj);
}


template<>
bool python_cast<bool>(PyObject* obj) {
  if (!PyBool_Check(obj))
    THROW_BAD_PYTHON_CAST(bool);
  return obj == Py_True;
}


template<>
std::string python_cast<std::string>(PyObject* obj) {
  if (PyString_CheckExact(obj)) {
    return std::string(PyString_AsString(obj), PyString_Size(obj));
  }
#ifdef PY3
  else if (PyUnicode_CheckExact(obj)) {
    Py_ssize_t len;
    char* str = PyUnicode_AsUTF8AndSize(obj, &len);
    return std::string(str, len);
  } else if (PyBytes_CheckExact(obj)) {
    return std::string(PyBytes_AsString(obj), PyBytes_Size(obj));
  }
#endif

  THROW_BAD_PYTHON_CAST(std::string);
}


#undef THROW_INVALID_PYTHON_CAST


#endif // MYSQLXPB_PYTHON_CAST_H