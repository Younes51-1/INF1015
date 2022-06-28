#pragma once
// Bugfix pour CODE_COVERAGE_FILTER de Visual Studio; voir https://developercommunity.visualstudio.com/content/problem/189844/compilation-error-in-codecoveragecodecoverageh-wit.html qui est actuellement "Closed - Lower Priority" alors on a notre propre bugfix.
// par Francois-R.Boyer@PolyMtl.ca

#ifdef _MSC_VER
# if __has_include(<CodeCoverage/CodeCoverage.h>)
#  include <CodeCoverage/CodeCoverage.h>
// Voir CODE_COVERAGE_FILTER dans le .h ci-dessus, il y a une erreur, il retourne un pointeur non const vers un littéral "...".
#  ifndef CODE_COVERAGE_DISABLE_ALL_FILTERS
#   undef CODE_COVERAGE_FILTER
// Copy&paste from CodeCoverage.h, only "const" has been added to the return type before CODE_COVERAGE_FILTER_NAME():
#   define CODE_COVERAGE_FILTER(CODE_COVERAGE_FILTER_CATEGORY, CODE_COVERAGE_FILTER_NAME, CODE_COVERAGE_FILTER_PATTERN) \
    namespace Microsoft{ namespace VisualStudio{ namespace CppCodeCoverageFramework{ namespace CODE_COVERAGE_FILTER_CATEGORY{ \
        __pragma(optimize("", off)) \
        __declspec(dllexport) __declspec(noinline) const void* CODE_COVERAGE_FILTER_NAME() \
        { \
            __annotation( \
                CODE_COVERAGE_CONCAT2(L, CODE_COVERAGE_TOSTRING2(CODE_COVERAGE_FILTER_NAME)) \
                L" " \
                CODE_COVERAGE_FILTER_PATTERN); \
            return CODE_COVERAGE_TOSTRING2(CODE_COVERAGE_FILTER_NAME); \
        } \
        __pragma(optimize("", on)) \
    }}}}
#  endif
# else
#  pragma message(__FILE__ ": warning : Il vous manque le fichier CodeCoverage.h de Visual Studio Enterprise ou Preview, avez vous la version Community?")
# endif
#endif
