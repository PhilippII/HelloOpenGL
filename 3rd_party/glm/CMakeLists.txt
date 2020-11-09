add_library(GLM INTERFACE)
# specify headers as system headers in order to skip warinings
target_include_directories(GLM SYSTEM INTERFACE .)
