#pragma once
#include <string>
#include <vector>

namespace guardian::license {

struct License {
    std::string customer;
    std::string email;
    std::string date;
    std::string license_id;
    bool valid;
    
    License() : valid(false) {}
    
    bool loadFromFile(const std::string& path);
    bool isValid() const;
    void print() const;
};

bool hasCommercialLicense();
void showLicenseStatus();

} // namespace guardian::license
