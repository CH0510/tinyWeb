aux_source_directory(./ SRC_LIST)

# 设置 库文件的默认输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 使用${SRC_LIST}为原材料生成静态库
add_library(tinyWeb_net STATIC ${SRC_LIST})
target_link_libraries(tinyWeb_net tinyWeb_base)