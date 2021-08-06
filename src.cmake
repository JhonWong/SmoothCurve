include(${PROJECT_SOURCE_DIR}/bspline_library/src.cmake)
source_group(b_spline FILES ${bspline_source_files})
set(SOURCE_FILES ${SOURCE_FILES} ${bspline_source_files})

include(${PROJECT_SOURCE_DIR}/app/src.cmake)
source_group(app FILES ${app_source_files})
set(SOURCE_FILES ${SOURCE_FILES} ${app_source_files})

include(${PROJECT_SOURCE_DIR}/spline/src.cmake)
source_group(spline FILES ${spline_source_files})
set(SOURCE_FILES ${SOURCE_FILES} ${spline_source_files})

include(${PROJECT_SOURCE_DIR}/bezier/src.cmake)
source_group(bezier FILES ${bezier_source_files})
set(SOURCE_FILES ${SOURCE_FILES} ${bezier_source_files})