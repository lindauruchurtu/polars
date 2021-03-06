
# Set source directory
set(CPP_SOURCE_DIR "src/cpp/polars")

# Build sources list
set(
        CPP_SOURCES
        "${CPP_SOURCE_DIR}/numc.h"
        "${CPP_SOURCE_DIR}/numc.cpp"
        "${CPP_SOURCE_DIR}/Series.cpp"
        "${CPP_SOURCE_DIR}/Series.h"
        "${CPP_SOURCE_DIR}/TimeSeries.h"
        "${CPP_SOURCE_DIR}/TimeSeriesMask.h"
        "${CPP_SOURCE_DIR}/SeriesMask.cpp"
        "${CPP_SOURCE_DIR}/SeriesMask.h"
        "${CPP_SOURCE_DIR}/WindowProcessor.cpp"
        "${CPP_SOURCE_DIR}/WindowProcessor.h"
)

add_library(polars_cpp ${CPP_SOURCES})

target_include_directories(polars_cpp PUBLIC ${Polars_SOURCE_DIR}/src/cpp)

if(BUILD_WITH_CONAN)
  target_link_libraries(polars_cpp CONAN_PKG::Armadillo CONAN_PKG::date)
else()
  target_link_libraries(polars_cpp armadillo date_interface)
endif()
