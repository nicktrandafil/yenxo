// boost
#include <boost/hana.hpp>

// std
#include <iostream>


class VariantErr : public std::runtime_error {
    explicit VariantErr(std::string const& x) : runtime_error(x) {}
};


class List {

};


struct Rule {
    int id;
    std::string name;
    std::string description;
};


int main() {
}
