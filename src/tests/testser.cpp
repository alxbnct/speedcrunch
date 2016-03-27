// This file is part of the SpeedCrunch project
// Copyright (C) 2015-2016 Hadrien Theveneau <theveneau@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include "math/hmath.h"
#include "math/cmath.h"
#include "math/units.h"
#include "math/quantity.h"
#include "tests/testcommon.h"

#include <cstdlib>
#include <string>
#include <iostream>
#include <QJsonDocument>

using namespace std;

#define CHECK_SER(x,y) check_ser(__FILE__, __LINE__, #x, x, y)
#define CHECK_DESER_HNUMBER(x,y) check_deser_hnumber(__FILE__, __LINE__, #x, x, y)
#define CHECK_DESER_CNUMBER(x,y) check_deser_cnumber(__FILE__, __LINE__, #x, x, y)
#define CHECK_DESER_QUANTITY(x,y) check_deser_quantity(__FILE__, __LINE__, #x, x, y)
#define CHECK_SER_DESER_HNUMBER(x,y) check_ser_deser_hnumber(__FILE__, __LINE__, #x, x, y)
#define CHECK_SER_DESER_CNUMBER(x,y) check_ser_deser_cnumber(__FILE__, __LINE__, #x, x, y)

static int ser_total_tests = 0;
static int ser_failed_tests = 0;
static int ser_new_failed_tests = 0;

template <class T>
void check_ser(const char* file, int line, const char* msg, const T& num, const char* expected, int issue = 0) {
    /* Serialization */
    QJsonObject obj;
    num.serialize(obj);
    QJsonDocument doc(obj);
    string result = doc.toJson(QJsonDocument::Compact).toStdString();
    /* Test result and display info */
    ++ser_total_tests;
    DisplayErrorOnMismatch(file, line, msg, result, expected, ser_failed_tests, ser_new_failed_tests, issue);
}

void check_deser_hnumber(const char* file, int line, const char* msg, const char* str,  const char* expected, int issue = 0) {
    /* Deserialization */
    QJsonDocument doc = QJsonDocument::fromJson(str);
    HNumber num(doc.object());
    /* Test result and display info */
    ++ser_total_tests;
    string result = HMath::format(num, 'f').toStdString();
    DisplayErrorOnMismatch(file, line, msg, result, expected, ser_failed_tests, ser_new_failed_tests, issue);
}

void check_deser_cnumber(const char* file, int line, const char* msg, const char* str,  const char* expected, int issue = 0) {
    /* Deserialization */
    QJsonDocument doc = QJsonDocument::fromJson(str);
    CNumber num(doc.object());
    /* Test result and display info */
    ++ser_total_tests;
    string result = CMath::format(num, 'f').toStdString();
    DisplayErrorOnMismatch(file, line, msg, result, expected, ser_failed_tests, ser_new_failed_tests, issue);
}

void check_deser_quantity(const char* file, int line, const char* msg, const char* str,  const char* expected, int issue = 0) {
    /* Deserialization */
    QJsonDocument doc = QJsonDocument::fromJson(str);
    Quantity q(doc.object());
    /* Test result and display info */
    ++ser_total_tests;
    string result = DMath::format(q, 'f').toStdString();
    DisplayErrorOnMismatch(file, line, msg, result, expected, ser_failed_tests, ser_new_failed_tests, issue);
}

void check_ser_deser_hnumber(const char* file, int line, const char* msg, const HNumber& src, const char* expected, int issue = 0) {
    /* Serialization + deserialization */
    QJsonObject obj;
    src.serialize(obj);
    HNumber dest = HNumber::deSerialize(obj);
    /* Test result and display info */
    string result = HMath::format(dest, 'g', 50).toStdString();
    ++ser_total_tests;
    DisplayErrorOnMismatch(file, line, msg, result, expected, ser_failed_tests, ser_new_failed_tests, issue);
}

void check_ser_deser_cnumber(const char* file, int line, const char* msg, const CNumber& src, const char* expected, int issue = 0) {
    /* Serialization + deserialization */
    QJsonObject obj;
    src.serialize(obj);
    CNumber dest = CNumber::deSerialize(obj);
    /* Test result and display info */
    string result = CMath::format(dest, 'g', 50).toStdString();
    DisplayErrorOnMismatch(file, line, msg, result, expected, ser_failed_tests, ser_new_failed_tests, issue);
}

int main(int, char**)
{
    /* Serialization tests */
    CHECK_SER(HNumber("3"), "{\"format\":\"NULL\",\"value\":\"3.000000000000000000000000000000000000000000000000000000000000000000000000000000\"}");

    CHECK_SER(CNumber("3"), "{\"format\":\"NULL\",\"value\":\"3.000000000000000000000000000000000000000000000000000000000000000000000000000000\"}");
    CHECK_SER(CNumber("3+1j"), "{\"format\":\"NULL\",\"value\":\"3.000000000000000000000000000000000000000000000000000000000000000000000000000000+1.000000000000000000000000000000000000000000000000000000000000000000000000000000j\"}");

    Quantity a(CNumber("3"));
    a.setFormat('b');
    a *= Units::meter();
    a.setDisplayUnit(CNumber("0.3"), QString("foot"));
    const char q_json_blob[] = "{\"dimension\":{\"length\":\"1\"},\"format\":\"\",\"numeric_value\":{\"format\":\"NULL\",\"value\":\"3.000000000000000000000000000000000000000000000000000000000000000000000000000000\"},\"unit\":{\"format\":\"NULL\",\"value\":\"0.300000000000000000000000000000000000000000000000000000000000000000000000000000\"},\"unit_name\":\"foot\"}";
    CHECK_SER(a, q_json_blob);

    /* HNumber deserialization tests */
    CHECK_DESER_HNUMBER("{\"format\": \"g\",\"value\": \"1\"}", "1");
    CHECK_DESER_HNUMBER("{\"format\": \"g\",""\"value\": \"0.1\"}", "0.1");
    /* CNumber deserialization tests */
    CHECK_DESER_CNUMBER("{\"format\": \"g\",\"value\": \"1\"}", "1");
    CHECK_DESER_CNUMBER("{\"format\": \"g\",\"value\": \"0.1\"}", "0.1");
    CHECK_DESER_CNUMBER("{\"format\": \"g\",\"value\": \"0.0+1.0j\"}", "1j");
    /* Quantity deserialization tests */
    CHECK_DESER_QUANTITY(q_json_blob, "10 foot");

    /* Serialization + deserialization tests */
    CHECK_SER_DESER_HNUMBER(HNumber("3"), "3.00000000000000000000000000000000000000000000000000");
    CHECK_SER_DESER_CNUMBER(CNumber("3"), "3.00000000000000000000000000000000000000000000000000");
    CHECK_SER_DESER_CNUMBER(CNumber("3+1j"), "3.00000000000000000000000000000000000000000000000000+1.00000000000000000000000000000000000000000000000000j");
    cout << ser_total_tests  << " total, "
         << ser_failed_tests << " failed, "
         << ser_new_failed_tests << " new" << endl;
}

