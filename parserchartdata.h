#ifndef PARSERCHARTDATA_H
#define PARSERCHARTDATA_H

class ParserChartData
{
public:
    ParserChartData();

    double minPrice = 0.0;
    double maxPrice = 0.0;
    double latestRate = 0.0;
    bool isValid = false;
};

#endif // PARSERCHARTDATA_H
