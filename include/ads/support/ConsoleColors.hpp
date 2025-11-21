//===--------------------------------------------------------------------------===//
/**
 * @file ConsoleColors.hpp
 * @author 
 * @brief Shared ANSI color palette for demos and debugging output.
 */
//===--------------------------------------------------------------------------===//

#pragma once

// Using macros keeps compatibility with existing demo sources that already rely on
// short color tokens. They are guarded to avoid redefinition warnings.

#ifndef RESET
#define RESET "\033[0m"
#endif

#ifndef RED
#define RED "\033[31m"
#endif

#ifndef GREEN
#define GREEN "\033[32m"
#endif

#ifndef YELLOW
#define YELLOW "\033[33m"
#endif

#ifndef BLUE
#define BLUE "\033[34m"
#endif

#ifndef MAGENTA
#define MAGENTA "\033[35m"
#endif

#ifndef CYAN
#define CYAN "\033[36m"
#endif

#ifndef BOLD
#define BOLD "\033[1m"
#endif
