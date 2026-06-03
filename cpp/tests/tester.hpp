#ifndef SUBLEQ_TESTER_HPP
#define SUBLEQ_TESTER_HPP

#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace tester {

    template <typename T>
    bool run(const std::string& suite_name, const json& test_data, std::function<bool(const std::string&, const json&)> test_func) {
        bool success = true;
        int count = 0;

        for (auto it = test_data.begin(); it != test_data.end(); ++it) {
            count++;
            std::string name = it.key();
            try {
                bool ok = test_func(name, it.value());
                if (!ok) {
                    success = false;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error in test " << name << ": " << e.what() << std::endl;
                success = false;
            }
        }

        if (count == 0) {
            std::cout << suite_name << ": NO TESTS FOUND" << std::endl;
            return false;
        }

        if (success) {
            std::cout << suite_name << ": PASS" << std::endl;
        } else {
            std::cout << suite_name << ": FAIL" << std::endl;
            exit(1);
        }

        return success;
    }

} // namespace tester

#endif
