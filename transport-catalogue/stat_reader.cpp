#include "stat_reader.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

namespace tc::printer
{

    std::pair<std::string_view, std::string_view>
    ParseQueryType(const std::string_view line)
    {
        using namespace std::string_view_literals;
        auto space_pos = line.find(' ');
        auto data_begin = line.find_first_not_of(' ', space_pos);

        if (space_pos == std::string::npos || data_begin == std::string::npos)
        {
            return {};
        }

        auto query_type = line.substr(0, space_pos);
        auto data = line.substr(data_begin);

        return {query_type, data};
    }

    void PrintBusInfo(const TransportCatalogue &cat, std::string_view bus_name)
    {
        using namespace std::string_view_literals;

        Bus *bus = cat.GetBus(bus_name);
        std::cout << "Bus "sv << bus_name << ": "sv;

        if (bus == nullptr)
        {
            std::cout << "not found"sv << std::endl;
        }
        else
        {
            auto info = cat.GetBusInfo(bus);
            std::cout << info.total_stops << " stops on route, "sv
                      << info.unique_stops << " unique stops, "sv
                      << std::setprecision(6)
                      << info.fact_route_length << " route length, "sv
                      << info.fact_route_length / info.line_route_length
                      << " curvature"sv << std::endl;
        }
    }

    void PrintStopInfo(const TransportCatalogue &cat, std::string_view stop_name)
    {
        using namespace std::string_view_literals;

        std::cout << "Stop "sv << stop_name << ": "sv;

        if (cat.GetStop(stop_name) == nullptr)
        {
            std::cout << "not found"sv << std::endl;
            return;
        }

        const auto &buses = cat.GetBusesByStop(stop_name);
        if (buses.empty())
        {
            std::cout << "no buses"sv << std::endl;
            return;
        }

        std::cout << "buses"sv;
        for (const auto &bus : buses)
        {
            std::cout << " "sv << bus->name;
        }
        std::cout << std::endl;
    }

    void ParseAndPrint(const TransportCatalogue &cat, std::string_view line)
    {
        using namespace std::string_view_literals;
        auto [query_type, query_data] = ParseQueryType(line);

        // Пропуск некорректных запросов (отсутствует тип или имя запроса)
        if (query_type.empty() || query_data.empty())
        {
            return;
        }

        if (query_type == "Bus"sv)
        {
            PrintBusInfo(cat, query_data);
        }
        else if (query_type == "Stop"sv)
        {
            PrintStopInfo(cat, query_data);
        }
    }

    void ProcessQueries(const TransportCatalogue &cat,
                        std::vector<std::string> &data)
    {
        for (const std::string_view line : data)
        {
            ParseAndPrint(cat, line);
        }
    }

} // namespace tc::printer
