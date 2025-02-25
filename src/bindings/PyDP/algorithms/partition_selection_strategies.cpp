// Provides bindings for Bounded Functions

#include "pybind11/complex.h"
#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "algorithms/partition-selection.h"

namespace py = pybind11;
namespace dp = differential_privacy;

using PyPartitionSelectionStrategy = std::unique_ptr<dp::PartitionSelectionStrategy>;

template <class Strategy>
PyPartitionSelectionStrategy CreatePartitionStrategy(double epsilon, double delta,
                                                     int max_partitions_contributed) {
  typename Strategy::Builder builder;
  builder.SetEpsilon(epsilon);
  builder.SetDelta(delta);
  builder.SetMaxPartitionsContributed(max_partitions_contributed);

  auto obj = builder.Build();
  if (!obj.ok()) {
    throw std::runtime_error(obj.status().ToString());
  }
  return std::move(obj.ValueOrDie());
}

void init_algorithms_partition_selection_strategies(py::module& m) {
  auto pyClass =
      py::class_<dp::PartitionSelectionStrategy, PyPartitionSelectionStrategy>(
          m, "PartitionSelectionStrategy",
          R"pbdoc(
        Base class for all (Ɛ, 𝛿)-differenially private partition selection strategies.
      )pbdoc");
  pyClass
      .def("should_keep", &dp::PartitionSelectionStrategy::ShouldKeep,
           py::arg("num_users"),
           R"pbdoc(
              Decides whether or not to keep a partition with `num_users` based on differential privacy parameters and strategy.
            )pbdoc")
      .attr("__module__") = "_partition_selection";

  m.def("create_truncated_geometric_partition_strategy",
        &CreatePartitionStrategy<dp::PreaggPartitionSelection>);
  m.def("create_laplace_partition_strategy",
        &CreatePartitionStrategy<dp::LaplacePartitionSelection>);
  m.def("create_gaussian_partition_strategy",
        &CreatePartitionStrategy<dp::GaussianPartitionSelection>);
}
