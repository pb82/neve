#include "printer.hpp"

namespace JSON {

void Printer::dispatchType(const Value &val, std::ostringstream &out) {
	switch (val.getType()) {
	case JSON_OBJECT:
		printObject(val, out);
		break;
	case JSON_ARRAY:
		printArray(val, out);
		break;
	case JSON_NUMBER:
		printNumber(val, out);
		break;
	case JSON_BOOL:
		printBoolean(val, out);
		break;
	case JSON_STRING:
		printString(val, out);
		break;
	case JSON_NULL:
		out << "null";
		break;
	}
}

void Printer::printObject(const Value &val, std::ostringstream &out) {
	bool firstLine = true;
	out << "{";
	for (auto pair : val.as<Object>()) {
		if (!firstLine) { out << ","; }
		out << "\"";
		out << pair.first;
		out << "\"";
		out << ":";
		dispatchType(pair.second, out);
		firstLine = false;
	}
	out << "}";
}

void Printer::printArray(const Value &val, std::ostringstream &out) {
	bool firstItem = true;
	out << "[";
	for (auto item : val.as<Array>()) {
		if (!firstItem) { out << ","; }
		dispatchType(item, out);
		firstItem = false;
	}
	out << "]";
}

void Printer::printNumber(const Value &val, std::ostringstream &out) {
	out << val.as<double>();
}

void Printer::printBoolean(const Value &val, std::ostringstream &out) {
	out << (val.as<bool>() ? "true" : "false");
}

void Printer::printString(const Value &val, std::ostringstream &out) {
	out << "\"";
	out << val.as<std::string>();
	out << "\"";
}

void Printer::print(const Value val, std::ostringstream &out) {
	dispatchType(val, out);
}

std::string Printer::print(const Value val) {
	std::ostringstream out;
	dispatchType(val, out);
	return out.str();
}

} // End namespace JSON
