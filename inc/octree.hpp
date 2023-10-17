//
// Created by miguel.yermo on 5/03/20.
//

#pragma once

#include "Lpoint.hpp"
#include "NeighborKernels/KernelFactory.hpp"
#include "point.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>

class Octree
{
	private:
	// Keep dividing the octree while octants have more points than these.
	static constexpr unsigned int MAX_POINTS        = 100;
	static constexpr float        MIN_OCTANT_RADIUS = 0.1;
	static constexpr size_t       DEFAULT_KNN       = 100;
	static constexpr short        OCTANTS_PER_NODE  = 8;

	std::vector<Octree>  octants_{};
	Point                center_{};
	Point                min_{};
	Point                max_{};
	std::vector<Lpoint*> points_{};
	float                radius_{};

	public:
	Octree();

	explicit Octree(std::vector<Lpoint>& points);
	explicit Octree(std::vector<Lpoint*>& points);

	Octree(const Point& center, float radius);
	Octree(Point center, float radius, std::vector<Lpoint*>& points);
	Octree(Point center, float radius, std::vector<Lpoint>& points);

	[[nodiscard]] inline const std::vector<Octree>& getOctants() const { return octants_; }

	inline void setOctants(const std::vector<Octree>& octants) { octants_ = octants; }
	inline void setCenter(const Point& center) { center_ = center; }
	inline void setMin(const Point& min) { min_ = min; }
	inline void setMax(const Point& max) { max_ = max; }

	[[nodiscard]] inline const std::vector<Lpoint*>& getPoints() const { return points_; }

	[[nodiscard]] inline auto getNumPoints() const { return points_.size(); }

	inline void setPoints(const std::vector<Lpoint*>& points) { points_ = points; }
	inline void setRadius(float radius) { radius_ = radius; }

	[[nodiscard]] inline const Point& getMin() const { return min_; }
	[[nodiscard]] inline const Point& getMax() const { return max_; }

	void computeOctreeLimits();

	[[nodiscard]] inline double getDensity() const
	/*
   * @brief Computes the point density of the given Octree as nPoints / Volume
   */
	{
		return static_cast<double>(points_.size()) / (radius_ * radius_ + radius_);
	}

	void writeDensities(const std::filesystem::path& path) const;
	void writeNumPoints(const std::filesystem::path& path) const;

	[[nodiscard]] const Octree* findOctant(const Lpoint* p) const;

	[[nodiscard]] std::vector<std::pair<Point, double>> computeDensities() const;
	[[nodiscard]] std::vector<std::pair<Point, size_t>> computeNumPoints() const;

	[[nodiscard]] bool isInside2D(const Point& p) const;

	void   insertPoints(std::vector<Lpoint>& points);
	void   insertPoints(std::vector<Lpoint*>& points);
	void   insertPoint(Lpoint* p);
	void   createOctants();
	void   fillOctants();
	size_t octantIdx(const Lpoint* p) const;

	[[nodiscard]] inline bool isLeaf() const { return octants_.empty(); }
	[[nodiscard]] inline bool isEmpty() const { return this->points_.empty(); };

	void buildOctree(std::vector<Lpoint>& points);
	void buildOctree(std::vector<Lpoint*>& points);

	[[nodiscard]] inline auto& getCenter() const { return center_; }
	[[nodiscard]] inline auto  getRadius() const { return radius_; }

	template<Kernel_t kernel_type = Kernel_t::square>
	[[nodiscard]] inline std::vector<Lpoint*> searchNeighbors(const Point& p, double radius) const
	/**
   * @brief Search neighbors function. Given a point and a radius, return the points inside a given kernel type
   * @param p Center of the kernel to be used
   * @param radius Radius of the kernel to be used
   * @return Points inside the given kernel type
   */
	{
		const auto kernel = kernelFactory<kernel_type>(p, radius);
		// Dummy condition that always returns true, so we can use the same function for all cases
		// The compiler should optimize this away
		constexpr auto dummyCondition = [](const Lpoint&) { return true; };

		return neighbors(kernel, dummyCondition);
	}

	template<Kernel_t kernel_type = Kernel_t::cube>
	[[nodiscard]] inline std::vector<Lpoint*> searchNeighbors(const Point& p, const Vector& radii) const
	/**
   * @brief Search neighbors function. Given a point and a radius, return the points inside a given kernel type
   * @param p Center of the kernel to be used
   * @param radii Radii of the kernel to be used
   * @return Points inside the given kernel type
   */
	{
		const auto kernel = kernelFactory<kernel_type>(p, radii);
		// Dummy condition that always returns true, so we can use the same function for all cases
		// The compiler should optimize this away
		constexpr auto dummyCondition = [](const Lpoint&) { return true; };

		return neighbors(kernel, dummyCondition);
	}

	template<Kernel_t kernel_type = Kernel_t::square, class Function>
	[[nodiscard]] inline std::vector<Lpoint*> searchNeighbors(const Point& p, double radius, Function&& condition) const
	/**
   * @brief Search neighbors function. Given a point and a radius, return the points inside a given kernel type
   * @param p Center of the kernel to be used
   * @param radius Radius of the kernel to be used
   * @param condition function that takes a candidate neighbor point and imposes an additional condition (should return a boolean).
   * The signature of the function should be equivalent to `bool cnd(const Lpoint &p);`
   * @return Points inside the given kernel type
   */
	{
		const auto kernel = kernelFactory<kernel_type>(p, radius);
		return neighbors(kernel, std::forward<Function&&>(condition));
	}

	template<Kernel_t kernel_type = Kernel_t::square, class Function>
	[[nodiscard]] inline std::vector<Lpoint*> searchNeighbors(const Point& p, const Vector& radii,
	                                                          Function&& condition) const
	/**
   * @brief Search neighbors function. Given a point and a radius, return the points inside a given kernel type
   * @param p Center of the kernel to be used
   * @param radii Radii of the kernel to be used
   * @param condition function that takes a candidate neighbor point and imposes an additional condition (should return a boolean).
   * The signature of the function should be equivalent to `bool cnd(const Lpoint &p);`
   * @return Points inside the given kernel type
   */
	{
		const auto kernel = kernelFactory<kernel_type>(p, radii);
		return neighbors(kernel, std::forward<Function&&>(condition));
	}

	[[nodiscard]] std::vector<Lpoint*> KNN(const Point& p, size_t k, size_t maxNeighs = DEFAULT_KNN) const;

	template<typename Kernel, typename Function>
	[[nodiscard]] std::vector<Lpoint*> neighbors(const Kernel& k, Function&& condition) const
	/**
   * @brief Search neighbors function. Given kernel that already contains a point and a radius, return the points inside the region.
   * @param k specific kernel that contains the data of the region (center and radius)
   * @param condition function that takes a candidate neighbor point and imposes an additional condition (should return a boolean).
   * The signature of the function should be equivalent to `bool cnd(const Lpoint &p);`
   * @return Points inside the given kernel type. Actually the same as ptsInside.
   */
	{
		std::vector<Lpoint*> ptsInside;

		std::vector<std::reference_wrapper<const Octree>> toVisit;
		toVisit.reserve(OCTANTS_PER_NODE); // There is usually less than 8 octants to visit
		toVisit.emplace_back(*this);

		while (!toVisit.empty())
		{
			const auto& octree = toVisit.back().get();
			toVisit.pop_back();

			if (octree.isLeaf())
			{
				for (Lpoint* point_ptr : octree.points_)
				{
					if (k.isInside(*point_ptr) && k.center().id() != point_ptr->id() && condition(*point_ptr))
					{
						ptsInside.emplace_back(point_ptr);
					}
				}
			}
			else
			{
				std::copy_if(std::begin(octree.octants_), std::end(octree.octants_), std::back_inserter(toVisit),
				             [&](const Octree& octant) { return k.boxOverlap(octant.getCenter(), octant.getRadius()); });
			}
		}

		return ptsInside;
	}

	[[nodiscard]] inline std::vector<Lpoint*> searchNeighbors2D(const Point& p, const double radius) const
	{
		return searchNeighbors<Kernel_t::square>(p, radius);
	}

	[[nodiscard]] inline std::vector<Lpoint*> searchCylinderNeighbors(const Lpoint& p, const double radius,
	                                                                  const double zMin, const double zMax) const
	{
		return searchNeighbors<Kernel_t::circle>(p, radius,
		                                         [&](const Lpoint& p) { return p.getZ() >= zMin && p.getZ() <= zMax; });
	}

	[[nodiscard]] inline std::vector<Lpoint*> searchCircleNeighbors(const Lpoint& p, const double radius) const
	{
		return searchNeighbors<Kernel_t::circle>(p, radius);
	}

	[[nodiscard]] inline std::vector<Lpoint*> searchCircleNeighbors(const Lpoint* p, const double radius) const
	{
		return searchCircleNeighbors(*p, radius);
	}

	[[nodiscard]] inline std::vector<Lpoint*> searchNeighbors3D(const Point& p, const Vector& radii) const
	{
		return searchNeighbors<Kernel_t::cube>(p, radii);
	}

	[[nodiscard]] inline std::vector<Lpoint*> searchNeighbors3D(const Point& p, double radius) const
	{
		return searchNeighbors<Kernel_t::cube>(p, radius);
	}

	[[nodiscard]] inline std::vector<Lpoint*> searchNeighbors3D(const Point& p, const double radius,
	                                                            const std::vector<bool>& flags) const
	/**
   * Searching neighbors in 3D using a different radius for each direction
   * @param p Point around the neighbors will be search
   * @param radius Vector of radiuses: one per spatial direction
   * @param flags Vector of flags: return only points which flags[pointId] == false
   * @return Points inside the given kernel
   */
	{
		const auto condition = [&](const Point& point) { return !flags[point.id()]; };
		return searchNeighbors<Kernel_t::cube>(p, radius, condition);
	}


	template<Kernel_t kernel_type = Kernel_t::square>
	[[nodiscard]] inline size_t numNeighbors(const Point& p, const double radius) const
	/**
   * @brief Search neighbors function. Given a point and a radius, return the number of points inside a given kernel type
   * @param p Center of the kernel to be used
   * @param radius Radius of the kernel to be used
   * @param condition function that takes a candidate neighbor point and imposes an additional condition (should return a boolean).
   * The signature of the function should be equivalent to `bool cnd(const Lpoint &p);`
   * @return Points inside the given kernel
   */
	{
		const auto kernel = kernelFactory<kernel_type>(p, radius);
		return numNeighbors(kernel);
	}

	template<Kernel_t kernel_type = Kernel_t::square, class Function>
	[[nodiscard]] inline size_t numNeighbors(const Point& p, const double radius, Function&& condition) const
	/**
   * @brief Search neighbors function. Given a point and a radius, return the number of points inside a given kernel type
   * @param p Center of the kernel to be used
   * @param radius Radius of the kernel to be used
   * @param condition function that takes a candidate neighbor point and imposes an additional condition (should return a boolean).
   * The signature of the function should be equivalent to `bool cnd(const Lpoint &p);`
   * @return Points inside the given kernel
   */
	{
		const auto kernel = kernelFactory<kernel_type>(p, radius);
		return numNeighbors(kernel, std::forward<Function&&>(condition));
	}

	template<typename Kernel>
	[[nodiscard]] size_t numNeighbors(const Kernel& k) const
	{
		size_t ptsInside = 0;

		std::vector<std::reference_wrapper<const Octree>> toVisit;
		toVisit.reserve(OCTANTS_PER_NODE); // There is usually less than 8 octants to visit
		toVisit.emplace_back(*this);

		while (!toVisit.empty())
		{
			const auto& octree = toVisit.back().get();
			toVisit.pop_back();

			if (octree.isLeaf())
			{
				for (Lpoint* point_ptr : octree.points_)
				{
					if (k.isInside(*point_ptr) && k.center().id() != point_ptr->id()) { ++ptsInside; }
				}
			}
			else
			{
				std::copy_if(std::begin(octree.octants_), std::end(octree.octants_), std::back_inserter(toVisit),
				             [&](const Octree& octant) { return k.boxOverlap(octant.getCenter(), octant.getRadius()); });
			}
		}

		return ptsInside;
	}

	template<typename Kernel, typename Function>
	[[nodiscard]] size_t numNeighbors(const Kernel& k, Function&& condition) const
	{
		size_t ptsInside = 0;

		std::vector<std::reference_wrapper<const Octree>> toVisit;
		toVisit.reserve(OCTANTS_PER_NODE); // There is usually less than 8 octants to visit
		toVisit.emplace_back(*this);

		while (!toVisit.empty())
		{
			const auto& octree = toVisit.back().get();
			toVisit.pop_back();

			if (octree.isLeaf())
			{
				for (Lpoint* point_ptr : octree.points_)
				{
					if (k.isInside(*point_ptr) && k.center().id() != point_ptr->id() && condition(*point_ptr)) { ++ptsInside; }
				}
			}
			else
			{
				std::copy_if(std::begin(octree.octants_), std::end(octree.octants_), std::back_inserter(toVisit),
				             [&](const Octree& octant) { return k.boxOverlap(octant.getCenter(), octant.getRadius()); });
			}
		}

		return ptsInside;
	}

	/** Inside a sphere */
	[[nodiscard]] inline std::vector<Lpoint*> searchSphereNeighbors(const Point& point, const float radius) const
	{
		return searchNeighbors<Kernel_t::sphere>(point, radius);
	}

	[[nodiscard]] std::vector<Lpoint*> searchNeighborsRing(const Lpoint& p, const Vector& innerRingRadii,
	                                                       const Vector& outerRingRadii) const
	/**
	 * A point is considered to be inside a Ring around a point if its outside the innerRing and inside the outerRing
	 * @param p Center of the kernel to be used
	 * @param innerRingRadii Radii of the inner part of the ring. Points within this part will be excluded
	 * @param outerRingRadii Radii of the outer part of the ring
	 * @return The points located between the inner ring and the outer ring
	 */
	{
		// Search points within "outerRingRadii"
		const auto outerKernel = kernelFactory<Kernel_t::cube>(p, outerRingRadii);
		// But not too close (within "innerRingRadii")
		const auto innerKernel = kernelFactory<Kernel_t::cube>(p, innerRingRadii);
		const auto condition   = [&](const Point& point) { return !innerKernel.isInside(point); };

		return neighbors(outerKernel, condition);
	}

	void writeOctree(std::ofstream& f, size_t index) const;

	void    extractPoint(const Lpoint* p);
	Lpoint* extractPoint();
	void    extractPoints(std::vector<Lpoint>& points);
	void    extractPoints(std::vector<Lpoint*>& points);

	std::vector<Lpoint*> searchEraseCircleNeighbors(const std::vector<Lpoint*>& points, double radius);

	/** Inside a sphere */
	std::vector<Lpoint*> searchEraseSphereNeighbors(const std::vector<Lpoint*>& points, float radius);

	/** Connected inside a spherical shell*/
	[[nodiscard]] std::vector<Lpoint*> searchConnectedShellNeighbors(const Point& point, float nextDoorDistance,
	                                                                 float minRadius, float maxRadius) const;

	/** Connected circle neighbors*/
	std::vector<Lpoint*> searchEraseConnectedCircleNeighbors(float nextDoorDistance);

	static std::vector<Lpoint*> connectedNeighbors(const Point* point, std::vector<Lpoint*>& neighbors,
	                                               float nextDoorDistance);

	static std::vector<Lpoint*> extractCloseNeighbors(const Point* p, std::vector<Lpoint*>& neighbors, float radius);

	std::vector<Lpoint*> kClosestCircleNeighbors(const Lpoint* p, size_t k) const;
	std::vector<Lpoint*> nCircleNeighbors(const Lpoint* p, size_t n, float& radius, float minRadius, float maxRadius,
	                                      float maxIncrement = 0.25, float maxDecrement = 0.25) const;

	std::vector<Lpoint*> nSphereNeighbors(const Lpoint& p, size_t n, float& radius, float minRadius, float maxRadius,
	                                      float maxStep = 0.25) const;
};