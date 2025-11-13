
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS  *.cpp *.h *.hpp *.c *.cc *.inl *.ipp *.hxx *.cxx *.tpp *.txx)

add_executable(iouring-webchat ${SRC})
target_include_directories(iouring-webchat PRIVATE .)
target_link_libraries(iouring-webchat uring)
