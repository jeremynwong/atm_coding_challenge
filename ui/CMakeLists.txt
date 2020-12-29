################################################################################
### UI Programs
################################################################################

# Admin User Interface used by bank employee to add new accounts
add_executable(admin ${CMAKE_CURRENT_SOURCE_DIR}/AdminUI.cpp)
target_link_libraries(admin atm)

# ATM User Interface used by customers
add_executable(user ${CMAKE_CURRENT_SOURCE_DIR}/UserUI.cpp)
target_link_libraries(user atm)