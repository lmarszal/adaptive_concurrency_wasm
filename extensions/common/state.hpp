#pragma once

#include <string>

std::string getExpAvgState();
void setExpAvgState(std::string state);

void ensureLimit();
uint32_t getLimit();
void setLimit(uint32_t limit);

void ensureInflight();
int32_t incrementInflight();
int32_t decrementInflight();
