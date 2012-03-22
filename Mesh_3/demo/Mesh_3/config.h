#ifndef CGAL_DEMO_MESH_3_CONFIG_H
#define CGAL_DEMO_MESH_3_CONFIG_H

// #define CGAL_POLYHEDRON_DEMO_NO_NEF
// #define CGAL_POLYHEDRON_DEMO_NO_SURFACE_MESHER
// #define CGAL_POLYHEDRON_DEMO_NO_PARAMETRIZATION

#define CGAL_MESH_3_VERBOSE
#define CGAL_MESH_3_IO_VERBOSE

#ifndef CGAL_POLYHEDRON_DEMO_NO_PARAMETRIZATION
#  define CGAL_POLYHEDRON_DEMO_USE_PARAMETRIZATION
#endif

#ifndef CGAL_POLYHEDRON_DEMO_NO_NEF
#  define CGAL_POLYHEDRON_DEMO_USE_NEF
#endif

#ifndef CGAL_POLYHEDRON_DEMO_NO_SURFACE_MESHER
#  define CGAL_POLYHEDRON_DEMO_USE_SURFACE_MESHER
#endif


// ==========================================================================
// CONCURRENCY
// ==========================================================================

#ifdef CONCURRENT_MESH_3

  // ==========================================================================
  // Concurrency activation
  // ==========================================================================

# define CGAL_MESH_3_LAZY_REFINEMENT_QUEUE
  #define CGAL_MESH_3_CONCURRENT_SCAN_TRIANGULATION
# define CGAL_MESH_3_CONCURRENT_REFINEMENT
  // In case some code uses CGAL_PROFILE, it needs to be concurrent
# define CGAL_CONCURRENT_PROFILE
//# define CGAL_CONCURRENT_MESH_3_VERBOSE

  // ==========================================================================
  // Locking strategy
  // ==========================================================================

# ifdef CGAL_MESH_3_CONCURRENT_REFINEMENT
//#   define CGAL_MESH_3_LOCKING_STRATEGY_CELL_LOCK
#   define CGAL_MESH_3_LOCKING_STRATEGY_SIMPLE_GRID_LOCKING
    const int LOCKING_GRID_NUM_CELLS_PER_AXIS = 5;
    const int FIRST_GRID_LOCK_RADIUS = 1;

#   ifdef CGAL_MESH_3_LOCKING_STRATEGY_CELL_LOCK
#   include <tbb/recursive_mutex.h>
    typedef tbb::recursive_mutex Cell_mutex_type; // CJTODO try others
#   endif
# endif

  // ==========================================================================
  // CJTODO: temp
  // ==========================================================================

# include <tbb/tbb.h>
  typedef tbb::queuing_mutex Global_mutex_type;
  extern Global_mutex_type g_global_mutex; // CJTODO: temporary

  // ==========================================================================
  // Concurrency Parameters
  // ==========================================================================

  const size_t ELEMENT_BATCH_SIZE = 3000;

  // ==========================================================================
  // Profiling
  // ==========================================================================

  // For abortion profiling, etc.
//# define CGAL_CONCURRENT_MESH_3_PROFILING
  
  // ==========================================================================
  // TBB
  // ==========================================================================

  // Use TBB malloc proxy (for all new/delete/malloc/free calls)
# include <tbb/tbbmalloc_proxy.h>

#endif // CONCURRENT_MESH_3
  
#define MESH_3_PROFILING

#endif // CGAL_DEMO_MESH_3_CONFIG_H
