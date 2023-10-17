//
// Created by ruben.laso on 7/10/22.
//

#ifndef KERNELFACTORY_HPP
#define KERNELFACTORY_HPP

#include "KernelCircle.hpp"
#include "KernelCube.hpp"
#include "KernelSphere.hpp"
#include "KernelSquare.hpp"
#include "point.hpp"

#include <memory>

enum class Kernel_t // Different types of kernels to be used in the factory function
{
	circle,
	sphere,
	square,
	cube
};

template<Kernel_t kernel_type>
inline auto kernelFactory_ptr(const Point& center, const double radius) -> std::unique_ptr<KernelAbstract>
{
	if constexpr (kernel_type == Kernel_t::circle) { return std::make_unique<KernelCircle>(center, radius); }
	else if constexpr (kernel_type == Kernel_t::square) { return std::make_unique<KernelSquare>(center, radius); }
	else if constexpr (kernel_type == Kernel_t::sphere) { return std::make_unique<KernelSphere>(center, radius); }
	else /* if constexpr (kernel_type == Kernel_t::cube) */ { return std::make_unique<KernelCube>(center, radius); }
}

template<Kernel_t kernel_type>
inline auto kernelFactory(const Point& center, const double radius)
{
	if constexpr (kernel_type == Kernel_t::circle) { return KernelCircle(center, radius); }
	else if constexpr (kernel_type == Kernel_t::square) { return KernelSquare(center, radius); }
	else if constexpr (kernel_type == Kernel_t::sphere) { return KernelSphere(center, radius); }
	else /* if constexpr (kernel_type == Kernel_t::cube) */ { return KernelCube(center, radius); }
}

template<Kernel_t kernel_type>
inline auto kernelFactory(const Point& center, const Vector& radii)
{
	// Check that kernel type can be used with given parameters
	constexpr bool valid_kernel_type = (kernel_type == Kernel_t::square) || (kernel_type == Kernel_t::cube);
	static_assert(valid_kernel_type, "Incorrect kernel type");

	if constexpr (kernel_type == Kernel_t::square) { return KernelSquare(center, radii); }
	else if constexpr (kernel_type == Kernel_t::cube) { return KernelCube(center, radii); }
}

#endif /* end of include guard: KERNELFACTORY_HPP */
