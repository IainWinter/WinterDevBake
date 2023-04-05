#pragma once

#include <ostream>
#include "PageBakeArguments.h"

int GeneratePage(std::ostream& out, const std::string& text, const PageBakeArguments& args);