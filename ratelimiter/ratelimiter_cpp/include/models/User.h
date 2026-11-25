// ============================================================================
// FILE: include/models/User.h
// PURPOSE: Value object representing who is making the request.
//
// WHY HEADER-ONLY?
//   User is a simple data holder with only inline getters.
//   Inline methods defined inside the class body are implicitly inline
//   (won't cause linker errors even if included in multiple .cpp files).
//   No complex logic → no .cpp file needed.
// ============================================================================

#ifndef USER_H
#define USER_H

#include <string>           // For std::string
#include "UserTier.h"       // Enum for subscription tier

class User {
private:
    std::string userId_;    // Unique identifier (email, UUID, IP)
    UserTier tier_;         // Subscription plan (FREE or PREMIUM)

public:
    // Constructor: takes both values
    User(const std::string& userId, UserTier tier)
        : userId_(userId),  // Initializer list: initialize member directly
          tier_(tier) {}    // More efficient than assignment in body

    // Getters: const → they don't modify the object
    // Defined inline (inside class body) → no linker issues
    const std::string& getUserId() const { return userId_; }
    // const std::string& → return by reference, no copy, caller can't modify

    UserTier getTier() const { return tier_; }
};

#endif // USER_H

