/* Copyright (c) 2016 The Value Types Authors. All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
==============================================================================*/

#include "unique_polymorphic.h"

#include <gtest/gtest.h>

#include <array>
#include <map>

#if __has_include(<memory_resource>)
#include <memory_resource>
#endif  // #if __has_include(<memory_resource>)
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

class Base {
 public:
  virtual ~Base() = default;
  virtual int value() const = 0;
  virtual void set_value(int) = 0;
};
class Derived_NoSBO : public Base, public xyz::NoPolymorphicSBO {
 private:
  int value_;

 public:
  Derived_NoSBO(int v) : value_(v) {}
  Derived_NoSBO() : Derived_NoSBO(0) {}
  int value() const override { return value_; }
  void set_value(int v) override { value_ = v; }
};

class Derived : public Base {
 private:
  int value_;

 public:
  Derived(int v) : value_(v) {}
  Derived() : Derived(0) {}
  int value() const override { return value_; }
  void set_value(int v) override { value_ = v; }
};

TEST(UniquePolymorphicTest, ValueAccessFromInPlaceConstructedObject) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived_NoSBO>, 42);
  EXPECT_EQ(a->value(), 42);
}

TEST(UniquePolymorphicTest, ValueAccessFromInPlaceConstructedObjectWithSBO) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived>, 42);
  EXPECT_EQ(a->value(), 42);
}

TEST(UniquePolymorphicTest, ValueAccessFromDefaultConstructedObject) {
  xyz::unique_polymorphic<Derived_NoSBO> a;
  EXPECT_EQ(a->value(), 0);
}

TEST(UniquePolymorphicTest, ValueAccessFromDefaultConstructedObjectWithSBO) {
  xyz::unique_polymorphic<Derived> a;
  EXPECT_EQ(a->value(), 0);
}

TEST(UniquePolymorphicTest, MoveRendersSourceValueless) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived_NoSBO>, 42);
  auto aa = std::move(a);
  EXPECT_TRUE(a.valueless_after_move());
}

TEST(UniquePolymorphicTest, MoveRendersSourceValuelessWithSBO) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived>, 42);
  auto aa = std::move(a);
  EXPECT_TRUE(a.valueless_after_move());
}

TEST(UniquePolymorphicTest, Swap) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived_NoSBO>, 42);
  xyz::unique_polymorphic<Base> b(std::in_place_type<Derived_NoSBO>, 101);
  EXPECT_EQ(a->value(), 42);
  EXPECT_EQ(b->value(), 101);
  swap(a, b);
  EXPECT_EQ(a->value(), 101);
  EXPECT_EQ(b->value(), 42);
}

TEST(UniquePolymorphicTest, SwapWithSBO) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived>, 42);
  xyz::unique_polymorphic<Base> b(std::in_place_type<Derived>, 101);
  EXPECT_EQ(a->value(), 42);
  EXPECT_EQ(b->value(), 101);
  swap(a, b);
  EXPECT_EQ(a->value(), 101);
  EXPECT_EQ(b->value(), 42);
}

TEST(UniquePolymorphicTest, SwapWithNoSBOAndSBO) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived>, 42);
  xyz::unique_polymorphic<Base> b(std::in_place_type<Derived_NoSBO>, 101);
  EXPECT_EQ(a->value(), 42);
  EXPECT_EQ(b->value(), 101);
  swap(a, b);
  EXPECT_EQ(a->value(), 101);
  EXPECT_EQ(b->value(), 42);
}

TEST(UniquePolymorphicTest, SwapWithSBOAndNoSBO) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived_NoSBO>, 42);
  xyz::unique_polymorphic<Base> b(std::in_place_type<Derived>, 101);
  EXPECT_EQ(a->value(), 42);
  EXPECT_EQ(b->value(), 101);
  swap(a, b);
  EXPECT_EQ(a->value(), 101);
  EXPECT_EQ(b->value(), 42);
}

TEST(UniquePolymorphicTest, AccessDerivedObject) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived_NoSBO>, 42);
  EXPECT_EQ(a->value(), 42);
}

TEST(UniquePolymorphicTest, AccessDerivedObjectWithSBO) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived>, 42);
  EXPECT_EQ(a->value(), 42);
}

TEST(UniquePolymorphicTest, MoveAssignment) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived_NoSBO>, 42);
  xyz::unique_polymorphic<Base> b(std::in_place_type<Derived_NoSBO>, 101);
  EXPECT_EQ(a->value(), 42);
  a = std::move(b);

  EXPECT_TRUE(b.valueless_after_move());
  EXPECT_EQ(a->value(), 101);
}

TEST(UniquePolymorphicTest, MoveAssignmentWithSBO) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived>, 42);
  xyz::unique_polymorphic<Base> b(std::in_place_type<Derived>, 101);
  EXPECT_EQ(a->value(), 42);
  a = std::move(b);

  EXPECT_TRUE(b.valueless_after_move());
  EXPECT_EQ(a->value(), 101);
}

TEST(UniquePolymorphicTest, NonMemberSwap) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived_NoSBO>, 42);
  xyz::unique_polymorphic<Base> b(std::in_place_type<Derived_NoSBO>, 101);
  using std::swap;
  swap(a, b);
  EXPECT_EQ(a->value(), 101);
  EXPECT_EQ(b->value(), 42);
}

TEST(UniquePolymorphicTest, NonMemberSwapWithSBO) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived>, 42);
  xyz::unique_polymorphic<Base> b(std::in_place_type<Derived>, 101);
  using std::swap;
  swap(a, b);
  EXPECT_EQ(a->value(), 101);
  EXPECT_EQ(b->value(), 42);
}

TEST(UniquePolymorphicTest, MemberSwap) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived_NoSBO>, 42);
  xyz::unique_polymorphic<Base> b(std::in_place_type<Derived_NoSBO>, 101);

  a.swap(b);
  EXPECT_EQ(a->value(), 101);
  EXPECT_EQ(b->value(), 42);
}

TEST(UniquePolymorphicTest, MemberSwapWithSBO) {
  xyz::unique_polymorphic<Base> a(std::in_place_type<Derived>, 42);
  xyz::unique_polymorphic<Base> b(std::in_place_type<Derived>, 101);

  a.swap(b);
  EXPECT_EQ(a->value(), 101);
  EXPECT_EQ(b->value(), 42);
}

TEST(UniquePolymorphicTest, ConstPropagation) {
  struct SomeType : xyz::NoPolymorphicSBO {
    enum class Constness { CONST, NON_CONST };
    Constness member() { return Constness::NON_CONST; }
    Constness member() const { return Constness::CONST; }
  };

  xyz::unique_polymorphic<SomeType> a(std::in_place_type<SomeType>);
  EXPECT_EQ(a->member(), SomeType::Constness::NON_CONST);
  EXPECT_EQ((*a).member(), SomeType::Constness::NON_CONST);
  const auto& ca = a;
  EXPECT_EQ(ca->member(), SomeType::Constness::CONST);
  EXPECT_EQ((*ca).member(), SomeType::Constness::CONST);
}

TEST(UniquePolymorphicTest, ConstPropagationWithSBO) {
  struct SomeType {
    enum class Constness { CONST, NON_CONST };
    Constness member() { return Constness::NON_CONST; }
    Constness member() const { return Constness::CONST; }
  };

  xyz::unique_polymorphic<SomeType> a(std::in_place_type<SomeType>);
  EXPECT_EQ(a->member(), SomeType::Constness::NON_CONST);
  EXPECT_EQ((*a).member(), SomeType::Constness::NON_CONST);
  const auto& ca = a;
  EXPECT_EQ(ca->member(), SomeType::Constness::CONST);
  EXPECT_EQ((*ca).member(), SomeType::Constness::CONST);
}

template <typename T>
struct TrackingAllocator {
  unsigned* alloc_counter_;
  unsigned* dealloc_counter_;

  using propagate_on_container_move_assignment = std::true_type;

  TrackingAllocator(unsigned* alloc_counter, unsigned* dealloc_counter)
      : alloc_counter_(alloc_counter), dealloc_counter_(dealloc_counter) {}

  template <typename U>
  TrackingAllocator(const TrackingAllocator<U>& other)
      : alloc_counter_(other.alloc_counter_),
        dealloc_counter_(other.dealloc_counter_) {}

  using value_type = T;

  template <typename Other>
  struct rebind {
    using other = TrackingAllocator<Other>;
  };

  constexpr T* allocate(std::size_t n) {
    ++(*alloc_counter_);
    std::allocator<T> default_allocator{};
    return default_allocator.allocate(n);
  }
  constexpr void deallocate(T* p, std::size_t n) {
    ++(*dealloc_counter_);
    std::allocator<T> default_allocator{};
    default_allocator.deallocate(p, n);
  }

  friend bool operator==(const TrackingAllocator& lhs,
                         const TrackingAllocator& rhs) noexcept {
    return lhs.alloc_counter_ == rhs.alloc_counter_ &&
           lhs.dealloc_counter_ == rhs.dealloc_counter_;
  }
};

TEST(UniquePolymorphicTest, GetAllocator) {
  unsigned alloc_counter = 0;
  unsigned dealloc_counter = 0;

  xyz::unique_polymorphic<Base, TrackingAllocator<Base>> a(
      std::allocator_arg,
      TrackingAllocator<Base>(&alloc_counter, &dealloc_counter),
      std::in_place_type<Derived_NoSBO>, 42);
  EXPECT_EQ(alloc_counter, 1);
  EXPECT_EQ(dealloc_counter, 0);

  auto tracking_allocator = a.get_allocator();
  EXPECT_EQ(alloc_counter, *tracking_allocator.alloc_counter_);
  EXPECT_EQ(dealloc_counter, *tracking_allocator.dealloc_counter_);
}

TEST(UniquePolymorphicTest, CountAllocationsForInPlaceConstruction) {
  unsigned alloc_counter = 0;
  unsigned dealloc_counter = 0;
  {
    xyz::unique_polymorphic<Base, TrackingAllocator<Base>> a(
        std::allocator_arg,
        TrackingAllocator<Base>(&alloc_counter, &dealloc_counter),
        std::in_place_type<Derived_NoSBO>, 42);
    EXPECT_EQ(alloc_counter, 1);
    EXPECT_EQ(dealloc_counter, 0);
  }
  EXPECT_EQ(alloc_counter, 1);
  EXPECT_EQ(dealloc_counter, 1);
}

TEST(UniquePolymorphicTest, CountAllocationsForDerivedTypeConstruction) {
  unsigned alloc_counter = 0;
  unsigned dealloc_counter = 0;
  {
    xyz::unique_polymorphic<Base, TrackingAllocator<Base>> a(
        std::allocator_arg,
        TrackingAllocator<Base>(&alloc_counter, &dealloc_counter),
        std::in_place_type<Derived_NoSBO>, 42);
    EXPECT_EQ(alloc_counter, 1);
    EXPECT_EQ(dealloc_counter, 0);
  }
  EXPECT_EQ(alloc_counter, 1);
  EXPECT_EQ(dealloc_counter, 1);
}

TEST(UniquePolymorphicTest, CountAllocationsForMoveAssignment) {
  unsigned alloc_counter = 0;
  unsigned dealloc_counter = 0;
  {
    xyz::unique_polymorphic<Derived_NoSBO, TrackingAllocator<Derived_NoSBO>> a(
        std::allocator_arg,
        TrackingAllocator<Derived_NoSBO>(&alloc_counter, &dealloc_counter),
        std::in_place_type<Derived_NoSBO>, 42);
    xyz::unique_polymorphic<Derived_NoSBO, TrackingAllocator<Derived_NoSBO>> b(
        std::allocator_arg,
        TrackingAllocator<Derived_NoSBO>(&alloc_counter, &dealloc_counter),
        std::in_place_type<Derived_NoSBO>, 101);
    EXPECT_EQ(alloc_counter, 2);
    EXPECT_EQ(dealloc_counter, 0);
    b = std::move(a);
  }
  EXPECT_EQ(alloc_counter, 2);
  EXPECT_EQ(dealloc_counter, 2);
}

template <typename T>
struct NonEqualTrackingAllocator : TrackingAllocator<T> {
  using TrackingAllocator<T>::TrackingAllocator;

  template <typename Other>
  struct rebind {
    using other = NonEqualTrackingAllocator<Other>;
  };

  friend bool operator==(const NonEqualTrackingAllocator&,
                         const NonEqualTrackingAllocator&) noexcept {
    return false;
  }
};

#ifdef XYZ_POLYMORPHIC_USES_EXPERIMENTAL_SMALL_BUFFER_OPTIMIZATION
TEST(UniquePolymorphicTest, CountAllocationsForMoveAssignmentWithSBO) {
  unsigned alloc_counter = 0;
  unsigned dealloc_counter = 0;
  {
    xyz::unique_polymorphic<Derived, TrackingAllocator<Derived>> a(
        std::allocator_arg,
        TrackingAllocator<Derived>(&alloc_counter, &dealloc_counter),
        std::in_place_type<Derived>, 42);
    xyz::unique_polymorphic<Derived, TrackingAllocator<Derived>> b(
        std::allocator_arg,
        TrackingAllocator<Derived>(&alloc_counter, &dealloc_counter),
        std::in_place_type<Derived>, 101);
    EXPECT_EQ(alloc_counter, 0);
    EXPECT_EQ(dealloc_counter, 0);
    b = std::move(a);
  }
  // We never allocated as SBO was used.
  EXPECT_EQ(alloc_counter, 0);
  EXPECT_EQ(dealloc_counter, 0);
}

TEST(UniquePolymorphicTest,
     CountAllocationsForMoveAssignmentWhenAllocatorsDontCompareEqualWithSBO) {
  unsigned alloc_counter = 0;
  unsigned dealloc_counter = 0;
  {
    xyz::unique_polymorphic<Derived, NonEqualTrackingAllocator<Derived>> a(
        std::allocator_arg,
        NonEqualTrackingAllocator<Derived>(&alloc_counter, &dealloc_counter),
        std::in_place_type<Derived>, 42);
    xyz::unique_polymorphic<Derived, NonEqualTrackingAllocator<Derived>> b(
        std::allocator_arg,
        NonEqualTrackingAllocator<Derived>(&alloc_counter, &dealloc_counter),
        std::in_place_type<Derived>, 101);
    EXPECT_EQ(alloc_counter, 0);
    EXPECT_EQ(dealloc_counter, 0);
    b = std::move(a);
  }
  // We never allocated as SBO was used.
  EXPECT_EQ(alloc_counter, 0);
  EXPECT_EQ(dealloc_counter, 0);
}
#endif  // XYZ_POLYMORPHIC_USES_EXPERIMENTAL_SMALL_BUFFER_OPTIMIZATION

TEST(UniquePolymorphicTest, CountAllocationsForMoveConstruction) {
  unsigned alloc_counter = 0;
  unsigned dealloc_counter = 0;
  {
    xyz::unique_polymorphic<Derived_NoSBO, TrackingAllocator<Derived_NoSBO>> a(
        std::allocator_arg,
        TrackingAllocator<Derived_NoSBO>(&alloc_counter, &dealloc_counter),
        std::in_place_type<Derived_NoSBO>, 42);
    EXPECT_EQ(alloc_counter, 1);
    EXPECT_EQ(dealloc_counter, 0);
    xyz::unique_polymorphic<Derived_NoSBO, TrackingAllocator<Derived_NoSBO>> b(
        std::move(a));
  }
  EXPECT_EQ(alloc_counter, 1);
  EXPECT_EQ(dealloc_counter, 1);
}

template <typename T>
struct POCSTrackingAllocator : TrackingAllocator<T> {
  using TrackingAllocator<T>::TrackingAllocator;
  using propagate_on_container_swap = std::true_type;

  template <typename Other>
  struct rebind {
    using other = POCSTrackingAllocator<Other>;
  };
};

TEST(UniquePolymorphicTest, NonMemberSwapWhenAllocatorsDontCompareEqual) {
  unsigned alloc_counter = 0;
  unsigned dealloc_counter = 0;
  {
    xyz::unique_polymorphic<Derived_NoSBO, POCSTrackingAllocator<Derived_NoSBO>>
        a(std::allocator_arg,
          POCSTrackingAllocator<Derived_NoSBO>(&alloc_counter,
                                               &dealloc_counter),
          std::in_place_type<Derived_NoSBO>, 42);
    xyz::unique_polymorphic<Derived_NoSBO, POCSTrackingAllocator<Derived_NoSBO>>
        b(std::allocator_arg,
          POCSTrackingAllocator<Derived_NoSBO>(&alloc_counter,
                                               &dealloc_counter),
          std::in_place_type<Derived_NoSBO>, 101);
    EXPECT_EQ(alloc_counter, 2);
    EXPECT_EQ(dealloc_counter, 0);
    swap(a, b);
    EXPECT_EQ(a->value(), 101);
    EXPECT_EQ(b->value(), 42);
  }
  EXPECT_EQ(alloc_counter, 2);
  EXPECT_EQ(dealloc_counter, 2);
}

TEST(UniquePolymorphicTest, MemberSwapWhenAllocatorsDontCompareEqual) {
  unsigned alloc_counter = 0;
  unsigned dealloc_counter = 0;
  {
    xyz::unique_polymorphic<Derived_NoSBO, POCSTrackingAllocator<Derived_NoSBO>>
        a(std::allocator_arg,
          POCSTrackingAllocator<Derived_NoSBO>(&alloc_counter,
                                               &dealloc_counter),
          std::in_place_type<Derived_NoSBO>, 42);
    xyz::unique_polymorphic<Derived_NoSBO, POCSTrackingAllocator<Derived_NoSBO>>
        b(std::allocator_arg,
          POCSTrackingAllocator<Derived_NoSBO>(&alloc_counter,
                                               &dealloc_counter),
          std::in_place_type<Derived_NoSBO>, 101);
    EXPECT_EQ(alloc_counter, 2);
    EXPECT_EQ(dealloc_counter, 0);
    a.swap(b);
    EXPECT_EQ(a->value(), 101);
    EXPECT_EQ(b->value(), 42);
  }
  EXPECT_EQ(alloc_counter, 2);
  EXPECT_EQ(dealloc_counter, 2);
}

struct ThrowsOnConstruction : xyz::NoPolymorphicSBO {
  class Exception : public std::exception {
    const char* what() const noexcept override {
      return "ThrowsOnConstruction::Exception";
    }
  };

  template <typename... Args>
  ThrowsOnConstruction(Args&&...) {
    throw Exception();
  }
};

struct ThrowsOnConstructionWithSBO {
  class Exception : public std::exception {
    const char* what() const noexcept override {
      return "ThrowsOnConstructionWithSBO::Exception";
    }
  };

  template <typename... Args>
  ThrowsOnConstructionWithSBO(Args&&...) {
    throw Exception();
  }
};

TEST(UniquePolymorphicTest, DefaultConstructorWithExceptions) {
  EXPECT_THROW(xyz::unique_polymorphic<ThrowsOnConstruction>(),
               ThrowsOnConstruction::Exception);
}

TEST(UniquePolymorphicTest, DefaultConstructorWithExceptionsWithSBO) {
  EXPECT_THROW(xyz::unique_polymorphic<ThrowsOnConstructionWithSBO>(),
               ThrowsOnConstructionWithSBO::Exception);
}

TEST(UniquePolymorphicTest, ConstructorWithExceptions) {
  EXPECT_THROW(xyz::unique_polymorphic<ThrowsOnConstruction>(
                   std::in_place_type<ThrowsOnConstruction>, "unused"),
               ThrowsOnConstruction::Exception);
}

TEST(UniquePolymorphicTest, ConstructorWithExceptionsWithSBO) {
  EXPECT_THROW(xyz::unique_polymorphic<ThrowsOnConstructionWithSBO>(
                   std::in_place_type<ThrowsOnConstructionWithSBO>, "unused"),
               ThrowsOnConstructionWithSBO::Exception);
}

TEST(UniquePolymorphicTest, ConstructorWithExceptionsTrackingAllocations) {
  unsigned alloc_counter = 0;
  unsigned dealloc_counter = 0;
  auto construct = [&]() {
    return xyz::unique_polymorphic<ThrowsOnConstruction,
                                   TrackingAllocator<ThrowsOnConstruction>>(
        std::allocator_arg,
        TrackingAllocator<ThrowsOnConstruction>(&alloc_counter,
                                                &dealloc_counter),
        std::in_place_type<ThrowsOnConstruction>, "unused");
  };
  EXPECT_THROW(construct(), ThrowsOnConstruction::Exception);
  EXPECT_EQ(alloc_counter, 1);
  EXPECT_EQ(dealloc_counter, 1);
}

TEST(UniquePolymorphicTest, InteractionWithOptional) {
  std::optional<xyz::unique_polymorphic<Base>> a;
  EXPECT_FALSE(a.has_value());
  a.emplace(std::in_place_type<Derived_NoSBO>, 42);
  EXPECT_TRUE(a.has_value());
  EXPECT_EQ((*a)->value(), 42);
}

TEST(UniquePolymorphicTest, InteractionWithVector) {
  std::vector<xyz::unique_polymorphic<Base>> as;
  for (int i = 0; i < 16; ++i) {
    as.push_back(
        xyz::unique_polymorphic<Base>(std::in_place_type<Derived_NoSBO>, i));
  }
  for (int i = 0; i < 16; ++i) {
    EXPECT_EQ(as[i]->value(), i);
  }
}

TEST(UniquePolymorphicTest, InteractionWithMap) {
  std::map<int, xyz::unique_polymorphic<Base>> as;
  for (int i = 0; i < 16; ++i) {
    as.emplace(
        i, xyz::unique_polymorphic<Base>(std::in_place_type<Derived_NoSBO>, i));
  }
  for (auto& [k, v] : as) {
    EXPECT_EQ(v->value(), k);
  }
}

TEST(UniquePolymorphicTest, InteractionWithUnorderedMap) {
  std::unordered_map<int, xyz::unique_polymorphic<Base>> as;
  for (int i = 0; i < 16; ++i) {
    as.emplace(
        i, xyz::unique_polymorphic<Base>(std::in_place_type<Derived_NoSBO>, i));
  }
  for (auto& [k, v] : as) {
    EXPECT_EQ(v->value(), k);
  }
}

TEST(UniquePolymorphicTest, InteractionWithSizedAllocators) {
  EXPECT_EQ(
      sizeof(xyz::unique_polymorphic<int, TrackingAllocator<int>>),
      (sizeof(xyz::unique_polymorphic<int>) + sizeof(TrackingAllocator<int>)));
}

struct BaseA {
  int a_value = 3;
  virtual ~BaseA() = default;
  virtual int value() { return a_value; }
};

struct BaseB {
  int b_value = 4;
  virtual ~BaseB() = default;
  virtual int value() { return b_value; }
};

struct MultipleBases : BaseA, BaseB {
  int d_value = 5;
  virtual int value() { return d_value; }
};

TEST(UniquePolymorphicTest, MultipleBases) {
  xyz::unique_polymorphic<BaseA> a(std::in_place_type<MultipleBases>);

  xyz::unique_polymorphic<BaseB> b(std::in_place_type<MultipleBases>);

  EXPECT_EQ(a->value(), 5);
  EXPECT_EQ(b->value(), 5);
  EXPECT_EQ(a->a_value, 3);
  EXPECT_EQ(b->b_value, 4);
}

#if (__cpp_lib_memory_resource >= 201603L)
// TODO: Fix compilation issues with pmr allocators and SBO.
// https://github.com/jbcoe/value_types/issues/112
#ifndef XYZ_POLYMORPHIC_USES_EXPERIMENTAL_SMALL_BUFFER_OPTIMIZATION
TEST(UniquePolymorphicTest, InteractionWithPMRAllocators) {
  std::array<std::byte, 1024> buffer;
  std::pmr::monotonic_buffer_resource mbr{buffer.data(), buffer.size()};
  std::pmr::polymorphic_allocator<Base> pa{&mbr};
  using PolymorphicBase =
      xyz::unique_polymorphic<Base, std::pmr::polymorphic_allocator<Base>>;
  PolymorphicBase a(std::allocator_arg, pa, std::in_place_type<Derived_NoSBO>,
                    42);
  std::pmr::vector<PolymorphicBase> values{pa};
  values.push_back(std::move(a));
  EXPECT_EQ(values[0]->value(), 42);
}

#endif  // XYZ_POLYMORPHIC_USES_EXPERIMENTAL_SMALL_BUFFER_OPTIMIZATION
#endif  // (__cpp_lib_memory_resource >= 201603L)

}  // namespace