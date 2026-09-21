
add_executable(test_gf2n test_gf2n.cpp)
target_link_libraries(test_gf2n PRIVATE galois)
add_test(NAME test_gf2n COMMAND test_gf2n)


add_executable(test_lfsr test_lfsr.cpp)
target_link_libraries(test_lfsr PRIVATE galois)
add_test(NAME test_lfsr COMMAND test_lfsr)