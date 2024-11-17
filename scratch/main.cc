#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#define FILE_NAME "./data/sample.csv"

int
main()
{
    std::vector<std::vector<int> > data;
    std::ifstream file(FILE_NAME);
    std::string line;

    if (!file.is_open())
    {
        std::cerr << "Unable to open file " << FILE_NAME << std::endl;
        return 1;
    }

    while (getline(file, line))
    {
        std::stringstream lineStream(line);
        std::string cell;
        std::vector<int> row;

        while (getline(lineStream, cell, ','))
        {
            row.push_back(std::stoi(cell));
        }

        data.push_back(row);
    }

    for (size_t i = 0; i < data.size(); ++i)
    {
        for (size_t j = 0; j < data[i].size(); ++j)
        {
            std::cout << "data[" << i << "][" << j << "] = " << data[i][j] << std::endl;
        }
    }

    file.close();
    return 0;
}
