#include "license.hpp"
#include <fstream>
#include <iostream>
#include <vector>

namespace guardian::license {

bool License::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Licensee:") != std::string::npos) {
            customer = line.substr(line.find(":") + 2);
        } else if (line.find("Email:") != std::string::npos) {
            email = line.substr(line.find(":") + 2);
        } else if (line.find("Date:") != std::string::npos) {
            date = line.substr(line.find(":") + 2);
        } else if (line.find("License ID:") != std::string::npos) {
            license_id = line.substr(line.find(":") + 2);
        }
    }
    
    valid = true;
    return true;
}

bool License::isValid() const {
    return valid && !customer.empty() && !license_id.empty();
}

void License::print() const {
    std::cout << "🔷 The Guardian — Commercial License\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "  Licensee:   " << customer << "\n";
    std::cout << "  Email:      " << email << "\n";
    std::cout << "  Date:       " << date << "\n";
    std::cout << "  License ID: " << license_id << "\n";
    std::cout << "  Status:     " << (isValid() ? "✅ VALID" : "❌ INVALID") << "\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
}

bool hasCommercialLicense() {
    License license;
    
    std::vector<std::string> paths = {
        "commercial_license.txt",
        "../commercial_license.txt",
        "/etc/guardian/commercial_license.txt",
        "license/commercial_license.txt"
    };
    
    for (const auto& path : paths) {
        if (license.loadFromFile(path)) {
            return license.isValid();
        }
    }
    
    return false;
}

void showLicenseStatus() {
    License license;
    bool found = false;
    
    std::vector<std::string> paths = {
        "commercial_license.txt",
        "../commercial_license.txt",
        "/etc/guardian/commercial_license.txt",
        "license/commercial_license.txt"
    };
    
    for (const auto& path : paths) {
        if (license.loadFromFile(path)) {
            found = true;
            break;
        }
    }
    
    if (found && license.isValid()) {
        std::cout << "\n✅ The Guardian is licensed for commercial use!\n";
        license.print();
    } else if (found) {
        std::cout << "\n❌ Invalid commercial license.\n";
        std::cout << "   Please contact support.\n";
    } else {
        std::cout << "\n🔷 The Guardian — Free Edition\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "  This is the free version of The Guardian.\n";
        std::cout << "  For commercial use, please purchase a license:\n";
        std::cout << "  https://buy.polar.sh/polar_cl_czZ7LW2XcJR1zNjUoiGbBYU4MNOI4ShcvcR6l00HLU9\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    }
}

} // namespace guardian::license
