#ifndef PRINTER_H
#define PRINTER_H

#include <sstream>
#include "value.hpp"

namespace JSON {
    class Printer {
    public:		
        void print(const Value val, std::ostringstream& out);
        std::string print(const Value val);

        virtual ~Printer() { }

    protected:
        void dispatchType(const Value &val, std::ostringstream &out);
        void printNumber(const Value &val, std::ostringstream &out);
        void printBoolean(const Value &val, std::ostringstream &out);
        void printString(const Value &val, std::ostringstream &out);

        /**
          * Allow to overwrite the formatting of Object and
          * Array for Pretty Printing.
          */
        virtual void printObject(const Value &val, std::ostringstream &out);
        virtual void printArray(const Value &val, std::ostringstream &out);
    };
}

#endif // PRINTER_H
