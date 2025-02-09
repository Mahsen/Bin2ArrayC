#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <stdexcept>

std::vector<uint8_t> parseIntelHexFile(const std::string &fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << fileName << std::endl;
        throw std::runtime_error("File not found");
    }

    std::vector<uint8_t> binaryData;

    std::string line;
    while (std::getline(file, line)) {
        if (line[0] != ':') {
            std::cerr << "Invalid start of line in HEX file: " << line << std::endl;
            continue;  // Skip lines that don't start with ':'
        }

        // Line length: 9 characters + data
        size_t byteCount = std::stoi(line.substr(1, 2), nullptr, 16);
        size_t address = std::stoi(line.substr(3, 4), nullptr, 16);
        std::string dataStr = line.substr(9, byteCount * 2);  // Data starts at position 9
        // Validate checksum (optional)
        // (Checksum validation can be added here if needed)
        
        for (size_t i = 0; i < byteCount * 2; i += 2) {
            std::string byteStr = dataStr.substr(i, 2);
            try {
                uint8_t byte = std::stoi(byteStr, nullptr, 16);
                binaryData.push_back(byte);
            } catch (const std::invalid_argument &e) {
                std::cerr << "Invalid hex value: " << byteStr << " at position " << i << std::endl;
                continue;
            }
        }
    }

    return binaryData;
}

void convertHexToCArray(const std::string &fileName, const std::string &outputFileName) {
    std::vector<uint8_t> binaryData;
    try {
        binaryData = parseIntelHexFile(fileName);
    } catch (const std::exception &e) {
        std::cerr << "Error processing the hex file: " << e.what() << std::endl;
        return;
    }

    std::ofstream outputFile(outputFileName);
    if (!outputFile.is_open()) {
        std::cerr << "Could not open output file: " << outputFileName << std::endl;
        return;
    }

    outputFile << "// Program.bin\r\nconst unsigned char mainData[] = {";
    for (size_t i = 0; i < binaryData.size(); ++i) {
        outputFile << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)binaryData[i];
        if (i != binaryData.size() - 1) {
            outputFile << ", ";
        }
    }
    outputFile << "};" << std::endl;

    std::cout << "Output written to " << outputFileName << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_hex_file> <output_txt_file>" << std::endl;
        return 1;
    }

    std::string hexFile = argv[1];
    std::string outputFile = argv[2];

    convertHexToCArray(hexFile, outputFile);
    return 0;
}

